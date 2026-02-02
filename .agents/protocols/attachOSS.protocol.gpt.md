# attachOSS(protocol) for new OSS subdomains

Goal: given `(subdomain, referenceProduct)` (e.g.,
attachOSS(docs, Google Docs)), pick a self-hostable OSS
alternative, wire it behind `<subdomain>.trycopilot.ai` via
ngrok/Namecheap, and keep `make` working with only minimal
touches to `docker-compose.yml` and `ngrok.yml`.

Steps:

1. Guardrails
   - Read `AGENTS.md` plus any person-specific notes.
   - Keep edits ASCII and tightly scoped (typically
     `docker-compose.yml`, `ngrok.yml`,
     `domains/manifest.json`).
   - Prefer official images and avoid extra host deps unless
     required by the app.

2. Pick the OSS app
   - Choose a maintained, self-hostable alternative to the
     reference product with a stable container image and
     light runtime deps.
   - Confirm listen port, base URL/env needs, and flags to
     disable telemetry/banners; plan to serve at
     `https://<subdomain>.trycopilot.ai`.

3. Domains/tunnels
   - Add `<subdomain>.trycopilot.ai` to
     `domains/manifest.json` (source of truth for ngrok
     reservations and Namecheap DNS). If the file is absent,
     create it using the existing tunnels as a baseline and
     include the new host + reserved domain target.
   - In `ngrok.yml`, add a tunnel named `<subdomain>` that
     points to the container’s port (proto `http` unless the
     app needs `tcp`) and sets
     `domain: <subdomain>.trycopilot.ai`.
   - Update the `ngrok` service in `docker-compose.yml` to
     depend on the new service and include the tunnel name
     in the `ngrok start` command so `make` brings it up.

4. Docker Compose service
   - Add the service under the optional plugins section
     using the official image, `container_name: cs-<name>`,
     `restart: unless-stopped`, and a free host port
     mapping.
   - Set only necessary env (root URL, auth defaults,
     storage paths) and volumes for persistence; avoid
     touching unrelated services or Make targets.

5. Validate
   - Run `docker compose config` for YAML sanity and
     `make lint` (prettier will format YAML).
   - If creds are available, run `make sync-domains` to
     reserve the subdomain and push DNS; otherwise, note the
     follow-up.
   - Bring up the service (`make` or
     `docker compose up -d ngrok <service>`), check
     `docker compose ps`, and hit the container port or
     ngrok dash to confirm reachability.

6. Document
   - Record the chosen OSS app, ports, env defaults, and any
     manual steps (admin creds, sync-domains) in your
     summary so future runs of `make` have no surprises.
