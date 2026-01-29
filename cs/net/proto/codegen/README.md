# Code generation for C++/JSON bindings

## Validation attributes

- Annotate fields in `.proto.hh` with `[[...]]` (fields
  only). Prefix `validate::` is optional (e.g.,
  `[[required]]` or `[[validate::required]]`).
  - Examples: `[[required]] std::string email;`
  - `[[gt(8)]] int age;`
  - `[[email, len_lt(255)]] std::string email;`
- Supported atoms: `required`, `email`, `gt/ge/lt/le`,
  `len_gt/len_lt`, `enum_in(...)`, `matches("regex")`,
  `oneof_set`, `custom("token")`.
- Codegen emits `<proto>.validate.hh` with
  `using <Proto>Rules = all<...>;` and path strings from
  field names; validation collects all errors.
- Include the generated `*.validate.hh` and call
  `cs::net::proto::validation::Validate(msg, <Proto>Rules{});`.

## Testing & goldens

- Tests live in `cs/net/proto/codegen/tests` and rely on
  deterministic set ordering. Run them with:
  `PYTHONHASHSEED=0 python -m unittest discover cs/net/proto/codegen/tests`.
- Golden outputs for fixtures are checked in under
  `cs/net/proto/codegen/testdata/golden`. Refresh after
  intentional generator changes with:
  `PYTHONHASHSEED=0 python cs/net/proto/codegen/tests/refresh_goldens.py`.
