# `cite.pub`

`cite.pub` is a short URL service that provides a permanent
cache of the target site to ensure that referenced material
will always be available. The high availability and strong
data persistence gauruntees of this project will make
`cite.pub` a suitable service for inclusion in academic
papers as permanent URLs. Think doi.org but also for all
digital documents.

## API

- `SaveResource(string publicUrl, uint depth = 1, string proposedAlias) -> ResultOr<string cacheUrl>`
- `ResolveResource(string alias) -> ResultOr<string cacheUrl>`

## UI

The web UI will have a few web endpoints:

- `https://cite.pub`
  - `/` - GET: splash page with creation form and resolving
    form.
  - `/<string alias>` - GET: what users use to load their
    cached resource, `302 Found` redirect to view page
    above. POST: where the form on the front page will POST
    it's content for creating a new alias.

## Constraints

### Short URL alias format

Considering short URL standards, valid characters will be:

```txt
23458abcdefghijknrstxyz
```

- `0` is not allowed because it looks like `O` or `o`..
- `1` is not allowed because it looks like `l` or `L`.
- `6` and `9` are not allowed because they are mirrors.
- `l` is not allowed because it looks like `1`.
- `m` is not allowed because it looks like a dense
  character.
- `o` is not allowed because it looks like `0`.
- `p` and `q` are not allowed because they are mirrors.
- `u` and `v` are not allowed because they look similar.
- `w` is not allowed because it looks like a dense.
- Capital letters and all other characters are not allowed.

Given 23 allowed characters and alias strings of a max
length of 5, without repeated characters, we have:

$$ nCr(23, 5) = \dfrac{23!}{(23-5)!} = 4037880 $$

Or around four million available unique IDs; that's plenty
for now!

### File formats

Typical file formats supported are listed below and
discussed:

- PDF: `*.pdf`
- HTML: `*.html` - presents a unique challenge: CSS,
  JavaScript, images, and hrefs in the source HTML file may
  need to be fetched and inlined, or be rewritten to refer
  to other cache links provided by `cite.pub`.

## Data Models

The data format will be as:

```cc
// Collection: resources/<uuid>.json
struct Resource {
  // Auto-generated.
  string uuid;
  // Auto-generated or user-selected shortId (globally unique).
  // E.g., `fnc38` a random 5 character unique ID.
  string alias;
  string created_iso8601;
  // Reference to the contents/<contents_uuid>.json file
  // containing all the large contents of the file.
  string contents_uuid;
};
```

```cc
// Collection: contents/<uuid>.json
struct Content {
  string uuid;
  // Parent reference.
  string resource_uuid;
  int content_length;
  string content_type;
};
```

By default, all files will be publically-available and
cannot be deleted.

The database used is a UUID-based document database so we
need a structure to allow fast lookups of aliases to UUIDs.
An index "table" can be:

```cc
// Filename: asdf.json
// Collection: lookups/<uuid>.json
struct Lookup {
  string uuid;
  // Takes on `asdf` in this example.
  string alias;
  // Use O(1) lookup of aliases via filesystem checks.
  // The `Resource::uuid` field, i.e. the filename of the actual record.
  string resource_uuid;
};
```
