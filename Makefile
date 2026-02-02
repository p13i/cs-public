SHELL := /bin/bash
MAKEFLAGS += --always-make
MAKEFLAGS += --no-keep-going
BAZEL=USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk
BAZEL_TIMEOUT_CMD = env USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk  # otherwise treats USE_BAZEL_VERSION=8.4.2 as the command and fails).
DOCKER_BUILDKIT ?= 1
COMPOSE_DOCKER_CLI_BUILD ?= 1
COMPOSE_PROJECT_NAME ?= cs-dev
COMPOSE_BUILD_FLAGS ?= $(strip --parallel --no-cache --pull --force-rm)  # Force compose builds to skip cache and refresh layers every run.
COMPOSE_UP_FLAGS ?= --wait --remove-orphans
BAZEL_JOBS ?= $(shell command -v nproc >/dev/null 2>&1 && nproc || sysctl -n hw.ncpu 2>/dev/null || echo 8)
BAZEL_BUILD_MODE ?= fastbuild
BAZEL_BUILD_FLAGS ?= --noenable_workspace --enable_bzlmod --spawn_strategy=standalone --genrule_strategy=standalone -c $(BAZEL_BUILD_MODE) --jobs=$(BAZEL_JOBS)
BAZEL_BUILD_TARGETS ?= //cs/...
DOC_BASE ?= http://localhost:8080
DOC_HANDLER ?= demo/subscribe
DOC_OPTS ?=
# For timeout(1): use env so the command is a single executable (timeout
# otherwise treats USE_BAZEL_VERSION=8.4.2 as the command and fails).
# Force compose builds to skip cache and refresh layers every run.


# Put near top of Makefile
ifneq (,$(wildcard .env))
  include .env
  export $(shell sed -n 's/^[[:space:]]*\([A-Za-z_][A-Za-z0-9_]*\)[[:space:]]*=.*/\1/p' .env)
endif


# Color toggle: NO_COLOR=1 make help  -> no ANSI
ifeq ($(NO_COLOR),1)
CYAN :=
RESET :=
else
CYAN := \033[36m
RESET := \033[0m
endif

.DEFAULT_GOAL := up

.PHONY: a
a: app  ## Alias for `app`.

.PHONY: app
app:  ## Run the web app and tee logs to data/app.log.
	@$(BAZEL) run --config=release //cs/apps/trycopilot.ai:main -- "/data/" "$(PORT)" "$(CURDIR)" "${VERSION}" "${COMMIT}" \
		2>&1 | tee -a "/data/app.log"

.PHONY: app-bin
app-bin:  ## Runs the main app binary, assuming it is at `./bin/main`
	@"$(CURDIR)/bin/main" "/data/" "8080" "$(CURDIR)" "${VERSION}" "${COMMIT}" \
		2>&1 | tee -a "/data/app.log"

.PHONY: app-bin-build
app-bin-build:   ## Build a binary of the www/main.cc program and place it in `./bin/`
	@$(BAZEL) build --config=release //cs/apps/trycopilot.ai:main
	mkdir -p bin
	cp "$(CURDIR)/bazel-bin/cs/apps/trycopilot.ai/main" "$(CURDIR)/bin/main"
	chmod +x "$(CURDIR)/bin/main"

.PHONY: assert-no-diffs
assert-no-diffs:  ## Fail if repo has unstaged/staged or untracked changes.
	@git update-index -q --refresh
	@# Unstaged/staged diffs
	@if ! git diff --quiet --ignore-submodules -- && true; then \
	  git --no-pager diff --stat; \
	  echo "ERROR: unstaged changes."; \
	  exit 1; \
	fi
	@if ! git diff --cached --quiet --ignore-submodules -- && true; then \
	  git --no-pager diff --cached --stat; \
	  echo "ERROR: staged but uncommitted changes."; \
	  exit 1; \
	fi
	@# Untracked files
	@if [ -n "$$(git ls-files --others --exclude-standard)" ]; then \
	  echo "ERROR: untracked files:"; \
	  git ls-files --others --exclude-standard; \
	  exit 1; \
	fi

.PHONY: b
b: build  ## Alias for `build`.

.PHONY: base-image
base-image:  ## Build cs-app-base:latest and runtime-base:latest images (required for app builds, no pull).
	@echo "[base-image] Building base image (using local/cached images only, no pull)..."
	@DOCKER_BUILDKIT=$(DOCKER_BUILDKIT) docker build --pull=false -t cs-app-base:latest -f Dockerfile --target base .
	@echo "[base-image] Building runtime-base image (using local/cached images only, no pull)..."
	@DOCKER_BUILDKIT=$(DOCKER_BUILDKIT) docker build --pull=false -t runtime-base:latest -f Dockerfile --target runtime-base .

.PHONY: build
build:  ## Bazel build tuned for speed (override BAZEL_BUILD_FLAGS/BAZEL_BUILD_TARGETS).
	@echo "[build] bazel build $(BAZEL_BUILD_FLAGS) -- $(BAZEL_BUILD_TARGETS)"
	@${BAZEL} build $(BAZEL_BUILD_FLAGS) -- $(BAZEL_BUILD_TARGETS)

.PHONY: c
c: coverage  ## Alias for coverage.

.PHONY: c-serve
c-serve: coverage-serve  ## Alias for coverage-serve.

.PHONY: check_visibility
check_visibility:  ## Ensure Bazel target visibility for public use.
	@echo "[check_visibility] Turned off for debugging."
	# ${BAZEL} run //cs/devtools:check_visibility -- --allow-dirty --mode fix --repo_root $(shell pwd)

.PHONY: ci
ci: clean setup assert-no-diffs lint assert-no-diffs build test gha-bazel gha-docker gha-devcontainer  ## Uses all available checks.

.PHONY: ci-local
ci-local: gha-bazel gha-docker gha-devcontainer  ## Uses all available checks.

.PHONY: cite.pub
cite.pub: ## cite.pub rule (auto)
	@${BAZEL} run //cs/apps/cite.pub:main

.PHONY: cite.pub-bin
cite.pub-bin:  ## Runs the cite.pub binary, assuming it is at `./bin/cite_pub_main`
	@"$(CURDIR)/bin/cite_pub_main"

.PHONY: cite.pub-bin-build
cite.pub-bin-build:   ## Build a binary of the cite.pub program and place it in `./bin/`
	@$(BAZEL) build --config=release //cs/apps/cite.pub:main
	mkdir -p bin
	cp "$(CURDIR)/bazel-bin/cs/apps/cite.pub/main" "$(CURDIR)/bin/cite_pub_main"
	chmod +x "$(CURDIR)/bin/cite_pub_main"


.PHONY: clean
clean:  ## Remove all Bazel outputs (expunge).
	@${BAZEL} clean --expunge

.PHONY: clean-volumes
clean-volumes:  ## Remove Docker volumes (destructive).
	@set -euo pipefail; \
	echo "WARNING: This will remove all data volumes. Continue? [y/N]"; \
	read -r confirm; \
	if [ "$$confirm" = "y" ] || [ "$$confirm" = "Y" ]; then \
	  . bin/utils/docker.sh; \
	  remove_volumes database-volume docuseal_data gitea_data overleaf_redis_data overleaf_mongo_data penpot_assets_data penpot_postgres_data penpot_redis_data; \
	else \
	  echo "Cancelled."; \
	fi

.PHONY: code-viewer-refresh
code-viewer-refresh:  ## No-op; cs-public is baked into the code-viewer image.
	@echo "[code-viewer-refresh] No-op; cs-public is baked into the image."

.PHONY: codegen
codegen:  ## (Deprecated) Code generation driver.
	echo "No-op. Deprecated."

.PHONY: codegen-goldens
codegen-goldens: ## Regenerate codegen goldens for cs/net/proto/codegen.
	@${BAZEL} run //cs/devtools:codegen_goldens


.PHONY: coverage
coverage:  ## Run Bazel coverage for C++ tests and export LCOV.
	@set -euo pipefail; \
	  targets="$$( $(BAZEL) query 'kind("cc_test", //cs/...)' )"; \
	  if [ -z "$$targets" ]; then \
	    echo "[coverage] no cc_test targets found"; \
	    exit 1; \
	  fi; \
	  echo "[coverage] bazel coverage (cc_test targets)"; \
	  $(BAZEL) coverage --combined_report=lcov --test_output=errors -- $$targets; \
	  mkdir -p out/coverage; \
	  cp "bazel-out/_coverage/_coverage_report.dat" "out/coverage/lcov.info"; \
	  if command -v genhtml >/dev/null 2>&1; then \
	    genhtml "out/coverage/lcov.info" --output-directory "out/coverage/html" --ignore-errors negative >/dev/null; \
	    echo "[coverage] html report: out/coverage/html/index.html"; \
	  else \
	    echo "[coverage] genhtml not found; skipping HTML report"; \
	  fi; \
	  echo "[coverage] lcov info: out/coverage/lcov.info"; \
	  echo "[coverage] bazel report: bazel-out/_coverage/_coverage_report.dat"

.PHONY: coverage-serve
coverage-serve:  ## Serve coverage HTML output via make serve.
	@$(MAKE) serve FOLDER=out/coverage/html PORT=8087

.PHONY: dbuild
dbuild:  ## Build core Compose images with BuildKit.
	@echo "[dbuild] Building services"
	@DOCKER_BUILDKIT=$(DOCKER_BUILDKIT) COMPOSE_DOCKER_CLI_BUILD=$(COMPOSE_DOCKER_CLI_BUILD) \
	  COMPOSE_PROJECT_NAME=$(COMPOSE_PROJECT_NAME) \
	  docker compose build $(COMPOSE_BUILD_FLAGS)

.PHONY: dclean
dclean: ## Remove this project's containers/images/volumes and Docker caches.
	@COMPOSE_PROJECT_NAME=$(COMPOSE_PROJECT_NAME) docker compose down --rmi all --volumes --remove-orphans || true
	@docker buildx prune -af || true
	@docker builder prune -af || true

.PHONY: dclean-slate
dclean-slate:  ## Remove ALL Docker containers, images, volumes, and networks (clean slate).
	@echo "[dclean-slate] Stopping all containers..."
	@docker stop $$(docker ps -aq) 2>/dev/null || true
	@echo "[dclean-slate] Removing all containers..."
	@docker rm $$(docker ps -aq) 2>/dev/null || true
	@echo "[dclean-slate] Removing all images..."
	@docker rmi $$(docker images -q) -f 2>/dev/null || true
	@echo "[dclean-slate] Removing all volumes..."
	@docker volume rm $$(docker volume ls -q) 2>/dev/null || true
	@echo "[dclean-slate] Removing all networks..."
	@docker network prune -f || true
	@echo "[dclean-slate] Done. Docker is now a clean slate."

.PHONY: dcurl
dcurl:  ## Run curl in temp container on compose network. Usage: make dcurl URL="http://database-service:8080/rpc/query/" DATA='{"collection":"http_logs","steps":[]}'
	@NETWORK=$$(docker compose ps database-service --format '{{.Networks}}' 2>/dev/null | head -1 || echo "$(COMPOSE_PROJECT_NAME)_default"); \
	docker run --rm --network "$$NETWORK" curlimages/curl:latest \
	  -X POST "${URL}" \
	  -H "Content-Type: application/json" \
	  $(if $(DATA),-d '$(DATA)',)

.PHONY: dev
dev: expand images-load setup-volumes ## Run dev stack using Bazel-built images (no docker build). Requires local docker daemon for compose up.
	@set -euo pipefail; \
	make dup-dev; \
	TAIL="$${TAIL:-200}"; \
	# echo "[dev] Reattaching to logs (last $$TAIL lines; Ctrl-C to stop)…"; \
	# COMPOSE_PROJECT_NAME=cs-dev docker compose -f docker-compose.yml -f docker-compose.dev.yml logs -f --tail "$$TAIL"

.PHONY: devcontainer-bash
devcontainer-bash:  ## Open bash shell in devcontainer from macOS Terminal.
	@echo "[DEBUG] Connecting to container: $$(docker ps --filter "label=devcontainer.config_file" --format "{{.Names}}" | head -1)" >&2
	@docker exec -it -u root -w /Users/p13i/git/cs $$(docker ps --filter "label=devcontainer.config_file" --format "{{.Names}}" | head -1) bash

.PHONY: devcontainer-postCreateCommand
devcontainer-postCreateCommand:  ## Devcontainer provisioning steps.
	@set -euo pipefail; \
	ln -sf /workspaces/cs/.devcontainer/.bash_aliases ~/.bash_aliases; \
	apt-get update; \
	apt-get install -y --no-install-recommends ca-certificates curl gnupg lcov=2.0-4ubuntu2; \
	curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm-archive-keyring.gpg; \
	echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] https://apt.llvm.org/noble/ llvm-toolchain-noble-18 main" > /etc/apt/sources.list.d/llvm-toolchain-noble-18.list; \
	apt-get update; \
	apt-get install -y --no-install-recommends \
	  clang-format-18=1:18.1.8~++20240731025043+3b5b5c1ec4a3-1~exp1~20240731145144.92; \
	update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100; \
	update-alternatives --set clang-format /usr/bin/clang-format-18; \
	apt-get clean; \
	rm -rf /var/lib/apt/lists/*; \
	npm install -g @bazel/bazelisk; \
	$(MAKE) setup help

.PHONY: dexec
dexec:  ## Runs command inside active container.
	COMPOSE_PROJECT_NAME=$(COMPOSE_PROJECT_NAME) docker compose exec -u 0 -it ${SERVICE} ${COMMAND}


.PHONY: down
down:  ## Stops Compose services.
	COMPOSE_PROJECT_NAME=$(COMPOSE_PROJECT_NAME) docker compose down

.PHONY: down-wiki
down-wiki: ## download rule (auto)
	@${BAZEL} run //cs/ai/gpt:download_wikipedia -- 3 "$(CURDIR)/out/wiki"

.PHONY: dpurge
dpurge: ## Stop all containers and remove all Docker images (keeps volumes/networks).
	@set -euo pipefail; \
	echo "[dpurge] Stopping containers..."; \
	docker stop $$(docker ps -q) 2>/dev/null || true; \
	echo "[dpurge] Removing containers..."; \
	docker rm -f $$(docker ps -aq) 2>/dev/null || true; \
	echo "[dpurge] Removing images..."; \
	docker rmi -f $$(docker images -aq) 2>/dev/null || true; \
	echo "[dpurge] Done."

.PHONY: drun
drun: dbuild  ## SERVICE=web COMMAND="make build" make drun
	COMPOSE_PROJECT_NAME=$(COMPOSE_PROJECT_NAME) docker compose run --rm --no-deps ${SERVICE} /bin/sh -lc "${COMMAND}"

.PHONY: dup-dev
dup-dev:  ## Start dev stack detached (docker compose up) assuming Bazel images are loaded.
	@echo "[dup] Starting docker compose (detached) with prebuilt Bazel images…"; \
	COMPOSE_PROJECT_NAME=cs-dev COMPOSE_DOCKER_CLI_BUILD=0 \
	  docker compose -f docker-compose.yml -f docker-compose.dev.yml up -d

.PHONY: enforce_header_guards
enforce_header_guards:  ## Enforce header guards across the 'cs' tree.
	@${BAZEL} run //cs/devtools:enforce_header_guards -- --paths=cs --base_dir=$(shell pwd) --apply

.PHONY: expand
expand: ## Generate docker-compose.yml and Dockerfile from system.gpt.yml.
	@${BAZEL} run //cs/devtools:expand_system -- $(CURDIR)/system.gpt.yml
	@${BAZEL} run //cs/devtools:test_expand_system -- $(CURDIR)
	@${BAZEL} build //cs/apps/load-balancer:routing
	@cp bazel-bin/cs/apps/load-balancer/routing.json $(CURDIR)/routing.json 2>/dev/null || true

.PHONY: expand-check
expand-check: ## Check which services need rebuilding based on content hashes.
	@echo "[expand-check] Analyzing service images..."
	@docker compose config 2>/dev/null | awk '/^  [a-z]/ && /:$$/ {svc=$$1; sub(/:/, "", svc)} /^    image:/ {print svc, $$2}' | while read svc img; do \
	  if [ -n "$$img" ]; then \
	    if ! docker images -q "$$img" 2>/dev/null | grep -q .; then \
	      echo "  ⚠️  $$svc: $$img (rebuild needed)"; \
	    else \
	      echo "  ✓  $$svc: $$img (cached)"; \
	    fi; \
	  fi; \
	done

.PHONY: expand-test
expand-test: ## Run unit tests for expand_system.gpt.py.
	@python3 cs/devtools/test_expand_system_unit.gpt.py

.PHONY: fn-in-cmt
fn-in-cmt: ## Ensures that Python, C++, and BUILD files have comments with the filename at the top.
	@${BAZEL} run //cs/devtools:filenames_in_comments -- --root "$(CURDIR)"

.PHONY: font-demo
font-demo:  ## Render a text demo to the console.
	@${BAZEL} run //cs/apps/trycopilot.ai/text:draw_to_console

.PHONY: format-black
format-black:
	@${BAZEL} run //cs/devtools:format_black -- "$(CURDIR)/cs"

.PHONY: fp
fp: freeport # Alias for make freeport. ## fp rule (auto)

.PHONY: freeport
freeport:  ## Free TCP port $(PORT) by killing listeners (uses sudo).
	@echo "freeport disabled; skipping port clearing for PORT=$(PORT)"

.PHONY: gha
gha: ## gha rule (auto)
	@${BAZEL} run //cs/devtools:gha -- \
	  --workflow-yml "$(GHA_WORKFLOW_YML)" \
	  --repo-root "$(CURDIR)" \
	  ${GHA_EVENT:+--event "$(GHA_EVENT)"} \
	  ${GHA_JOB:+--job "$(GHA_JOB)"} \
	  ${GHA_ACT_FLAGS:+--act-flags '$(GHA_ACT_FLAGS)'} \
	  ${GHA_ENV_FILE:+--env-file '$(GHA_ENV_FILE)'} \
	  ${GHA_SECRETS_FILE:+--secrets-file '$(GHA_SECRETS_FILE)'}

.PHONY: gha-agents
gha-agents: ## gha-agents rule (auto)
	make gha GHA_WORKFLOW_YML=.github/workflows/agents.yml

.PHONY: gha-bazel
gha-bazel: ## gha-bazel rule (auto)
	make gha GHA_WORKFLOW_YML=.github/workflows/bazel.yml

.PHONY: gha-devcontainer
gha-devcontainer: ## gha-devcontainer rule (auto)
	make gha GHA_WORKFLOW_YML=.github/workflows/devcontainer.yml

.PHONY: gha-docker
gha-docker: ## gha-docker rule (auto)
	make gha GHA_WORKFLOW_YML=.github/workflows/docker.yml

.PHONY: gpt
gpt: ## Runs toy GPT implementation.
	@${BAZEL} run //cs/ai/gpt:main -- --train

.PHONY: help
help: ## Show this help message: Self-documenting Make help [ChatGPT]
	@printf "Usage: make <target> [VAR=val]\n\nTargets:\n"
	@grep -h -E '^[a-zA-Z0-9_.-]+:.*?## ' $(MAKEFILE_LIST) | \
	  awk -v c='$(CYAN)' -v r='$(RESET)' 'BEGIN{FS=":.*?## "} {printf "  %s%-24s%s %s\n", c, $$1, r, $$2}'

.PHONY: help-%
help-%: ## Show help for a single target: `make help-up`
	@grep -h -E '^$*[:].*?## ' $(MAKEFILE_LIST) | \
	  awk 'BEGIN {FS=":.*?## "}{printf "$(CYAN)%s$(RESET): %s\n", $$1, $$2}'


.PHONY: help.txt
help.txt: ## Write a plain-text snapshot of `make help` (strip ANSI + dir chatter).
	@NO_COLOR=1 $(MAKE) --no-print-directory help \
	| sed -E '/^make(\[[0-9]+\])?: (Entering|Leaving) directory/d' \
	| awk '{gsub(/\033\[[0-9;]*[mK]/,"")}1' \
	> $@


.PHONY: images-build
images-build: expand base-image  ## Build Docker images using Dockerfiles (fast layer caching, no pull).
	@echo "[images-build] Building Docker images with BuildKit (no pull - local images only)..."
	@DOCKER_BUILDKIT=$(DOCKER_BUILDKIT) COMPOSE_DOCKER_CLI_BUILD=$(COMPOSE_DOCKER_CLI_BUILD) \
	  COMPOSE_PROJECT_NAME=$(COMPOSE_PROJECT_NAME) \
	  docker compose build --no-cache --force-rm

.PHONY: images-load
images-load: images-build  ## Alias for images-build (backward compatibility).

.PHONY: images-load-dev
images-load-dev: images-build  ## Alias for images-build (backward compatibility).

.PHONY: inline-using
inline-using: ## Remove usings from C++ header files.
	@echo "[inline-using] Inlining anonymous-namespace using declarations across .hh files"
	@${BAZEL} run //cs/devtools:inline_using -- --repo_root="$(shell pwd)"

.PHONY: l
l: lint  ## Alias for `lint`.

.PHONY: lint
lint: expand  ## Run all linters/formatters.
	make codegen-goldens
	make lint-newfile-examples lint-makefile
	make fn-in-cmt inline-using check_visibility enforce_header_guards help.txt
	make lint-bazel lint-cc lint-md-html-yaml lint-rust \
	  lint-python lint-makefile lint-csharp lint-java \
	  lint-go lint-javascript lint-typescript lint-julia \
	  lint-bash lint-c

.PHONY: lint-add-acls
lint-add-acls:  ## Dry-run adding ACLs to public files.
	@${BAZEL} run //scripts:add_acls -- --root $(pwd) --dry-run

.PHONY: lint-bash
lint-bash:  ## Enforce 60-col width for Bash lab sources.
	@echo "lint-bash is not implemented!"

.PHONY: lint-bazel
lint-bazel:  ## Lint Bazel files with buildifier.
	# Pin to a commit before buildifier adopted Go slices.Collect (Go 1.23).
	go run github.com/bazelbuild/buildtools/buildifier@b163fcf72b7def638f364ed129c9b28032c1d39b -r .

.PHONY: lint-c
lint-c:  ## Enforce 60-col width for C lab sources.
	@find cs/labs/c -type f \( -name '*.c' -o -name '*.h' \) \
	  -print0 | xargs -0 awk -v m=60 'length>m{print FILENAME ":" NR; e=1} END{exit e}'

.PHONY: lint-cc
lint-cc:  ## Format C/C++ sources with clang-format.
	find . -path '*/node_modules/*' -prune -o \
	  -type f \( -name '*.c' -o -name '*.h' -o -name '*.hh' -o -name '*.cc' \) -print \
	  | xargs clang-format -i

.PHONY: lint-csharp
lint-csharp:  ## Enforce 60-col width for C# lab sources.
	@echo "lint-csharp is not implemented!"

.PHONY: lint-go
lint-go:  ## Enforce 60-col width for Go lab sources.
	@echo "lint-go is not implemented!"

.PHONY: lint-java
lint-java:  ## Enforce 60-col width for Java lab sources.
	@echo "lint-java is not implemented!"

.PHONY: lint-javascript
lint-javascript:  ## Enforce 60-col width for JS lab sources.
	@echo "lint-javascript is not implemented!"

.PHONY: lint-julia
lint-julia:  ## Enforce 60-col width for Julia lab sources.
	@echo "lint-julia is not implemented!"

.PHONY: lint-makefile
lint-makefile: ## Lint/organize the top-level Makefile in-place (backup saved).
	@${BAZEL} run //cs/devtools:lint_makefile -- --in-place --wrap 0 "$(CURDIR)/Makefile"

.PHONY: lint-md
lint-md:  ## Format Markdown files with Prettier.
	npx prettier@3.7.4 --write --print-width 60 --trailing-comma=none --prose-wrap always '**/*.md'

.PHONY: lint-md-html-yaml
lint-md-html-yaml:  ## Format Markdown/HTML/YAML/JSON with Prettier.
	npx prettier@3.7.4 --write --print-width 60 --trailing-comma=none --prose-wrap always '**/*.{md,html,yml,yaml,json}'

.PHONY: lint-newfile-examples
lint-newfile-examples:  ## Generate example files to validate templates.
	NEWFILE=cs/devtools/examples/example.hh make newfile
	NEWFILE=cs/devtools/examples/example.cc make newfile
	NEWFILE=cs/devtools/examples/example_test.cc make newfile

.PHONY: lint-python
lint-python: format-black ## Format Python code with Black.

.PHONY: lint-rust
lint-rust:  ## Format Rust files with Prettier Rust plugin.
	@echo "lint-rust is skipped (no parser available)"

.PHONY: lint-typescript
lint-typescript:  ## Enforce 60-col width for TS lab sources.
	@echo "lint-typescript is not implemented!"

.PHONY: load
load:  ## Generate sustained HTTP load (default: 10K QPS for 60s). Usage: make load
	${BAZEL} run //cs/devtools:load_generator -- \
		"https://dev.www.trycopilot.ai" \
		--qps=100 \
		--duration=60

.PHONY: load-find-max-qps
load-find-max-qps:  ## Find maximum QPS with 99.9%+ success rate using binary search.
	${BAZEL} run //cs/devtools:load_generator -- \
		"https://dev.www.trycopilot.ai" \
		--find-max-qps \
		--test-duration=30 \
		--min-qps=1 \
		--max-qps=1000 \
		--target-success-rate=99.9

.PHONY: logs
logs: ## Follow docker compose logs for core services (Ctrl-C to stop).
	docker compose logs -f

.PHONY: logs-db
logs-db: ## Follow docker compose logs for database-service (Ctrl-C to stop).
	@TAIL="$${TAIL:-200}"; \
	echo "Tailing database-service logs (last $$TAIL lines)…"; \
	COMPOSE_PROJECT_NAME=$(COMPOSE_PROJECT_NAME) docker compose logs -f --tail "$$TAIL" database-service

.PHONY: logs-save
logs-save: ## Check logs for Save Page performance issues (cite-pub).
	@echo "=== cite-pub Service Logs (Save Page requests) ==="; \
	docker logs cs-cite-pub 2>&1 | grep -E "(POST /save|Processed HTTP request|Fetching URL)" | tail -30; \
	echo ""; \
	echo "=== Full cite-pub logs (last 50 lines) ==="; \
	docker logs cs-cite-pub 2>&1 | tail -50

.PHONY: mnist
mnist:  ## Run the MNIST demo with the given model.json.
	@${BAZEL} run //cs/ai:mnist -- $(shell pwd)/model.json

.PHONY: mobile-android
mobile-android: ## mobile-android rule (auto)
	@${BAZEL} run //cs/apps/mobile:expo_android

.PHONY: mobile-ios
mobile-ios: ## mobile-ios rule (auto)
	@${BAZEL} run //cs/apps/mobile:expo_ios

.PHONY: mobile-start
mobile-start: ## mobile-start rule (auto)
	@${BAZEL} run //cs/apps/mobile:expo_start

.PHONY: move_cc
move_cc:  ## Refactor/move C++ files (SRC -> DST).
	@${BAZEL} run //cs/devtools:refactor_move -- --src ${SRC} --dst ${DST} --apply --base_dir=$(shell pwd)

.PHONY: my-ip
my-ip:  ## Print public IP for Namecheap allowlist.
	@curl -fsSL https://ifconfig.me

.PHONY: newfile
newfile:  ## Create a new file from template (requires NEWFILE=path).
	@${BAZEL} run //cs/devtools:newfile -- $(shell pwd) ${NEWFILE}

.PHONY: ngrok
ngrok:  ## Expose local port 8080 at fixed domain via ngrok.
	ngrok http --url=www.trycopilot.ai 80

.PHONY: own
own:  ## Take ownership of the repo files (requires sudo).
	@if [ -n "$$(find "$(shell pwd)" -maxdepth 1 -not -user "$(USER)" 2>/dev/null)" ]; then \
	  sudo chown -R "$(USER)":"$(USER)" "$(shell pwd)"; \
	else \
	  echo "Files already owned by $(USER), skipping chown."; \
	fi

.PHONY: parse-logs
parse-logs:  ## Parse /app/data/app.log into /app/data/app_logs.
	$(BAZEL) run //cs/devtools:parse_logs -- --input "/data/app.log" --outdir "/data/app_logs"

.PHONY: parse-logs-bin
parse-logs-bin:  ## Runs the parse_logs binary, assuming it is at `./bin/parse_logs`
	@"$(CURDIR)/bin/parse_logs" --input "/data/app.log" --outdir "/data/app_logs"

.PHONY: parse-logs-job
parse-logs-job:  ## Run parse-logs every 300s until interrupted.
	@echo "Running parse-logs every 300s (Ctrl-C to stop)…"
	@trap 'exit 0' INT TERM; \
	while true; do \
	  echo "[$$(date -Is)] parse-logs"; \
	  $(MAKE) --no-print-directory parse-logs-bin; \
	  sleep 300; \
	done


.PHONY: prompt
prompt:  ## Send a prompt to https://www.trycopilot.ai/prompt/?prompt=... (GET) and print the reply.
	@curl -fsSL -G --data-urlencode "prompt=$$MSG" "https://www.trycopilot.ai/prompt/"

.PHONY: pub
pub: ## Publishes the source code, respecting acl.json files, into ../cs-public
	set -a
	. ./.env
	set +a
	@${BAZEL} run //cs/devtools:publish_code -- $(shell pwd) $(shell pwd)/../cs-public
	@cd ../cs-public && git add . && git status && git commit -m "[publish] $${VERSION} ($${COMMIT})"

.PHONY: r
r: release

.PHONY: release
release: update-env ## Updates the .env file with the latest COMMIT hash and commits a release.
	set -a
	. ./.env
	set +a
	git add .env
	git commit -m "[release] $${VERSION} ($${COMMIT})"
	git push


.PHONY: serve
serve: ## serve rule (auto)
	@set -euo pipefail; \
	: $${FOLDER:?Usage: FOLDER=/path/to/dir PORT=8087 make serve}; \
	PORT="$${PORT:-8080}"; \
	if [ ! -d "$$FOLDER" ]; then \
	  echo "Error: folder not found: $$FOLDER" >&2; exit 2; \
	fi; \
	echo "Serving $$FOLDER at http://127.0.0.1:$$PORT (Ctrl-C to stop)"; \
	if python3 -m http.server --help 2>&1 | grep -q -- '--directory'; then \
	  exec python3 -m http.server "$$PORT" --bind 127.0.0.1 --directory "$$FOLDER"; \
	else \
	  ( cd "$$FOLDER" && exec python3 -m http.server "$$PORT" --bind 127.0.0.1 ); \
	fi

.PHONY: setup
setup: setup-git setup-mobile ## One-time dev setup (linters, DB dirs, Node).

.PHONY: setup-git
setup-git:  ## Sets common git configs for users of this repo.
	@if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then \
	  git config include.path ".gitconfig"; \
	fi

.PHONY: setup-mobile
setup-mobile:  ## Installs NPM packages for mobile app.
	@cd cs/apps/mobile && npm install

.PHONY: setup-volumes
setup-volumes:  ## Create Docker volumes if they don't exist.
	@set -euo pipefail; \
	. cs/devtools/docker.sh; \
	create_volumes_if_not_exist database-volume docuseal_data gitea_data overleaf_redis_data overleaf_mongo_data penpot_assets_data penpot_postgres_data penpot_redis_data

.PHONY: stash
stash:  ## Commit all changes to branch $(BRANCH), push, then switch back to main.
	git checkout -b $(BRANCH) && git add . && git commit -m "$(BRANCH)" && git push --set-upstream origin $(BRANCH) && git checkout main

.PHONY: status
status: ## Watch docker compose status for core services (Ctrl+C to exit).
	@set -euo pipefail; \
	if command -v watch >/dev/null 2>&1; then \
	  exec watch -n2 -- COMPOSE_PROJECT_NAME=$(COMPOSE_PROJECT_NAME) docker compose ps base www-trycopilot-ai ngrok; \
	fi; \
	echo "watch not found; showing updates every 2s (Ctrl+C to exit)"; \
	while true; do \
	  clear; \
	  COMPOSE_PROJECT_NAME=$(COMPOSE_PROJECT_NAME) docker compose ps base www-trycopilot-ai ngrok; \
	  sleep 2; \
	done

.PHONY: t
t: test  ## Alias for `test`.

.PHONY: test
test:  ## Runs all tests with full output.
	@${BAZEL} test --test_output=all -- //cs/...

.PHONY: test-bash
test-bash:  ## Run the Bash lab test.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/bash:hello_world_test

.PHONY: test-c
test-c:  ## Run the C lab test.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/c:hello_world_test

.PHONY: test-cpp
test-cpp:  ## Run the C++ lab test.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/cpp:hello_world_test

.PHONY: test-csharp
test-csharp:  ## Run the C# lab test.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/csharp:hello_world_test

.PHONY: test-forecast
test-forecast:  ## Run the Pydantic forecast demo tests.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/demos/pydantic:test_pipeline

.PHONY: test-go
test-go:  ## Run the Go lab test.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/go:hello_world_test

.PHONY: test-java
test-java:  ## Run the Java lab test.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/java:hello_world_test

.PHONY: test-javascript
test-javascript:  ## Run the JavaScript lab test.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/javascript:hello_world_test

.PHONY: test-json
test-json:  ## Run JSON parser/serializer DSL tests.
	@${BAZEL} test --test_output=errors \
	  //cs/net/json:absurd_json_test

.PHONY: test-julia
test-julia:  ## Run the Julia lab test.
	@PATH="$(HOME)/.local/bin:$$PATH" \
	  ${BAZEL} test --test_output=errors \
	  //cs/labs/julia:hello_world_test

.PHONY: test-labs
test-labs: ## Run all lab hello-world tests.
	make test-bash test-c test-cpp test-csharp test-go \
	  test-java test-javascript test-julia test-python \
	  test-rust test-typescript

.PHONY: test-pydantic
test-pydantic:  ## Run all Pydantic demo tests.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/demos/pydantic:all

.PHONY: test-python
test-python:  ## Run the Python lab test.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/python:hello_world_test

.PHONY: test-rust
test-rust:  ## Run the Rust lab test.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/rust:hello_world_test

.PHONY: test-typescript
test-typescript:  ## Run the TypeScript lab test.
	@${BAZEL} test --test_output=errors \
	  //cs/labs/typescript:hello_world_test

.PHONY: trees-test
trees-test:  ## Run tree view tests in cs/q/trees.
	@${BAZEL} test --test_output=errors \
	  //cs/q/trees:view_from_left_test \
	  //cs/q/trees:view_from_right_test \
	  //cs/q/trees:view_from_top_test

.PHONY: up
up: images-build setup-volumes ## Bring up the production stack (docker-compose.yml).
	@set -euo pipefail; \
	echo "[up] freeport disabled; leaving ports 3001/3002 unchanged"; \
	echo "[up] Ensuring Docker is running..."; \
	if ! docker info >/dev/null 2>&1; then \
	  echo "ERROR: Docker daemon is not running. Please start Docker and try again."; \
	  exit 1; \
	fi; \
	echo "[up] Starting all services..."; \
	if ! COMPOSE_PROJECT_NAME=cs docker compose up -d $(COMPOSE_UP_FLAGS) 2>&1; then \
	  echo "ERROR: docker compose up failed"; \
	  echo "Attempting to diagnose..."; \
	  COMPOSE_PROJECT_NAME=cs docker compose ps 2>&1 || true; \
	  echo "Try: make down && make up"; \
	  exit 1; \
	fi; \
	echo "[up] Force-recreating service-registry so it uses the newly built image"; \
	COMPOSE_PROJECT_NAME=cs docker compose up -d --force-recreate service-registry 2>&1 || true; \
	echo "[up] Waiting for services to initialize..."; \
	sleep 3; \
	echo "[up] Checking service status..."; \
	ps_output=$$(COMPOSE_PROJECT_NAME=cs docker compose ps --format json 2>/dev/null || echo "[]"); \
	if [ -n "$$ps_output" ] && [ "$$ps_output" != "[]" ]; then \
	  running=$$(echo "$$ps_output" | jq -r '[.[] | select(.State == "running")] | length' 2>/dev/null | head -1 || echo "0"); \
	  total=$$(echo "$$ps_output" | jq -r 'length' 2>/dev/null | head -1 || echo "0"); \
	  if [ -n "$$running" ] && [ -n "$$total" ] && [ "$$running" != "" ] && [ "$$total" != "" ]; then \
	    if [ "$$total" -gt 0 ] 2>/dev/null; then \
	      echo "[up] Services running: $$running/$$total"; \
	      if [ "$$running" -lt "$$total" ] 2>/dev/null; then \
	        echo "WARNING: Some services may not be running. Check logs with: make logs"; \
	        COMPOSE_PROJECT_NAME=cs docker compose ps --format "table {{.Name}}\t{{.Status}}" 2>&1 | grep -v "running" || true; \
	      fi; \
	    fi; \
	  fi; \
	fi; \
	echo "[up] Stack deployed. Status:"; \
	COMPOSE_PROJECT_NAME=cs docker compose ps 2>&1 || true; \
	echo ""; \
	echo "Tail logs: make logs [TAIL=200]"


.PHONY: update-env
update-env: ## update-env rule (auto)
	$(BAZEL) run //cs/devtools:update_env -- --env "$(CURDIR)/.env"

