import json
import http.client
import urllib.parse
import sys
import os


def run_requests(host, config_file):
    # Load the JSON configuration
    with open(config_file, "r") as f:
        requests_config = json.load(f)

    for entry in requests_config:
        request_info = entry["request"]
        expected_response = entry["expectedResponse"]

        method = request_info["method"].upper()
        url = host + request_info["url"]
        data = request_info.get("data", "")
        headers = request_info.get("headers", {})

        try:
            print(f"Sending {method} request to {url}...")

            parsed_url = urllib.parse.urlparse(url)
            connection = http.client.HTTPSConnection(parsed_url.netloc)

            path = parsed_url.path
            if parsed_url.query:
                path += f"?{parsed_url.query}"

            body = json.dumps(data).encode("utf-8") if data else None
            if body and "Content-Type" not in headers:
                headers["Content-Type"] = "application/json"

            connection.request(method, path, body=body, headers=headers)

            response = connection.getresponse()
            response_status = response.status

            # Check expected response status
            expected_status = expected_response["status"]
            if response_status == expected_status:
                print(f"Request succeeded with expected status {expected_status}.")
            else:
                print(
                    f"Unexpected status code: {response_status}. Expected: {expected_status}."
                )

            expected_content_type = expected_response.get("contentType", "")
            if expected_content_type:
                real_content_type = response.info()["Content-Type"]
                if expected_content_type == real_content_type:
                    print(f"Request found expected content type!")
                else:
                    print(
                        f"expected_content_type != real_content_type: {expected_content_type} != {real_content_type}"
                    )

            expected_body = expected_response.get("body", "")
            if expected_body:
                if response.info()["Content-Type"] == "application/json":

                    def ordered(obj):
                        if isinstance(obj, dict):
                            return sorted((k, ordered(v)) for k, v in obj.items())
                        if isinstance(obj, list):
                            return sorted(ordered(x) for x in obj)
                        else:
                            return obj

                    response_json = json.loads(response.read().decode("UTF-8"))
                    if ordered(expected_body) != ordered(response_json):
                        print("Unexpected body!")

            connection.close()

        except Exception as e:
            print(f"An error occurred while processing the request: {e}")


if __name__ == "__main__":
    host = os.environ.get("TARGET", "https://cs.p13i.io")
    config_path = "cs/www/app/probes.json"
    run_requests(host, config_path)
