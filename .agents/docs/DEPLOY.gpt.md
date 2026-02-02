# Deployment report: what happens when you run `make`

This document describes every step that runs when you
execute `make` with no arguments. The default goal is `up`.

---

## 1. Default goal and dependency tree

From the Makefile:

```makefile
.DEFAULT_GOAL := up
```

So `make` runs the target **`up`**.

**Dependencies:**

- `up` depends on: **`images-build`**, **`setup-volumes`**
- `images-build` depends on: **`expand`**, **`base-image`**
- `expand` has no Makefile prerequisites (it runs Bazel and
  writes files)
- `base-image` has no Makefile prerequisites
- `setup-volumes` has no Makefile prerequisites

**Execution order:** Make runs `images-build` and
`setup-volumes` (order between them is unspecified). To
build `images-build`, it first runs `expand` and
`base-image` (order between them unspecified), then the
`images-build` recipe. Finally it runs the `up` recipe.

**Important variables** (from Makefile, before any `.env`
override):

| Variable                   | Default                                             | Used by                       |
| -------------------------- | --------------------------------------------------- | ----------------------------- |
| `MAKEFLAGS`                | `--always-make --no-keep-going`                     | make itself                   |
| `DOCKER_BUILDKIT`          | `1`                                                 | images-build, base-image      |
| `COMPOSE_DOCKER_CLI_BUILD` | `1`                                                 | images-build                  |
| `COMPOSE_PROJECT_NAME`     | `cs-dev`                                            | images-build (compose build)  |
| `COMPOSE_UP_FLAGS`         | `--wait --remove-orphans`                           | up                            |
| `BAZEL`                    | `USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk` | expand, base-image (indirect) |

Note: the **`up`** recipe hardcodes
**`COMPOSE_PROJECT_NAME=cs`** for all `docker compose`
commands it runs, so the stack is brought up under project
name **`cs`**, not `cs-dev`.

---

## 2. Step 1: `expand`

**Target:** `expand`  
**Purpose:** Generate `docker-compose.yml` and `Dockerfile`
from `system.gpt.yml`, then build load-balancer routing and
copy `routing.json`.

**Recipe (from Makefile):**

```makefile
.PHONY: expand
expand: ## Generate docker-compose.yml and Dockerfile from system.gpt.yml.
	@${BAZEL} run //cs/devtools:expand_system -- $(CURDIR)/system.gpt.yml
	@${BAZEL} run //cs/devtools:test_expand_system -- $(CURDIR)
	@${BAZEL} build //cs/apps/load-balancer:routing
	@cp bazel-bin/cs/apps/load-balancer/routing.json $(CURDIR)/routing.json 2>/dev/null || true
```

### 2.1 Run expand_system

**Command:**

```bash
USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk run //cs/devtools:expand_system -- /workspaces/cs/system.gpt.yml
```

**Effect:** The Python tool
`cs/devtools/expand_system.gpt.py` reads `system.gpt.yml`,
computes content hashes for each service’s source tree, and
writes:

- **`docker-compose.yml`** – Compose service definitions
  (build context, image names, labels, deploy replicas,
  command, env, volumes, etc.).
- **`Dockerfile`** – Multi-stage Dockerfile with a shared
  `base` stage, `runtime-base` / `runtime` stages, one build
  stage per app (e.g. `build_www_trycopilot_ai`), one
  runtime stage per app (e.g. `www_trycopilot_ai`), plus
  `cs-public` and `ngrok` stages.

**Example generated header (docker-compose.yml):**

```yaml
# WARNING: This file is auto-generated from system.gpt.yml
# Do not edit manually! Regenerate with: make expand
# Environment: prod

services:
  www-trycopilot-ai:
    build:
      context: .
      dockerfile: Dockerfile
      target: www_trycopilot_ai
    image: cs-app:www-trycopilot-ai-d105a86f
    pull_policy: never
    ...
```

**Example generated Dockerfile (fragment):**

```dockerfile
# WARNING: This file is auto-generated from system.gpt.yml
# Do not edit manually! Regenerate with: make expand
#
# Multi-stage Dockerfile - builds all services in parallel
# Each service gets its own build stage and minimal runtime stage

# Base build image with all source code and build tools (from context)
FROM docker.io/library/ubuntu:24.04@sha256:... AS base
RUN set -eux; \
    apt-get update; \
    apt-get install --yes --no-install-recommends \
    build-essential ca-certificates clang-format nodejs npm \
    && rm -rf /var/lib/apt/lists/*;
...
FROM base AS build_www_trycopilot_ai
RUN --mount=type=cache,target=/home/myappuser/.cache/bazel,... \
    USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk \
    --output_base=/tmp/bazel-output-www_trycopilot_ai \
    build --config=release //cs/apps/trycopilot.ai:main
...
FROM runtime AS www_trycopilot_ai
COPY --from=build_www_trycopilot_ai ... /main
CMD ["/main", "--host=0.0.0.0", "--port=8080", ...]
```

### 2.2 Run test_expand_system

**Command:**

```bash
USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk run //cs/devtools:test_expand_system -- $(CURDIR)
```

**Effect:** Runs the expand integration test (e.g. validates
generated Compose config and expected services).

### 2.3 Build load-balancer routing

**Command:**

```bash
USE_BAZEL_VERSION=8.4.2 npx --yes @bazel/bazelisk build //cs/apps/load-balancer:routing
```

**Effect:** Bazel builds the `routing` target, which
generates `routing.json` from `system.gpt.yml` (domain →
service routing for the load balancer). Output lives under
`bazel-bin/cs/apps/load-balancer/routing.json`.

### 2.4 Copy routing.json to repo root

**Command:**

```bash
cp bazel-bin/cs/apps/load-balancer/routing.json $(CURDIR)/routing.json 2>/dev/null || true
```

**Effect:** Copies the generated `routing.json` into the
workspace root so the load-balancer image (built later) can
use it at build/runtime. Failures are ignored (`|| true`).

---

## 3. Step 2: `base-image`

**Target:** `base-image`  
**Purpose:** Build the base Docker images used by the
multi-stage Dockerfile (`base` and `runtime-base`).

**Recipe (from Makefile):**

```makefile
.PHONY: base-image
base-image:  ## Build cs-app-base:latest and runtime-base:latest images (required for app builds, no pull).
	@echo "[base-image] Building base image (using local/cached images only, no pull)..."
	@DOCKER_BUILDKIT=$(DOCKER_BUILDKIT) docker build --pull=false -t cs-app-base:latest -f Dockerfile --target base .
	@echo "[base-image] Building runtime-base image (using local/cached images only, no pull)..."
	@DOCKER_BUILDKIT=$(DOCKER_BUILDKIT) docker build --pull=false -t runtime-base:latest -f Dockerfile --target runtime-base .
```

**Commands:**

```bash
DOCKER_BUILDKIT=1 docker build --pull=false -t cs-app-base:latest -f Dockerfile --target base .
DOCKER_BUILDKIT=1 docker build --pull=false -t runtime-base:latest -f Dockerfile --target runtime-base .
```

**Effect:** Builds the `base` and `runtime-base` stages from
the **generated** `Dockerfile` and tags them as
`cs-app-base:latest` and `runtime-base:latest`. These are
the same stages the rest of the Dockerfile uses as
`FROM base` and `FROM runtime-base`; the tagged images can
be used by other workflows. `images-build` runs
`docker compose build --no-cache`, so it will rebuild those
stages again; this step still runs because it is a
dependency of `images-build`.

---

## 4. Step 3: `images-build`

**Target:** `images-build`  
**Prerequisites:** `expand`, `base-image` (already
completed).

**Recipe (from Makefile):**

```makefile
.PHONY: images-build
images-build: expand base-image  ## Build Docker images using Dockerfiles (fast layer caching, no pull).
	@echo "[images-build] Building Docker images with BuildKit (no pull - local images only)..."
	@DOCKER_BUILDKIT=$(DOCKER_BUILDKIT) COMPOSE_DOCKER_CLI_BUILD=$(COMPOSE_DOCKER_CLI_BUILD) \
	  COMPOSE_PROJECT_NAME=$(COMPOSE_PROJECT_NAME) \
	  docker compose build --no-cache --force-rm
```

**Command (with defaults):**

```bash
DOCKER_BUILDKIT=1 COMPOSE_DOCKER_CLI_BUILD=1 COMPOSE_PROJECT_NAME=cs-dev \
  docker compose build --no-cache --force-rm
```

**Effect:** Uses the **generated** `docker-compose.yml` and
`Dockerfile`. Builds all service images referenced in the
Compose file (www-trycopilot-ai, www-cite-pub, code-viewer,
database-service, load-balancer, service-registry). Does
**not** pull base images (`pull_policy: never` in the
generated compose). Each service image is built from its
corresponding stage in the Dockerfile (e.g.
`www_trycopilot_ai`, `load_balancer`, `service_registry`).
The `ngrok` service uses a pre-built image and is not built
here. Resulting image names are like
`cs-app:www-trycopilot-ai-d105a86f`,
`cs-app:load-balancer-05de635b`,
`cs-app:service-registry-98636ff0`, etc.

---

## 5. Step 4: `setup-volumes`

**Target:** `setup-volumes`  
**Purpose:** Ensure required Docker volumes exist so Compose
services can mount them.

**Recipe (from Makefile):**

```makefile
.PHONY: setup-volumes
setup-volumes:  ## Create Docker volumes if they don't exist.
	@set -euo pipefail; \
	. cs/devtools/docker.sh; \
	create_volumes_if_not_exist database-volume docuseal_data gitea_data overleaf_redis_data overleaf_mongo_data penpot_assets_data penpot_postgres_data penpot_redis_data
```

**Effect:** Sources `cs/devtools/docker.sh`, then calls
`create_volumes_if_not_exist` with the listed volume names.
That function (from `docker.sh`) does:

```bash
function create_volumes_if_not_exist() {
    echo "Creating volumes if they do not exist:"
    for VOLUME_NAME in ${@:1}; do
        volume_exists="$(does_volume_exist $VOLUME_NAME)"
        if [ "$volume_exists" -eq "0" ]; then
            echo "--> Creating non-existent volume $VOLUME_NAME."
            docker volume create $VOLUME_NAME
        else
            echo "--> Volume $VOLUME_NAME already exists. Skipping creation."
        fi
    done
    echo "Done."
}
```

**Volumes created if missing:** `database-volume`,
`docuseal_data`, `gitea_data`, `overleaf_redis_data`,
`overleaf_mongo_data`, `penpot_assets_data`,
`penpot_postgres_data`, `penpot_redis_data`. Of these,
`docker-compose.yml` currently declares only
`database-volume` as `external: true`; the others are for
other stacks or future use.

---

## 6. Step 5: `up` (main recipe)

**Target:** `up`  
**Prerequisites:** `images-build`, `setup-volumes` (already
completed).

**Recipe (from Makefile, summarized):**

```makefile
.PHONY: up
up: images-build setup-volumes ## Bring up the production stack (docker-compose.yml).
	@set -euo pipefail; \
	echo "[up] freeport disabled; leaving ports 3001/3002 unchanged"; \
	echo "[up] Ensuring Docker is running..."; \
	if ! docker info >/dev/null 2>&1; then \
	  echo "ERROR: Docker daemon is not running. ..."; exit 1; fi; \
	echo "[up] Starting all services..."; \
	if ! COMPOSE_PROJECT_NAME=cs docker compose up -d $(COMPOSE_UP_FLAGS) 2>&1; then \
	  echo "ERROR: docker compose up failed"; ... exit 1; fi; \
	echo "[up] Force-recreating service-registry so it uses the newly built image"; \
	COMPOSE_PROJECT_NAME=cs docker compose up -d --force-recreate service-registry 2>&1 || true; \
	echo "[up] Waiting for services to initialize..."; \
	sleep 3; \
	# ... status check with jq, then ...
	echo "[up] Stack deployed. Status:"; \
	COMPOSE_PROJECT_NAME=cs docker compose ps 2>&1 || true; \
	echo ""; \
	echo "Tail logs: make logs [TAIL=200]"
```

### 6.1 Check Docker daemon

**Command:**

```bash
docker info >/dev/null 2>&1
```

**Effect:** If this fails, the script prints an error and
exits with 1. Otherwise it continues.

### 6.2 Start all services

**Command:**

```bash
COMPOSE_PROJECT_NAME=cs docker compose up -d --wait --remove-orphans
```

**Effect:** Starts all services defined in the generated
`docker-compose.yml` under project name **`cs`** (containers
will be named with prefix `cs_`). Uses the images built in
`images-build`. `--wait` waits for services to be
healthy/ready where applicable; `--remove-orphans` removes
containers that are no longer in the Compose file. Existing
containers are **not** recreated (no `--force-recreate`), so
they keep running the image they were created with unless
recreated in a later step.

### 6.3 Force-recreate service-registry

**Command:**

```bash
COMPOSE_PROJECT_NAME=cs docker compose up -d --force-recreate service-registry 2>&1 || true
```

**Effect:** Recreates only the **service-registry**
container so it uses the image that was just built in
`images-build`. Other services are left as-is. The `|| true`
prevents the overall `up` from failing if this step fails.

### 6.4 Wait and report status

**Commands:**

```bash
sleep 3
COMPOSE_PROJECT_NAME=cs docker compose ps --format json   # used for running/total count
COMPOSE_PROJECT_NAME=cs docker compose ps                 # final status table
```

**Effect:** Waits 3 seconds, then prints how many services
are running and a final `docker compose ps` table. It also
suggests `make logs [TAIL=200]` for viewing logs.

---

## 7. Summary: order of execution

| Order | Step            | What runs                                                                                                                                            |
| ----- | --------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| 1     | `expand`        | Bazel run expand_system → write `docker-compose.yml`, `Dockerfile`; Bazel run test_expand_system; Bazel build load-balancer:routing; cp routing.json |
| 2     | `base-image`    | docker build --target base → cs-app-base:latest; docker build --target runtime-base → runtime-base:latest                                            |
| 3     | `images-build`  | docker compose build --no-cache --force-rm (all service images)                                                                                      |
| 4     | `setup-volumes` | source docker.sh; create_volumes_if_not_exist for 8 volumes                                                                                          |
| 5     | `up`            | docker info check; docker compose up -d --wait --remove-orphans; docker compose up -d --force-recreate service-registry; sleep 3; docker compose ps  |

Steps 1 and 2 can run in parallel; step 3 runs after both.
Steps 3 and 4 can run in parallel; step 5 runs after both.

---

## 8. Generated and key files

| File                 | Generated by                                | Purpose                                      |
| -------------------- | ------------------------------------------- | -------------------------------------------- |
| `docker-compose.yml` | `expand` (expand_system)                    | Compose service definitions for prod stack   |
| `Dockerfile`         | `expand` (expand_system)                    | Multi-stage build for all app images + ngrok |
| `routing.json`       | `expand` (Bazel load-balancer:routing + cp) | Domain→service routing for load balancer     |

Optional: if `.env` exists, the Makefile includes it and
exports variables matching `VAR=value` lines, which can
override defaults (e.g. `COMPOSE_PROJECT_NAME`) for targets
that use them. The `up` recipe does **not** use
`COMPOSE_PROJECT_NAME` from the Makefile; it always uses
`cs`.
