#!/usr/bin/env python3
# cs/devtools/generate_routing.gpt.py
"""
Generate routing.json from system.gpt.yml for load balancer.

Reads the routing section from system.gpt.yml and generates a JSON file
that maps domains to service names.
"""

import json
import sys
import yaml
from pathlib import Path
from typing import Any, Dict


def load_system_spec(path: Path) -> Dict[str, Any]:
    """Load and parse the system specification YAML."""
    with open(path) as f:
        return yaml.safe_load(f)


def generate_routing_json(spec: Dict[str, Any]) -> Dict[str, Any]:
    """Generate routing JSON from system spec with both prod and dev routes."""
    ngrok_service = spec.get("services", {}).get("ngrok", {})
    expose_config = ngrok_service.get("expose", {})
    load_balancer_config = expose_config.get("load-balancer", {})
    mapping = load_balancer_config.get("mapping", {})

    prod_domains = load_balancer_config.get("prod", [])
    prod_routes = {}
    for domain in prod_domains:
        domain_clean = domain.replace("dev.", "", 1)
        service_name = mapping.get(domain_clean) or domain_clean.replace(".", "-")
        prod_routes[domain] = service_name

    dev_domains = load_balancer_config.get("dev", [])
    dev_routes = {}
    for domain in dev_domains:
        domain_clean = domain.replace("dev.", "", 1)
        service_name = mapping.get(domain_clean) or domain_clean.replace(".", "-")
        dev_routes[domain] = service_name

    return {"prod": {"routes": prod_routes}, "dev": {"routes": dev_routes}}


def main():
    if len(sys.argv) < 3:
        print("Usage: generate_routing.gpt.py <system.gpt.yml> <output.json>")
        sys.exit(1)

    system_file = Path(sys.argv[1])
    output_file = Path(sys.argv[2])

    if not system_file.exists():
        print(f"Error: {system_file} not found", file=sys.stderr)
        sys.exit(1)

    spec = load_system_spec(system_file)
    routing_data = generate_routing_json(spec)

    with open(output_file, "w") as f:
        json.dump(routing_data, f, indent=2)

    print(f"Generated {output_file} with prod and dev routes")


if __name__ == "__main__":
    main()
