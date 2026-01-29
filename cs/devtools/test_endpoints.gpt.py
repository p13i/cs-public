#!/usr/bin/env python3
# cs/devtools/test_endpoints.gpt.py
# cs/devtools/test_endpoints.py
"""Test HTTP connectivity and health for all public subdomains."""
import argparse
import json
import sys
import time
import urllib.error
import urllib.request
from typing import Dict, List, Optional


# Endpoints to test (from ngrok.yml)
ENDPOINTS = [
    "https://www.cite.pub",
    "https://www.trycopilot.ai",
]

DEFAULT_TIMEOUT = 10  # seconds


class EndpointResult:
    """Result of testing a single endpoint."""

    def __init__(
        self,
        endpoint: str,
        status_code: Optional[int] = None,
        response_time_ms: Optional[float] = None,
        success: bool = False,
        error: Optional[str] = None,
    ):
        self.endpoint = endpoint
        self.status_code = status_code
        self.response_time_ms = response_time_ms
        self.success = success
        self.error = error

    def to_dict(self) -> Dict:
        """Convert to dictionary for JSON output."""
        result = {
            "endpoint": self.endpoint,
            "success": self.success,
        }
        if self.status_code is not None:
            result["status_code"] = self.status_code
        if self.response_time_ms is not None:
            result["response_time_ms"] = round(self.response_time_ms, 2)
        if self.error:
            result["error"] = self.error
        return result


def test_endpoint(url: str, timeout: int = DEFAULT_TIMEOUT) -> EndpointResult:
    """Test a single endpoint and return result."""
    start_time = time.time()
    try:
        req = urllib.request.Request(url)
        req.add_header("User-Agent", "cs-endpoint-tester/1.0")
        with urllib.request.urlopen(req, timeout=timeout) as response:
            status_code = response.getcode()
            response_time_ms = (time.time() - start_time) * 1000
            # Consider 2xx and 3xx as success
            success = 200 <= status_code < 400
            return EndpointResult(
                endpoint=url,
                status_code=status_code,
                response_time_ms=response_time_ms,
                success=success,
            )
    except urllib.error.HTTPError as e:
        response_time_ms = (time.time() - start_time) * 1000
        # Some HTTP errors might still indicate connectivity
        success = False
        return EndpointResult(
            endpoint=url,
            status_code=e.code,
            response_time_ms=response_time_ms,
            success=success,
            error=f"HTTP {e.code}: {e.reason}",
        )
    except urllib.error.URLError as e:
        response_time_ms = (time.time() - start_time) * 1000
        return EndpointResult(
            endpoint=url,
            response_time_ms=response_time_ms,
            success=False,
            error=str(e.reason) if e.reason else str(e),
        )
    except Exception as e:
        response_time_ms = (time.time() - start_time) * 1000
        return EndpointResult(
            endpoint=url,
            response_time_ms=response_time_ms,
            success=False,
            error=str(e),
        )


def print_console_output(results: List[EndpointResult], verbose: bool = False):
    """Print human-readable console output."""
    print("=" * 80)
    print("Endpoint Health Check Results")
    print("=" * 80)
    print()

    # Table header
    print(f"{'Endpoint':<35} {'Status':<8} {'Time (ms)':<12} {'Result'}")
    print("-" * 80)

    all_passed = True
    for result in results:
        status_str = (
            str(result.status_code) if result.status_code is not None else "N/A"
        )
        time_str = (
            f"{result.response_time_ms:.2f}"
            if result.response_time_ms is not None
            else "N/A"
        )
        result_str = "✓ PASS" if result.success else "✗ FAIL"

        # Color indicators (simple text-based)
        if result.success:
            result_indicator = result_str
        else:
            result_indicator = result_str
            all_passed = False

        print(
            f"{result.endpoint:<35} {status_str:<8} {time_str:<12} {result_indicator}"
        )

        if verbose and result.error:
            print(f"  Error: {result.error}")

    print("-" * 80)
    print()

    # Summary
    passed = sum(1 for r in results if r.success)
    total = len(results)
    print(f"Summary: {passed}/{total} endpoints passed")

    if all_passed:
        print("All endpoints are healthy!")
    else:
        print("Some endpoints failed health checks.")
        failed = [r.endpoint for r in results if not r.success]
        if failed:
            print(f"Failed endpoints: {', '.join(failed)}")

    return all_passed


def print_json_output(results: List[EndpointResult]):
    """Print JSON output to stdout."""
    output = {
        "results": [r.to_dict() for r in results],
        "summary": {
            "total": len(results),
            "passed": sum(1 for r in results if r.success),
            "failed": sum(1 for r in results if not r.success),
        },
    }
    print(json.dumps(output, indent=2))


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Test HTTP connectivity for public subdomains"
    )
    parser.add_argument(
        "--json",
        action="store_true",
        help="Output results as JSON only",
    )
    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="Show detailed output including errors",
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=DEFAULT_TIMEOUT,
        help=f"Request timeout in seconds (default: {DEFAULT_TIMEOUT})",
    )
    args = parser.parse_args()

    # Test all endpoints
    results = []
    for endpoint in ENDPOINTS:
        if not args.json:
            print(f"Testing {endpoint}...", end=" ", flush=True)
        result = test_endpoint(endpoint, timeout=args.timeout)
        results.append(result)
        if not args.json:
            status = "✓" if result.success else "✗"
            print(f"{status}")

    # Output results
    if args.json:
        print_json_output(results)
    else:
        print_console_output(results, verbose=args.verbose)

    # Exit with appropriate code
    all_passed = all(r.success for r in results)
    sys.exit(0 if all_passed else 1)


if __name__ == "__main__":
    main()
