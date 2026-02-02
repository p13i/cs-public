---
name: Internal Database Service Implementation
overview:
  Implement a network-internal database service that
  isolates cs-data volume access, exposes Get/Set/Query RPCs
  via HTTP, and migrates existing services to use the new
  service instead of direct filesystem access. Only the
  database service will mount cs-data; all other services
  will query via HTTP API.
todos:
  - id: proto-models
    content:
      Create database.proto.hh with DatabaseRecord,
      GetDatabaseRequest/Response,
      SetDatabaseRequest/Response,
      QueryDatabaseRequest/Response, and supporting types
      (Value, Condition, Predicate, OrderBy, Projection)
    status: completed
  - id: database-service-binary
    content:
      Create database-service/main.gpt.cc with WebApp setup,
      argv parsing, and RPC route registration
    status: completed
  - id: get-rpc
    content:
      Implement GetDatabaseRPC handler with collection
      allowlist, uuid lookup, and collection scanning with
      filters
    status: pending
  - id: set-rpc
    content:
      Implement SetDatabaseRPC handler with validation, uuid
      generation, timestamp management, and write_mode
      support
    status: pending
  - id: query-rpc
    content:
      Implement QueryDatabaseRPC handler with predicate
      translation, JSON path traversal, and projection
      support
    status: pending
  - id: docker-compose
    content:
      Add database service to docker-compose.yml, remove
      cs-data mount from all other services, update BUILD
      file with database-service target
    status: pending
  - id: client-library
    content:
      Create DatabaseServiceClient in
      cs/net/proto/database/client.gpt.{hh,cc} with
      Get/Set/Query methods that provide
      JsonProtoDatabase-like API
    status: pending
  - id: query-builder
    content:
      Create DatabaseQueryBuilder fluent API in
      cs/net/proto/database/query_builder.gpt.{hh,cc}
    status: pending
  - id: service-registry
    content:
      Register database service in service-registry on
      startup
    status: pending
  - id: migrate-auth
    content:
      Migrate cs/apps/trycopilot.ai/api/auth.cc to use
      DatabaseServiceClient instead of direct
      JsonProtoDatabase
    status: pending
  - id: migrate-user
    content:
      Migrate cs/apps/trycopilot.ai/api/user.cc to use
      DatabaseServiceClient
    status: pending
  - id: migrate-logs
    content:
      Migrate cs/apps/trycopilot.ai/api/logs.cc to use
      DatabaseServiceClient
    status: pending
  - id: migrate-ui
    content:
      Migrate cs/apps/trycopilot.ai/ui/ui.cc to use
      DatabaseServiceClient
    status: pending
  - id: remove-db-routes
    content: Remove GET/POST /db/* routes from main.cc
    status: pending
  - id: update-build-files
    content:
      Update BUILD files for database-service binary,
      database proto, client library, and query builder
    status: pending
  - id: demo-app
    content:
      Create cs/apps/database-demo demonstration app showing
      Get, Set, and Query usage patterns with
      DatabaseServiceClient and DatabaseQueryBuilder
    status: pending
  - id: test-coverage
    content:
      Write 100% coverage C++ tests for all database service
      components (RPC handlers, client library, query
      builder, proto models)
    status: pending
---

# Internal Database Service Implementation

## Overview

Create a dedicated `database` service that handles all
database I/O via `JsonProtoDatabase`, isolating the
`cs-data` volume to a single container. **Only the database
service will have `cs-data` mounted**; all other services
will query the database service via HTTP RPCs using an API
similar to `JsonProtoDatabase` but over the network.

## Architecture

The new service will:

- Run as a separate binary in the trycopilot.ai app suite
- Listen on port 8080 (internal network only, no host
  exposure)
- **Mount `cs-data` exclusively at `/data`** (only this
  service has filesystem access)
- Expose three RPC endpoints: `GetDatabase`, `SetDatabase`,
  and `QueryDatabase`
- Enforce collection allowlist for security
- Use `JsonProtoDatabase<DatabaseRecord>` for storage

**Client API Design**: Other services will use
`DatabaseServiceClient` which provides a
`JsonProtoDatabase`-like interface but makes HTTP calls to
the database service. This allows existing code patterns to
be preserved while moving to a service-based architecture.

## Implementation Tasks

### 1. Define Proto Models

**File**: `cs/apps/trycopilot.ai/protos/database.proto.hh`

Create proto definitions using `DECLARE_PROTO`:

- `DatabaseRecord`: Core storage model with `uuid`,
  `collection`, `payload_json` (string), `metadata` (map),
  `created_at`, `updated_at`, `schema_version`
- `GetDatabaseRequest`: Collection, optional uuid, metadata
  filters, limit, order
- `GetDatabaseResponse`: Array of `DatabaseRecord`
- `SetDatabaseRequest`: Collection, payload_json, optional
  uuid, metadata, schema_version, write_mode enum
- `SetDatabaseResponse`: Single `DatabaseRecord`
- `Value`: Union type for query conditions
  (string/int64/double/bool/json)
- `Condition`: Field path, operator enum, value
- `Predicate`: AND/OR logic with condition arrays
- `OrderBy`: Field path and direction
- `Projection`: Optional field inclusion list
- `QueryDatabaseRequest`: Collection, predicate, order_by,
  limit, projection
- `QueryDatabaseResponse`: Array of `DatabaseRecord`

### 2. Create Database Service Binary

**File**:
`cs/apps/trycopilot.ai/database-service/main.gpt.cc`

- Accept `DATA_DIR` and `PORT` as argv (default `/data` and
  `8080`)
- Write `DATA_DIR` to `Context` for handler access
- Create `WebApp` instance
- Register three RPC handlers:
  - `GET /rpc/database/get` → `GetDatabaseRPC`
  - `POST /rpc/database/set` → `SetDatabaseRPC`
  - `POST /rpc/database/query` → `QueryDatabaseRPC`
- Run server on `0.0.0.0:PORT`
- Use `Result::Main` pattern like other services

### 3. Implement Get RPC Handler

**File**:
`cs/apps/trycopilot.ai/database-service/rpc.gpt.cc`

**GetDatabaseRPC**:

- Validate `collection` is in allowlist: `users`, `tokens`,
  `http_logs`, `app_logs`, `queries`
- If `uuid` provided: fetch single record via
  `JsonProtoDatabase<DatabaseRecord>::query_view().where(...).first()`
- Otherwise: scan collection, filter by metadata map (string
  equality), apply order + limit
- Return `200` with records array, `400` for invalid params,
  `404` if uuid not found, `500` for IO errors

### 4. Implement Set RPC Handler

**SetDatabaseRPC**:

- Validate collection in allowlist
- Validate `payload_json` is a JSON object
- Generate `uuid` if absent using `generate_uuid_v4()`
- Check existing record if `write_mode == IF_ABSENT`, return
  `409` if exists
- Stamp `created_at` (preserve on rewrite) and `updated_at`
  (always refresh) as ISO8601 UTC
- Persist via `JsonProtoDatabase<DatabaseRecord>::Insert()`
- Return `201` with stored record, `400` for validation,
  `409` for conflicts, `500` for IO errors

### 5. Implement Query RPC Handler

**QueryDatabaseRPC**:

- Validate collection in allowlist
- Parse `QueryDatabaseRequest` proto from request body
- Translate `Predicate` into `QueryView` filters:
  - `metadata.<key>` → filter on `DatabaseRecord.metadata`
    map
  - `payload_json.<path>` → parse JSON and traverse path
- Apply supported operators: `eq`, `neq`, `gt`, `gte`, `lt`,
  `lte`, `contains`, `starts_with`
- Reject unsupported comparators or unknown fields
- Apply `order_by` using `QueryView::order_by()`
- Apply `limit` (default 100 if zero)
- Apply `projection` if provided (filter returned fields)
- Return `QueryDatabaseResponse` with matching records

### 6. Add Service to Docker Compose

**File**: `cs/apps/trycopilot.ai/BUILD`

- Add `cc_binary` target `database-service` with
  `main.gpt.cc` and dependencies
- Update `compose_service` to include `database` service:
  - Image: `cs-app:web`
  - Command: `["/database-service", "/data", "8080"]`
  - Volumes: `cs-data:/data` (**ONLY this service mounts
    it**)
  - No published ports, expose 8080 internally
  - `restart: unless-stopped`

**File**: `cs/apps/trycopilot.ai/docker-compose.yml`

- **Remove `cs-data` volume from ALL other services**
  (www-trycopilot-ai, and any others)
- Add new `database` service entry with above configuration
- Verify no other services have `cs-data` mount

### 7. Create Client Helper Library

**File**: `cs/net/proto/database/client.gpt.hh` and
`cs/net/proto/database/client.gpt.cc`

Create `DatabaseServiceClient` class that provides a
`JsonProtoDatabase`-like API:

- Constructor takes base URL (e.g., `http://database:8080`)
  or uses service registry
- Methods that mirror `JsonProtoDatabase` interface:
  - `query_view()` → returns
    `QueryView<DatabaseRecord>`-like object that makes HTTP
    calls
  - `Insert(DatabaseRecord)` → HTTP POST to
    `/rpc/database/set`
  - `Get(collection, uuid?)` → returns `DatabaseRecord` or
    vector
  - `Set(collection, payload_json, metadata, ...)` → returns
    `DatabaseRecord`
  - `Query(QueryDatabaseRequest)` → returns vector of
    `DatabaseRecord`
- Uses `RPCClient` internally for HTTP calls
- Handles error responses and converts to `Result` types
- **Key design**: Provides drop-in replacement API so
  existing `JsonProtoDatabase` code can be migrated with
  minimal changes

### 8. Create Query Builder

**File**: `cs/net/proto/database/query_builder.gpt.hh` and
`cs/net/proto/database/query_builder.gpt.cc`

Create `DatabaseQueryBuilder` fluent API:

- `collection(name)` → sets collection
- `where_eq(field, value)`, `where_gt(field, value)`, etc. →
  adds conditions
- `order_by_updated_desc()`, `order_by_created_desc()` →
  sets ordering
- `limit(n)` → sets limit
- `select(fields...)` → sets projection
- `Build()` → returns `QueryDatabaseRequest` proto
- Validates field paths (must start with `metadata.` or
  `payload_json.`)
- Validates operators are supported

### 9. Register Service in Service Registry

**File**:
`cs/apps/trycopilot.ai/database-service/main.gpt.cc`

On startup, register the `database` service:

- Create `ServiceRegistry` entry with name `database`, host
  `database`, port `8080`
- Use `JsonProtoDatabase<ServiceRegistry>` to persist (but
  call via service-registry RPC if available)
- Or add self-registration logic similar to other services

### 10. Migrate Existing Services

Update services that currently use `JsonProtoDatabase`
directly:

**Files to update**:

- `cs/apps/trycopilot.ai/api/auth.cc`: Replace
  `JsonProtoDatabase<User>` and `JsonProtoDatabase<Token>`
  with `DatabaseServiceClient` calls
- `cs/apps/trycopilot.ai/api/user.cc`: Replace
  `JsonProtoDatabase<User>` with `DatabaseServiceClient`
  calls
- `cs/apps/trycopilot.ai/api/logs.cc`: Replace
  `JsonProtoDatabase<HttpLog>` and
  `JsonProtoDatabase<AppLog>` with `DatabaseServiceClient`
  calls
- `cs/apps/trycopilot.ai/ui/ui.cc`: Replace
  `JsonProtoDatabase<Token>` and `JsonProtoDatabase<User>`
  with `DatabaseServiceClient` calls

**Migration pattern**:

- **Remove `DATA_DIR` from `Context::Read()` calls**
  (services no longer need filesystem access)
- Replace
  `JsonProtoDatabase<T>(DATA_DIR, collection).query_view()...`
  with `DatabaseServiceClient(collection).query_view()...`
  (API is similar)
- Update lambda predicates to use query builder methods or
  keep existing patterns if client supports them
- Handle HTTP errors appropriately (network failures,
  service unavailable, etc.)
- **Verify `cs-data` volume mount is removed from
  docker-compose.yml for all migrated services**

### 11. Update BUILD Files

**File**: `cs/apps/trycopilot.ai/BUILD`

- Add `database-service` binary target
- Add proto target for `database.proto.hh`
- Update `trycopilot_ai_image` to include `database-service`
  binary

**File**: `cs/net/proto/database/BUILD`

- Add `client` library target
- Add `query_builder` library target

**File**: `cs/apps/trycopilot.ai/protos/BUILD`

- Add `database.proto` target

### 12. Remove Direct DB Access from Main

**File**: `cs/apps/trycopilot.ai/main.cc`

- Remove `GET /db/*` and `POST /db/*` routes (lines 369-370)
- These are replaced by the dedicated database service

### 13. Create Demonstration App

**Files**: `cs/apps/database-demo/main.gpt.cc`,
`cs/apps/database-demo/BUILD`,
`cs/apps/database-demo/docker-compose.yml`

Create a new standalone app that demonstrates database
service usage:

**Purpose**: Showcase how to use `DatabaseServiceClient` and
`DatabaseQueryBuilder` for common database operations.

**Features to demonstrate**:

1. **Basic Get operations**:
   - Get single record by uuid
   - Get all records from a collection
   - Get with metadata filters

2. **Set operations**:
   - Insert new record (auto-generate uuid)
   - Update existing record (INSERT_OR_REPLACE)
   - Insert only if absent (IF_ABSENT mode)

3. **Query operations**:
   - Query with metadata filters using query builder
   - Query with payload_json path filters
   - Query with ordering (updated_desc, created_desc)
   - Query with limit
   - Query with projection (select specific fields)

4. **Error handling**:
   - Handle 404 (not found)
   - Handle 400 (validation errors)
   - Handle 409 (conflict on IF_ABSENT)
   - Handle 500 (server errors)

**Implementation**:

- Create `cs/apps/database-demo/` directory
- `main.gpt.cc`: Simple WebApp with demo endpoints:
  - `GET /demo/get/:collection/:uuid?` - demonstrates Get
  - `POST /demo/set` - demonstrates Set with JSON body
  - `POST /demo/query` - demonstrates Query with JSON body
  - `GET /demo/examples` - returns HTML page with example
    requests
- Use `DatabaseServiceClient` initialized with
  `http://database:8080` (from service registry or env var)
- Use `DatabaseQueryBuilder` for constructing queries
- Return JSON responses with operation results
- Include logging to show what operations are being
  performed

**Docker setup**:

- Add to main `docker-compose.yml` or create separate
  compose file
- **No `cs-data` mount** (demonstrates client-only usage -
  no filesystem access)
- Expose port 8081 (or available port) for testing
- Depends on `database` service being running

## Data Flow

```
┌─────────────┐         ┌──────────────┐         ┌─────────────┐
│   www-try   │ HTTP    │   database   │         │   cs-data   │
│  copilot-ai │────────▶│   service    │────────▶│   volume    │
│  (api/ui)   │ RPCs    │   (8080)     │  FS     │   (/data)   │
└─────────────┘         └──────────────┘         └─────────────┘
      │                        │
      │                        │
      └────────────────────────┘
         (internal network)
         (NO cs-data mount)
```

**Key Points**:

- Only the `database` service has `cs-data` mounted
- All other services query via HTTP RPCs
- Services use `DatabaseServiceClient` which provides
  `JsonProtoDatabase`-like API

## Testing Considerations

- Verify database service starts and accepts connections
- Test Get with uuid and without (collection scan)
- Test Set with INSERT_OR_REPLACE and IF_ABSENT modes
- Test Query with various predicates and ordering
- Verify collection allowlist enforcement
- Test error handling (404, 400, 409, 500)
- Verify existing services work after migration
- Test service registry lookup for database service
- Verify no other services have `cs-data` mount after
  migration

## Security Notes

- Collection allowlist prevents arbitrary path traversal
- Metadata keys limited to ASCII, short values
- Field paths validated (must start with `metadata.` or
  `payload_json.`)
- No host port exposure (internal network only)
- **Single service mounts `cs-data` (principle of least
  privilege)** - all other services are stateless clients

## Test Coverage Requirements

**100% C++ test coverage required** for all components:

### Test Files to Create:

1. **`cs/apps/trycopilot.ai/database-service/rpc_test.gpt.cc`**
   - Test `GetDatabaseRPC` with all scenarios (uuid lookup,
     collection scan, filters, errors)
   - Test `SetDatabaseRPC` with all write modes, validation,
     error cases
   - Test `QueryDatabaseRPC` with all predicate types,
     operators, ordering, projection
   - Test collection allowlist enforcement
   - Test error responses (400, 404, 409, 500)
   - Mock `JsonProtoDatabase` for isolated unit tests

2. **`cs/net/proto/database/client_test.gpt.cc`**
   - Test `DatabaseServiceClient` constructor and
     initialization
   - Test all `Get` method variants
   - Test all `Set` method variants
   - Test `Query` method
   - Test `query_view()` method and returned query view
     object
   - Test error handling and HTTP error conversion
   - Mock HTTP client for isolated tests

3. **`cs/net/proto/database/query_builder_test.gpt.cc`**
   - Test all builder methods (`collection`, `where_*`,
     `order_by_*`, `limit`, `select`)
   - Test field path validation
   - Test operator validation
   - Test `Build()` method output
   - Test error cases (invalid paths, unsupported operators)

4. **`cs/apps/trycopilot.ai/protos/database_proto_test.gpt.cc`**
   - Test `DatabaseRecord` serialization/deserialization
   - Test all request/response proto models
   - Test `Value` union type handling
   - Test `Condition` and `Predicate` construction
   - Test `OrderBy` and `Projection` models

5. **Integration Tests**:
   `cs/apps/database-demo/integration_test.gpt.cc`\*\*
   - Test end-to-end flows with real database service
   - Test service registry integration
   - Test error propagation

**Coverage Goals**:

- All code paths executed
- All error branches tested
- All validation logic covered
- All edge cases handled
- Use gtest/gmock for mocking and assertions
