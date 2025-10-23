SHELL := /bin/bash
MAKEFLAGS += --always-make
BAZEL=USE_BAZEL_VERSION=8.4.2 npx @bazel/bazelisk
DC := docker compose
DOCS_PORT ?= 8000
INTERVAL ?= 300
POLL ?= 420
REPO ?= $(shell git rev-parse --show-toplevel)
SF_OUT_DIR ?= out/singlefile
SF_URL ?=
SINGLEFILE_IMAGE ?= singlefile 
SUDO ?= sudo
UPSTREAM ?=
VENV ?= .venv
WWW_HOST ?= 127.0.0.1
WWW_PORT ?= 8081
TIMESTAMP := $(shell date -u +"%Y%m%dT%H%M%SZ")
BASENAME := $(TIMESTAMP)
OUT := $(SF_OUT_DIR)/$(BASENAME).html
WRAP ?= 0
GHA_WORKFLOW_YML ?= .github/workflows/bazel.yml
GHA_REPO_ROOT    ?= $(CURDIR)

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

.DEFAULT_GOAL := Help

.PHONY=a
a: app  ## Alias for `app`.

.PHONY=app
app:  setup-db-dirs ## Run the web app and tee logs to data/app.log.
	@$(BAZEL) run --config=release //cs/www/app:main -- "$(CURDIR)/data/" "$(PORT)" "$(CURDIR)" "${VERSION}" "${COMMIT}" \
		2>&1 | tee -a "$(CURDIR)/data/app.log"

.PHONY=assert-no-diffs
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

.PHONY=b
b: build  ## Alias for `build`.

.PHONY=build
build:  ## Builds the project.
	@${BAZEL} build --noenable_workspace --enable_bzlmod --spawn_strategy=standalone --genrule_strategy=standalone --config=debug --jobs=32 -- //cs/...

.PHONY=build-app
build-app:  ## Build the app binary (debug flags enabled).
	@${BAZEL} build //cs/www/app:main --color=yes --sandbox_debug
# 	@$(BAZEL) build //cs/www/app:main
# 	@$(BAZEL) build --config=release //cs/www/app:main

.PHONY=build-index.js
build-index.js:  ## Builds the web bundle (index.js) via Bazel.
	@${BAZEL} build //cs/www/app:index.js

.PHONY=build-up
build-up: dbuild

.PHONY=build-up-in-workflow
build-up-in-workflow: build-up ## Builds a Compose project for the Github Action workflow context.

.PHONY=cg
cg: codegen  ## Alias for `codegen`.

.PHONY=check_visibility
check_visibility:  ## Ensure Bazel target visibility for public use.
	${BAZEL} run //cs/devtools:check_visibility -- --allow-dirty --mode fix --repo_root $(shell pwd)

.PHONY=ci
ci: clean setup assert-no-diffs lint assert-no-diffs build test gha-bazel gha-docker gha-devcontainer  ## Uses all available checks.

.PHONY=ci-local
ci-local: gha-bazel gha-docker gha-devcontainer  ## Uses all available checks.

.PHONY=cite.pub
cite.pub: ## cite.pub rule (auto)
	@${BAZEL} run //cs/www/cite.pub:main

.PHONY=cite.pub-mkdirs
cite.pub-mkdirs: ## cite.pub-mkdirs rule (auto)
	mkdir -p ./data
	mkdir -p ./data/cite.pub
	mkdir -p ./data/cite.pub/resources
	mkdir -p ./data/cite.pub/lookups
	mkdir -p ./data/cite.pub/contents

.PHONY=clean
clean:  ## Remove all Bazel outputs (expunge).
	@${BAZEL} clean --expunge

.PHONY=clean-docker
clean-docker: ## Remove this project's containers/images/volumes and Docker caches.
	@docker compose down --rmi all --volumes --remove-orphans || true
	@docker buildx prune -af || true
	@docker builder prune -af || true
	@docker system prune -af --volumes || true
	@docker rmi -f cs-app-base:latest || true

.PHONY=codegen
codegen:  ## (Deprecated) Code generation driver.
	echo "No-op. Deprecated."

.PHONY=dbuild
dbuild:  ## Builds a Compose project.
	docker build -t cs-app-base:latest -f Dockerfile .

.PHONY=dexec
dexec:  ## Runs command inside active container.
	docker compose exec -it ${SERVICE} ${COMMAND}

.PHONY=down
down:  ## Stops Compose services.
	$(DC) down

.PHONY=drun
drun: dbuild  ## SERVICE=web COMMAND="make build" make drun
	docker compose run --rm --no-deps ${SERVICE} /bin/sh -lc "${COMMAND}"

.PHONY=enforce_header_guards
enforce_header_guards:  ## Enforce header guards across the 'cs' tree.
	@${BAZEL} run //cs/devtools:enforce_header_guards -- --paths=cs --base_dir=$(shell pwd) --apply

.PHONY=font-demo
font-demo:  ## Render a text demo to the console.
	@${BAZEL} run //cs/www/app/text:draw_to_console

.PHONY=freeport
freeport:  ## Free TCP port $(PORT) by killing listeners (uses $(SUDO)).
	@if [ -z "$(PORT)" ]; then \
	  echo "Usage: make freeport PORT=8081"; exit 1; \
	fi
	@echo "Freeing TCP port $(PORT)…"
	@set -euo pipefail; \
	if command -v fuser >/dev/null 2>&1; then \
	  $(SUDO) fuser -k -TERM "$(PORT)"/tcp || true; \
	  sleep 0.3; \
	  if $(SUDO) ss -ltn 'sport = :'$(PORT) | grep -q LISTEN; then \
	    echo "Process still listening; sending SIGKILL…"; \
	    $(SUDO) fuser -k -KILL "$(PORT)"/tcp || true; \
	  fi; \
	else \
	  pids=$$($(SUDO) lsof -t -iTCP:$(PORT) -sTCP:LISTEN 2>/dev/null | tr '\n' ' '); \
	  if [ -n "$$pids" ]; then \
	    $(SUDO) kill -TERM $$pids || true; \
	    sleep 0.3; \
	    $(SUDO) kill -KILL $$pids || true; \
	  fi; \
	fi; \
	if $(SUDO) ss -ltn 'sport = :'$(PORT) | grep -q LISTEN; then \
	  echo "ERROR: port $(PORT) is still in use."; exit 2; \
	else \
	  echo "OK: port $(PORT) is free."; \
	fi

.PHONY=gha
gha: ## gha rule (auto)
	@${BAZEL} run //cs/devtools:gha -- \
	  --workflow-yml "$(GHA_WORKFLOW_YML)" \
	  --repo-root "$(GHA_REPO_ROOT)" \
	  ${GHA_EVENT:+--event "$(GHA_EVENT)"} \
	  ${GHA_JOB:+--job "$(GHA_JOB)"} \
	  ${GHA_ACT_FLAGS:+--act-flags '$(GHA_ACT_FLAGS)'} \
	  ${GHA_ENV_FILE:+--env-file '$(GHA_ENV_FILE)'} \
	  ${GHA_SECRETS_FILE:+--secrets-file '$(GHA_SECRETS_FILE)'}

.PHONY=gha-bazel
gha-bazel: ## gha-bazel rule (auto)
	make gha WORKFLOW_YML=.github/workflows/bazel.yml

.PHONY=gha-devcontainer
gha-devcontainer: ## gha-devcontainer rule (auto)
	make gha WORKFLOW_YML=.github/workflows/devcontainer.yml

.PHONY=gha-docker
gha-docker: ## gha-docker rule (auto)
	make gha WORKFLOW_YML=.github/workflows/docker.yml

.PHONY=gpt
gpt: ## Runs toy GPT implementation.
	@${BAZEL} run //cs/ai/gpt:main -- --train

.PHONY=help
help: ## Show this help message: Self-documenting Make help [ChatGPT]
	@printf "Usage: make <target> [VAR=val]\n\nTargets:\n"
	@grep -h -E '^[a-zA-Z0-9_.-]+:.*?## ' $(MAKEFILE_LIST) | \
	  awk -v c='$(CYAN)' -v r='$(RESET)' 'BEGIN{FS=":.*?## "} {printf "  %s%-24s%s %s\n", c, $$1, r, $$2}'

.PHONY=help-%
help-%: ## Show help for a single target: `make help-up`
	@grep -h -E '^$*[:].*?## ' $(MAKEFILE_LIST) | \
	  awk 'BEGIN {FS=":.*?## "}{printf "$(CYAN)%s$(RESET): %s\n", $$1, $$2}'


.PHONY=help.txt
help.txt: ## Write a plain-text snapshot of `make help` (strip ANSI + dir chatter).
	@NO_COLOR=1 $(MAKE) --no-print-directory help \
	| sed -E '/^make(\[[0-9]+\])?: (Entering|Leaving) directory/d' \
	| awk '{gsub(/\033\[[0-9;]*[mK]/,"")}1' \
	> $@

.PHONY=inline-using
inline-using: ## Remove usings from C++ header files.
	@echo "[inline-using] Inlining anonymous-namespace using declarations across .hh files"
	bazel run //cs/devtools:inline_using -- --repo_root="$(shell pwd)"

.PHONY=l
l: lint  ## Alias for `lint`.

.PHONY=lint
lint:  ## Run all linters/formatters.
	make lint-newfile-examples lint-makefile
	make inline-using check_visibility enforce_header_guards help.txt
	make lint-bazel lint-cc lint-md-html-yaml lint-rust lint-python lint-makefile

.PHONY=lint-add-acls
lint-add-acls:  ## Dry-run adding ACLs to public files.
	@${BAZEL} run //scripts:add_acls -- --root $(pwd) --dry-run

.PHONY=lint-bazel
lint-bazel:  ## Lint Bazel files with buildifier.
	go run github.com/bazelbuild/buildtools/buildifier@latest -r .

.PHONY=lint-cc
lint-cc:  ## Format C/C++ sources with clang-format.
	find . -iname *.c -o -iname *.h -o -iname *.hh -o -iname *.cc | xargs clang-format -i

.PHONY=lint-makefile
lint-makefile: ## Lint/organize the top-level Makefile in-place (backup saved).
	@${BAZEL} run //cs/devtools:lint_makefile -- --in-place --wrap $(WRAP) "$(CURDIR)/Makefile"

.PHONY=lint-md-html-yaml
lint-md-html-yaml:  ## Format Markdown/HTML/YAML/JSON with Prettier.
	npx prettier --write --print-width 60 --trailing-comma=none --prose-wrap always '**/*.{md,html,yml,yaml,json}'

.PHONY=lint-newfile-examples
lint-newfile-examples:  ## Generate example files to validate templates.
	NEWFILE=cs/devtools/examples/example.hh make newfile
	NEWFILE=cs/devtools/examples/example.cc make newfile
	NEWFILE=cs/devtools/examples/example_test.cc make newfile

.PHONY=lint-python
lint-python:  ## Format Python code with Black.
	python3 -m black .

.PHONY=lint-rust
lint-rust:  ## Format Rust files with Prettier Rust plugin.
	npm install prettier-plugin-rust
	npx prettier --write **/*.rs

.PHONY=loop
loop:  ## Re-run `make app` until it succeeds.
	until make app; do sleep 1; done

.PHONY=mnist
mnist:  ## Run the MNIST demo with the given model.json.
	@${BAZEL} run //cs/ai:mnist -- $(shell pwd)/model.json

.PHONY=move_cc
move_cc:  ## Refactor/move C++ files (SRC -> DST).
	@${BAZEL} run //cs/devtools:refactor_move -- --src ${SRC} --dst ${DST} --apply --base_dir=$(shell pwd)

.PHONY=newfile
newfile:  ## Create a new file from template (requires NEWFILE=path).
	@${BAZEL} run //cs/devtools:newfile -- $(shell pwd) ${NEWFILE}

.PHONY=ngrok
ngrok:  ## Expose local port 8080 at fixed domain via ngrok.
	ngrok http --url=cs.p13i.io 8080

.PHONY=parse-logs
parse-logs:  ## Parse /app/data/app.log into /app/data/app_logs.
	$(BAZEL) run //cs/devtools:parse_logs -- --input "/app/data/app.log" --outdir "/app/data/app_logs"

.PHONY=parse-logs-job
parse-logs-job:  ## Run parse-logs every $(INTERVAL)s until interrupted.
	@echo "Running parse-logs every $(INTERVAL)s (Ctrl-C to stop)…"
	@trap 'exit 0' INT TERM; \
	while true; do \
	  echo "[$$(date -Is)] parse-logs"; \
	  $(MAKE) --no-print-directory parse-logs; \
	  sleep $(INTERVAL); \
	done

.PHONY=probe
probe:  ## Run the HTTP prober utility.
	@${BAZEL} run //cs/www/app:prober

.PHONY=pub
pub: ## Publishes the source code, respecting acl.json files, into ../cs-public
	set -a
	[ -f .env ] && . ./.env
	set +a
	@${BAZEL} run //cs/devtools:publish_code -- $(shell pwd) $(shell pwd)/../cs-public
	@cd ../cs-public && git add . && git status && git commit -m "Publish source at $${COMMIT} ($${VERSION})" && git push

.PHONY=pull-singlefile-image
pull-singlefile-image:
	docker pull capsulecode/singlefile && docker tag capsulecode/singlefile $(SINGLEFILE_IMAGE)

.PHONY=r
r: release

.PHONY=release
release: update-env ## Updates the .env file with the latest COMMIT hash and commits a release.
	set -a
	[ -f .env ] && . ./.env
	set +a
	git add .env
	git commit -m "[release] $${VERSION:-unknown} ($${COMMIT})"
	git push


.PHONY=serve
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

.PHONY=serve-help
serve-help: help.md ## Serve help.md rendered at http://localhost:$(DOCS_PORT)/ using a venv-isolated 'grip'
	@set -e; \
	if [ ! -x "$(VENV)/bin/grip" ]; then \
	  echo "Creating venv at $(VENV) and installing grip…"; \
	  python3 -m venv "$(VENV)"; \
	  "$(VENV)/bin/pip" install --upgrade pip; \
	  "$(VENV)/bin/pip" install grip; \
	fi; \
	echo "Serving help.md at http://localhost:$(DOCS_PORT)/ (Ctrl-C to stop)…"; \
	"$(VENV)/bin/grip" help.md $(DOCS_PORT)

.PHONY=serve-site
serve-site:  ## Serve ./site via a local HTTP server.
	python3 -m http.server --directory site

.PHONY=setup
setup: setup-lint setup-db-dirs  ## One-time dev setup (linters, DB dirs, Node).
	# Install NPM
	curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
	sudo apt-get install -y nodejs
	# Print versions
	node -v
	npm -v

.PHONY=setup-db-dirs
setup-db-dirs:  ## Create local data directories for app state.
	mkdir -p "$(CURDIR)/data"
	mkdir -p "$(CURDIR)/data/users"
	mkdir -p "$(CURDIR)/data/tokens"
	mkdir -p "$(CURDIR)/data/http_logs"
	mkdir -p "$(CURDIR)/data/app_logs"

.PHONY=setup-lint
setup-lint:  ## Install buildifier, clang-format, and Black.
	sudo apt-get update -y
	# Install Buildifier
	sudo apt-get install golang-go -y
	go install github.com/bazelbuild/buildtools/buildifier@latest
	# Install clang-format
	sudo apt install clang-format -y
	clang-format --version
	# Python linter
	sudo apt-get install python3-full python3-pip -y
	python3 -m pip install black

.PHONY=setup-ngrok
setup-ngrok:  ## Install ngrok apt repo and package.
	curl -sSL https://ngrok-agent.s3.amazonaws.com/ngrok.asc \
	  | sudo tee /etc/apt/trusted.gpg.d/ngrok.asc >/dev/null \
	  && echo "deb https://ngrok-agent.s3.amazonaws.com buster main" \
	  | sudo tee /etc/apt/sources.list.d/ngrok.list \
	  && sudo apt update \
	  && sudo apt install ngrok

.PHONY=single-file-with-docker
single-file-with-docker:
	@set -euo pipefail; \
	SF_URL="$(SF_URL)"; \
	[ -n "$$SF_URL" ] || { echo "Usage: SF_URL=https://... make single-file-with-docker"; exit 2; }; \
	mkdir -p "$(SF_OUT_DIR)"; \
	slug="$$(printf '%s' "$$SF_URL" | awk ' \
	  BEGIN{FS="://"} \
	  {sub(/^[A-Za-z]+:\/\//,"")}     # drop scheme \
	  {gsub(/\/+$$/,"")}              # strip trailing / \
	  {gsub(/[^A-Za-z0-9._-]+/,"-")}  # replace unsafe with - \
	  {gsub(/-+/,"-")}                # collapse --- \
	  {sub(/^-+/,""); sub(/-+$$/,"")} # trim leading/trailing - \
	  {print substr($$0,1,180)}       # cap length \
	')"; \
	out="$(SF_OUT_DIR)/$(TIMESTAMP)__$$slug.html"; \
	if ! docker image inspect $(SINGLEFILE_IMAGE) >/dev/null 2>&1; then \
	  docker pull capsulecode/singlefile; \
	  docker tag capsulecode/singlefile $(SINGLEFILE_IMAGE); \
	fi; \
	docker run --rm \
	  -v "$$PWD/$(SF_OUT_DIR)":/usr/src/app/out \
	  $(SINGLEFILE_IMAGE) "$$SF_URL" "$$(basename "$$out")"; \
	echo "Saved: $$out"

.PHONY=singlefile
singlefile: ## singlefile rule (auto)
	@set -euo pipefail; \
	[ -n "$(SF_URL)" ] || { echo "Usage: SF_URL=https://... make singlefile [DEPTH=n]"; exit 2; }; \
	$(BAZEL) run //cs/devtools:singlefile -- --url="$(SF_URL)" $(if $(DEPTH),--depth="$(DEPTH)",) --output-dir="$(shell pwd)/out";

.PHONY=stash
stash:  ## Commit all changes to branch $(BRANCH), push, then switch back to main.
	git checkout -b $(BRANCH) && git add . && git commit -m "$(BRANCH)" && git push --set-upstream origin $(BRANCH) && git checkout main

.PHONY=stress
stress:  ## Loop: clean, build, and test repeatedly until stopped.
	until false; do ${BAZEL} clean && make b t; done;

.PHONY=t
t: test  ## Alias for `test`.

.PHONY=test
test:  ## Runs all tests with full output.
	@${BAZEL} test --test_output=all -- //cs/...

.PHONY=up
up: build-up ## Starts a Compose project.
	$(DC) up

.PHONY=update-env
update-env: ## update-env rule (auto)
	$(BAZEL) run //cs/devtools:update_env -- --env "$(CURDIR)/.env"

.PHONY=use-usings
use-usings:  ## Normalize/insert C++ `using` declarations.
	@$(BAZEL) run //cs/devtools:use_usings -- --repo_root "$(shell pwd)/cs" $(if $(ALIAS_STL),--include_std_vector)

.PHONY=www
www: ## Serve live logs UI and restart `make up` when upstream changes (Bazel Python).
	@echo "[www] repo=$(REPO) upstream=$(UPSTREAM) poll=$(POLL) url=http://$(WWW_HOST):$(WWW_PORT)/"
	@${BAZEL} run //cs/devtools:www -- \
	  --repo "$(REPO)" \
	  --poll "$(POLL)" \
	  $(if $(UPSTREAM),--upstream "$(UPSTREAM)",) \
	  --host "$(WWW_HOST)" \
	  --port "$(WWW_PORT)" \
	  --log-file "$(shell pwd)/data/www.log"

