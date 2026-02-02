// cs/apps/service-registry/protos/services_test.gpt.cc
#include <map>
#include <string>
#include <vector>

#include "cs/apps/service-registry/protos/docker.proto.hh"
#include "cs/apps/service-registry/protos/service.proto.hh"
#include "cs/net/json/object.hh"
#include "cs/net/json/parsers.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::cs::apps::service_registry::protos::
    DockerContainerInfo;
using ::cs::apps::service_registry::protos::
    DockerNetworkInspectElement;
using ::cs::apps::service_registry::protos::ServiceEntry;
using ::cs::apps::service_registry::protos::ServiceRegistry;
using ::cs::net::json::parsers::ParseObject;
using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;
using ::testing::StrEq;

namespace {

// Sample services.json from docker network inspect output
constexpr const char* kServicesJson = R"([
    {
        "Name": "cs-dev_default",
        "Id": "f3391d2a3c3fb048bc01015c3bc2ef3d6fb201876752b40baa0d549e56b956c3",
        "Created": "2026-01-04T04:13:02.888827919Z",
        "Scope": "local",
        "Driver": "bridge",
        "EnableIPv4": true,
        "EnableIPv6": false,
        "IPAM": {
            "Driver": "default",
            "Options": null,
            "Config": [
                {
                    "Subnet": "172.18.0.0/16",
                    "IPRange": "",
                    "Gateway": "172.18.0.1"
                }
            ]
        },
        "Internal": false,
        "Attachable": false,
        "Ingress": false,
        "ConfigFrom": {
            "Network": ""
        },
        "ConfigOnly": false,
        "Options": {
            "com.docker.network.enable_ipv4": "true",
            "com.docker.network.enable_ipv6": "false"
        },
        "Labels": {
            "com.docker.compose.config-hash": "344ec53b4a9fb54fa9f613aa904c2d3f3d6f0ecd2981f09d9f9ccbd90c8bc682",
            "com.docker.compose.network": "default",
            "com.docker.compose.project": "cs-dev",
            "com.docker.compose.version": "2.40.3"
        },
        "Containers": {
            "02be2d3a5dcc726e539642404fcec3cd62669e2b1657d1c99278e0f5789d0e6b": {
                "Name": "cs-dev-www-trycopilot-ai-6",
                "EndpointID": "bfbaf01865e035a7fbd3cf19555df3a2f492a542563fe045b3f470db913da35b",
                "MacAddress": "b6:a0:41:69:b8:03",
                "IPv4Address": "172.18.0.2/16",
                "IPv6Address": ""
            },
            "1481ab018a8763154b6ac9ae904657a506ad8062d048f89a0e85e34c7edc677e": {
                "Name": "cs-dev-www-trycopilot-ai-9",
                "EndpointID": "5da3800a0ee542b77c4f1d4f83eaaa71e4942205d0f28bd97bfbea0a1088b094",
                "MacAddress": "de:3f:ce:4d:f9:95",
                "IPv4Address": "172.18.0.9/16",
                "IPv6Address": ""
            },
            "1cd929293125275525b55a51271cd7c3eeb84659037de0ff67b19562960ea884": {
                "Name": "cs-dev-www-trycopilot-ai-10",
                "EndpointID": "d3dcc42bb51aa3d355b2821c48973367deb79082cabe5508a70e59f9c2bebce1",
                "MacAddress": "9a:cb:cb:1d:76:77",
                "IPv4Address": "172.18.0.12/16",
                "IPv6Address": ""
            },
            "2a0468568737d55884670b9f6478d35a273690f17b3a4a216696372c229d4762": {
                "Name": "cs-service-registry",
                "EndpointID": "6a253800075ff049c0e271b29f172b5eb9d68671a65a1dc53af30c5d5f292276",
                "MacAddress": "6e:f3:ca:5f:27:98",
                "IPv4Address": "172.18.0.4/16",
                "IPv6Address": ""
            },
            "4037e13e1b5ebf85ddcd2994105e3293e954b729a399b7bc12a9b48169d70658": {
                "Name": "cs-dev-load-balancer-1",
                "EndpointID": "825be2ead80d842b54568ca62154c973149afc80541c0be9c7e8a7f9dadea8de",
                "MacAddress": "be:c8:24:a9:3a:e2",
                "IPv4Address": "172.18.0.15/16",
                "IPv6Address": ""
            },
            "468dec886f515ca0cff8b1c536684c71540798ef32c8c52d5dc0b049aec0ae9a": {
                "Name": "cs-dev-www-trycopilot-ai-8",
                "EndpointID": "92230a5d18220756c59b4d564b9fd64a27b2c0d24ee155799ed6e9739c7ce022",
                "MacAddress": "9a:7a:c8:07:76:3f",
                "IPv4Address": "172.18.0.11/16",
                "IPv6Address": ""
            },
            "6ea063383f20230440ff3d66ae17ed73bdb94e642e5cab5104b2fdc972bdf3fc": {
                "Name": "cs-dev-www-trycopilot-ai-4",
                "EndpointID": "9b35bf03481268f9c9bda899b095446e46cd05af967bcdf0ba3c05ccfab836de",
                "MacAddress": "a6:2a:63:2b:ad:55",
                "IPv4Address": "172.18.0.10/16",
                "IPv6Address": ""
            },
            "7cc97454de628e5f5ab82145082dc9c3462b1b8f2ca9e4b0f116493dd909a685": {
                "Name": "cs-dev-www-trycopilot-ai-1",
                "EndpointID": "1bf2269e753baab82023a4cea00c58e1f678868ccd1b90fec13bdc42a80aef60",
                "MacAddress": "62:8b:3e:cb:4f:05",
                "IPv4Address": "172.18.0.13/16",
                "IPv6Address": ""
            },
            "8973fc6b4f16ac098c65d539f0af70b65cbff3c7f0e9da63ec40980bb9fc1e02": {
                "Name": "cs-dev-www-trycopilot-ai-3",
                "EndpointID": "d569c9f3f14900f2264e304307f3bdc7d702239141575d94ccc66b08650a0945",
                "MacAddress": "52:a4:3a:ed:37:0e",
                "IPv4Address": "172.18.0.7/16",
                "IPv6Address": ""
            },
            "8ee7bc59560988709b15c97617fae28268c01bf011151818da17233395679163": {
                "Name": "cs-dev-www-trycopilot-ai-5",
                "EndpointID": "7084fe3c4017e5ad2379f04e5cf73ac5d25eb0722c1659c41f8156c4929eae89",
                "MacAddress": "ca:82:94:d8:52:9d",
                "IPv4Address": "172.18.0.8/16",
                "IPv6Address": ""
            },
            "a1f1e4f4ea41ca928f7b27a1381b2d8108223a636109ba4fb51700745ad74ca4": {
                "Name": "cs-dev-www-trycopilot-ai-7",
                "EndpointID": "f6d015985934c6112c18c4ed1bf748912e247836e3bdf50724786a610f84c5d2",
                "MacAddress": "1e:48:c4:26:0a:b3",
                "IPv4Address": "172.18.0.3/16",
                "IPv6Address": ""
            },
            "bec25985ef7a46150d3a39c80fc27d75a71da1c0ecd3917e21a43a5ac0cc7b3c": {
                "Name": "cs-dev-www-trycopilot-ai-2",
                "EndpointID": "48c44e813e6fe028abddb19b5c24d5d6882391ddbe2894eeeef2b595497558bd",
                "MacAddress": "82:61:14:a3:c4:24",
                "IPv4Address": "172.18.0.14/16",
                "IPv6Address": ""
            },
            "db6d368c78e2c0ee6533f33eaa2bee2ad8f87928b3fa1a5a5bb350f80694c42b": {
                "Name": "cs-ngrok",
                "EndpointID": "40735af46c8c3f77317edb8173e8d7a75f79c8e58b365380c5afb0a41891201d",
                "MacAddress": "ca:9b:33:fb:15:95",
                "IPv4Address": "172.18.0.6/16",
                "IPv6Address": ""
            }
        },
        "Status": {
            "IPAM": {
                "Subnets": {
                    "172.18.0.0/16": {
                        "IPsInUse": 17,
                        "DynamicIPsAvailable": 65519
                    }
                }
            }
        }
    }
])";

}  // namespace

class ServicesJsonTest : public ::testing::Test {};

// Test parsing the full services.json array
TEST_F(ServicesJsonTest, ParseServicesJsonArray) {
  auto parsed = ParseObject(kServicesJson);
  ASSERT_THAT(parsed.ok(), IsTrue()) << parsed.message();

  auto array = parsed.value().as(
      std::vector<cs::net::json::Object>());
  ASSERT_THAT(array.size(), Eq(1))
      << "Expected array with one network element";

  auto network_obj = array[0].as(
      std::map<std::string, cs::net::json::Object>());
  ASSERT_THAT(
      network_obj.find("Containers") != network_obj.end(),
      IsTrue())
      << "Expected Containers field in network object";
}

// Test parsing DockerNetworkInspectElement from the JSON
TEST_F(ServicesJsonTest, ParseDockerNetworkInspectElement) {
  auto parsed = ParseObject(kServicesJson);
  ASSERT_THAT(parsed.ok(), IsTrue());

  auto array = parsed.value().as(
      std::vector<cs::net::json::Object>());
  auto network_obj = array[0].as(
      std::map<std::string, cs::net::json::Object>());

  // Extract just the Containers field as JSON string
  auto containers_obj = network_obj["Containers"];
  std::string containers_json = containers_obj.str();

  // Create a minimal network inspect element JSON with just
  // Containers
  std::string network_json =
      R"({"Containers":)" + containers_json + "}";

  auto network_inspect =
      DockerNetworkInspectElement().Parse(network_json);
  ASSERT_THAT(network_inspect.ok(), IsTrue())
      << "Failed to parse DockerNetworkInspectElement: "
      << network_inspect.message();

  auto inspect = network_inspect.value();
  ASSERT_THAT(inspect.Containers.size(), Eq(13))
      << "Expected 13 containers in the network";

  // Verify specific containers
  ASSERT_THAT(
      inspect.Containers.find(
          "2a0468568737d55884670b9f6478d35a273690f17b3a4a21"
          "6696372c229d4762") != inspect.Containers.end(),
      IsTrue());

  auto service_registry = inspect.Containers.at(
      "2a0468568737d55884670b9f6478d35a273690f17b3a4a216696"
      "372c229d4762");
  ASSERT_THAT(service_registry.Name,
              StrEq("cs-service-registry"));
  ASSERT_THAT(service_registry.IPv4Address,
              StrEq("172.18.0.4/16"));
}

// Test DockerNetworkInspectElement round-trip serialization
TEST_F(ServicesJsonTest,
       DockerNetworkInspectElementRoundTrip) {
  // Parse original
  auto parsed = ParseObject(kServicesJson);
  ASSERT_THAT(parsed.ok(), IsTrue());

  auto array = parsed.value().as(
      std::vector<cs::net::json::Object>());
  auto network_obj = array[0].as(
      std::map<std::string, cs::net::json::Object>());
  auto containers_obj = network_obj["Containers"];
  std::string containers_json = containers_obj.str();
  std::string network_json =
      R"({"Containers":)" + containers_json + "}";

  auto network_inspect =
      DockerNetworkInspectElement().Parse(network_json);
  ASSERT_THAT(network_inspect.ok(), IsTrue());

  // Serialize back to JSON
  std::string serialized =
      network_inspect.value().Serialize();

  // Parse again
  auto roundtrip =
      DockerNetworkInspectElement().Parse(serialized);
  ASSERT_THAT(roundtrip.ok(), IsTrue())
      << "Round-trip parse failed: " << roundtrip.message();

  // Verify containers match
  ASSERT_THAT(
      roundtrip.value().Containers.size(),
      Eq(network_inspect.value().Containers.size()));

  for (const auto& [id, container] :
       network_inspect.value().Containers) {
    ASSERT_THAT(roundtrip.value().Containers.find(id) !=
                    roundtrip.value().Containers.end(),
                IsTrue());
    auto roundtrip_container =
        roundtrip.value().Containers.at(id);
    ASSERT_THAT(roundtrip_container.Name,
                StrEq(container.Name));
    ASSERT_THAT(roundtrip_container.IPv4Address,
                StrEq(container.IPv4Address));
  }
}

// Test DockerContainerInfo round-trip
TEST_F(ServicesJsonTest, DockerContainerInfoRoundTrip) {
  DockerContainerInfo original;
  original.Name = "cs-dev-www-trycopilot-ai-1";
  original.IPv4Address = "172.18.0.13/16";

  std::string serialized = original.Serialize();
  auto parsed = DockerContainerInfo().Parse(serialized);

  ASSERT_THAT(parsed.ok(), IsTrue());
  ASSERT_THAT(parsed.value().Name, StrEq(original.Name));
  ASSERT_THAT(parsed.value().IPv4Address,
              StrEq(original.IPv4Address));
}

// Test ServiceEntry round-trip
TEST_F(ServicesJsonTest, ServiceEntryRoundTrip) {
  ServiceEntry original;
  original.name = "www-trycopilot-ai";
  original.container_name = "cs-dev-www-trycopilot-ai-1";
  original.ip_address = "172.18.0.13";
  original.host = "www-trycopilot-ai";
  original.port = 8080;
  original.last_updated = "2026-01-04T04:13:02.888827919Z";

  std::string serialized = original.Serialize();
  auto parsed = ServiceEntry().Parse(serialized);

  ASSERT_THAT(parsed.ok(), IsTrue())
      << "Parse failed: " << parsed.message();
  ASSERT_THAT(parsed.value().name, StrEq(original.name));
  ASSERT_THAT(parsed.value().container_name,
              StrEq(original.container_name));
  ASSERT_THAT(parsed.value().ip_address,
              StrEq(original.ip_address));
  ASSERT_THAT(parsed.value().host, StrEq(original.host));
  ASSERT_THAT(parsed.value().port, Eq(original.port));
  ASSERT_THAT(parsed.value().last_updated,
              StrEq(original.last_updated));

  // Round-trip again
  std::string roundtrip_serialized =
      parsed.value().Serialize();
  auto roundtrip_parsed =
      ServiceEntry().Parse(roundtrip_serialized);

  ASSERT_THAT(roundtrip_parsed.ok(), IsTrue());
  ASSERT_THAT(roundtrip_parsed.value().name,
              StrEq(original.name));
  ASSERT_THAT(roundtrip_parsed.value().container_name,
              StrEq(original.container_name));
}

// Test ServiceRegistry with multiple entries round-trip
TEST_F(ServicesJsonTest, ServiceRegistryRoundTrip) {
  ServiceRegistry original;
  original.network_name = "cs-dev_default";
  original.prefix = "cs";
  original.last_updated = "2026-01-04T04:13:02.888827919Z";
  original.uuid = "test-uuid-123";

  // Add multiple service entries
  ServiceEntry entry1;
  entry1.name = "www-trycopilot-ai";
  entry1.container_name = "cs-dev-www-trycopilot-ai-1";
  entry1.ip_address = "172.18.0.13";
  entry1.host = "www-trycopilot-ai";
  entry1.port = 8080;
  entry1.last_updated = "2026-01-04T04:13:02.888827919Z";
  original.services.push_back(entry1);

  ServiceEntry entry2;
  entry2.name = "service-registry";
  entry2.container_name = "cs-service-registry";
  entry2.ip_address = "172.18.0.4";
  entry2.host = "service-registry";
  entry2.port = 8080;
  entry2.last_updated = "2026-01-04T04:13:02.888827919Z";
  original.services.push_back(entry2);

  ServiceEntry entry3;
  entry3.name = "load-balancer";
  entry3.container_name = "cs-dev-load-balancer-1";
  entry3.ip_address = "172.18.0.15";
  entry3.host = "load-balancer";
  entry3.port = 8080;
  entry3.last_updated = "2026-01-04T04:13:02.888827919Z";
  original.services.push_back(entry3);

  std::string serialized = original.Serialize();
  auto parsed = ServiceRegistry().Parse(serialized);

  ASSERT_THAT(parsed.ok(), IsTrue())
      << "Parse failed: " << parsed.message();
  ASSERT_THAT(parsed.value().network_name,
              StrEq(original.network_name));
  ASSERT_THAT(parsed.value().prefix,
              StrEq(original.prefix));
  ASSERT_THAT(parsed.value().last_updated,
              StrEq(original.last_updated));
  ASSERT_THAT(parsed.value().uuid, StrEq(original.uuid));
  ASSERT_THAT(parsed.value().services.size(),
              Eq(original.services.size()));

  // Verify each service entry
  for (size_t i = 0; i < original.services.size(); ++i) {
    ASSERT_THAT(parsed.value().services[i].name,
                StrEq(original.services[i].name));
    ASSERT_THAT(parsed.value().services[i].container_name,
                StrEq(original.services[i].container_name));
    ASSERT_THAT(parsed.value().services[i].ip_address,
                StrEq(original.services[i].ip_address));
    ASSERT_THAT(parsed.value().services[i].host,
                StrEq(original.services[i].host));
    ASSERT_THAT(parsed.value().services[i].port,
                Eq(original.services[i].port));
  }

  // Round-trip again
  std::string roundtrip_serialized =
      parsed.value().Serialize();
  auto roundtrip_parsed =
      ServiceRegistry().Parse(roundtrip_serialized);

  ASSERT_THAT(roundtrip_parsed.ok(), IsTrue());
  ASSERT_THAT(roundtrip_parsed.value().services.size(),
              Eq(original.services.size()));
}

// Test ServiceRegistry with empty services list
TEST_F(ServicesJsonTest, ServiceRegistryEmptyServices) {
  ServiceRegistry original;
  original.network_name = "test_network";
  original.prefix = "test";
  original.last_updated = "2026-01-04T04:13:02.888827919Z";
  original.uuid = "empty-uuid";
  // services is empty

  std::string serialized = original.Serialize();
  auto parsed = ServiceRegistry().Parse(serialized);

  ASSERT_THAT(parsed.ok(), IsTrue());
  ASSERT_THAT(parsed.value().services.size(), Eq(0));
  ASSERT_THAT(parsed.value().network_name,
              StrEq(original.network_name));
}

// Test extracting containers from the full services.json
TEST_F(ServicesJsonTest,
       ExtractContainersFromServicesJson) {
  auto parsed = ParseObject(kServicesJson);
  ASSERT_THAT(parsed.ok(), IsTrue());

  auto array = parsed.value().as(
      std::vector<cs::net::json::Object>());
  auto network_obj = array[0].as(
      std::map<std::string, cs::net::json::Object>());
  auto containers_obj = network_obj["Containers"];
  std::string containers_json = containers_obj.str();
  std::string network_json =
      R"({"Containers":)" + containers_json + "}";

  auto network_inspect =
      DockerNetworkInspectElement().Parse(network_json);
  ASSERT_THAT(network_inspect.ok(), IsTrue());

  // Verify we can find all expected container types
  int www_count = 0;
  int other_count = 0;

  for (const auto& [id, container] :
       network_inspect.value().Containers) {
    if (container.Name.find("www-trycopilot-ai") !=
        std::string::npos) {
      www_count++;
    } else {
      other_count++;
    }
  }

  ASSERT_THAT(www_count, Eq(10))
      << "Expected 10 www-trycopilot-ai containers";
  ASSERT_THAT(other_count, Eq(3))
      << "Expected 3 other containers (service-registry, "
         "load-balancer, ngrok)";

  // Verify specific container names
  bool found_service_registry = false;
  bool found_load_balancer = false;
  bool found_ngrok = false;

  for (const auto& [id, container] :
       network_inspect.value().Containers) {
    if (container.Name == "cs-service-registry") {
      found_service_registry = true;
      ASSERT_THAT(container.IPv4Address,
                  StrEq("172.18.0.4/16"));
    } else if (container.Name == "cs-dev-load-balancer-1") {
      found_load_balancer = true;
      ASSERT_THAT(container.IPv4Address,
                  StrEq("172.18.0.15/16"));
    } else if (container.Name == "cs-ngrok") {
      found_ngrok = true;
      ASSERT_THAT(container.IPv4Address,
                  StrEq("172.18.0.6/16"));
    }
  }

  ASSERT_THAT(found_service_registry, IsTrue());
  ASSERT_THAT(found_load_balancer, IsTrue());
  ASSERT_THAT(found_ngrok, IsTrue());
}

// Exhaustively verify all containers and their fields from
// services.json
TEST_F(ServicesJsonTest,
       ExhaustiveContainerFieldVerification) {
  // Parse the full JSON structure
  auto parsed = ParseObject(kServicesJson);
  ASSERT_THAT(parsed.ok(), IsTrue());

  auto array = parsed.value().as(
      std::vector<cs::net::json::Object>());
  auto network_obj = array[0].as(
      std::map<std::string, cs::net::json::Object>());
  auto containers_result = network_obj.find("Containers");
  ASSERT_THAT(containers_result != network_obj.end(),
              IsTrue());
  auto containers_obj = containers_result->second;

  // Extract containers as JSON map
  auto containers_map = containers_obj.as(
      std::map<std::string, cs::net::json::Object>());

  // Build expected container data directly from JSON Object
  std::map<std::string, std::pair<std::string, std::string>>
      expected_containers;
  for (const auto& [container_id, container_obj] :
       containers_map) {
    auto container_data = container_obj.as(
        std::map<std::string, cs::net::json::Object>());
    auto name_result = container_obj.get("Name");
    ASSERT_THAT(name_result.ok(), IsTrue())
        << "Failed to get Name for container "
        << container_id;
    std::string expected_name =
        name_result.value().as(std::string());
    auto ip_result = container_obj.get("IPv4Address");
    ASSERT_THAT(ip_result.ok(), IsTrue())
        << "Failed to get IPv4Address for container "
        << container_id;
    std::string expected_ip =
        ip_result.value().as(std::string());
    expected_containers[container_id] =
        std::make_pair(expected_name, expected_ip);
  }

  // Verify we extracted all expected containers from JSON
  ASSERT_THAT(expected_containers.size(), Eq(13))
      << "Expected 13 containers in services.json";

  // Verify each container can be parsed individually via
  // proto
  for (const auto& [container_id, expected] :
       expected_containers) {
    // Create a single container JSON object
    std::string container_json =
        R"({"Name":")" + expected.first +
        R"(","IPv4Address":")" + expected.second + R"("})";
    auto container_info =
        DockerContainerInfo().Parse(container_json);
    ASSERT_THAT(container_info.ok(), IsTrue())
        << "Failed to parse container " << container_id
        << ": " << container_info.message();

    // Verify every field matches exactly
    ASSERT_THAT(container_info.value().Name,
                StrEq(expected.first))
        << "Name mismatch for container " << container_id
        << ": expected '" << expected.first << "', got '"
        << container_info.value().Name << "'";
    ASSERT_THAT(container_info.value().IPv4Address,
                StrEq(expected.second))
        << "IPv4Address mismatch for container "
        << container_id << ": expected '" << expected.second
        << "', got '" << container_info.value().IPv4Address
        << "'";

    // Round-trip test for individual container
    std::string serialized =
        container_info.value().Serialize();
    auto roundtrip =
        DockerContainerInfo().Parse(serialized);
    ASSERT_THAT(roundtrip.ok(), IsTrue());
    ASSERT_THAT(roundtrip.value().Name,
                StrEq(expected.first));
    ASSERT_THAT(roundtrip.value().IPv4Address,
                StrEq(expected.second));
  }
}

// Exhaustively test that we can crawl the entire JSON tree
// and verify all container fields
TEST_F(ServicesJsonTest, ExhaustiveJsonTreeCrawl) {
  // Parse original JSON
  auto parsed = ParseObject(kServicesJson);
  ASSERT_THAT(parsed.ok(), IsTrue());

  auto array = parsed.value().as(
      std::vector<cs::net::json::Object>());
  ASSERT_THAT(array.size(), Eq(1));

  auto network_obj = array[0].as(
      std::map<std::string, cs::net::json::Object>());

  // Verify network-level fields exist
  ASSERT_THAT(network_obj.find("Name") != network_obj.end(),
              IsTrue());
  ASSERT_THAT(
      network_obj.find("Containers") != network_obj.end(),
      IsTrue());

  auto name_obj = network_obj["Name"];
  ASSERT_THAT(name_obj.is(std::string()), IsTrue());
  std::string network_name = name_obj.as(std::string());
  ASSERT_THAT(network_name, StrEq("cs-dev_default"));

  // Exhaustively crawl all containers
  auto containers_obj = network_obj["Containers"];
  auto containers_map = containers_obj.as(
      std::map<std::string, cs::net::json::Object>());

  ASSERT_THAT(containers_map.size(), Eq(13));

  // Verify every container has all required fields
  for (const auto& [container_id, container_obj] :
       containers_map) {
    auto container_data = container_obj.as(
        std::map<std::string, cs::net::json::Object>());

    // Verify all expected fields are present
    ASSERT_THAT(
        container_data.find("Name") != container_data.end(),
        IsTrue())
        << "Container " << container_id
        << " missing Name field";
    ASSERT_THAT(container_data.find("IPv4Address") !=
                    container_data.end(),
                IsTrue())
        << "Container " << container_id
        << " missing IPv4Address field";

    // Extract and verify field values
    auto name_result = container_obj.get("Name");
    ASSERT_THAT(name_result.ok(), IsTrue());
    std::string name =
        name_result.value().as(std::string());
    ASSERT_THAT(name.length() > 0, IsTrue())
        << "Container " << container_id
        << " has empty Name";

    auto ip_result = container_obj.get("IPv4Address");
    ASSERT_THAT(ip_result.ok(), IsTrue());
    std::string ip = ip_result.value().as(std::string());
    ASSERT_THAT(ip.length() > 0, IsTrue())
        << "Container " << container_id
        << " has empty IPv4Address";
    ASSERT_THAT(ip.find('/') != std::string::npos, IsTrue())
        << "Container " << container_id
        << " IPv4Address should contain CIDR notation";
  }
}

// Test individual container round-trip for all containers
TEST_F(ServicesJsonTest,
       ExhaustiveIndividualContainerRoundTrip) {
  // Parse original JSON
  auto parsed = ParseObject(kServicesJson);
  ASSERT_THAT(parsed.ok(), IsTrue());

  auto array = parsed.value().as(
      std::vector<cs::net::json::Object>());
  auto network_obj = array[0].as(
      std::map<std::string, cs::net::json::Object>());
  auto containers_obj = network_obj["Containers"];
  std::string containers_json = containers_obj.str();
  std::string network_json =
      R"({"Containers":)" + containers_json + "}";

  auto network_inspect =
      DockerNetworkInspectElement().Parse(network_json);
  ASSERT_THAT(network_inspect.ok(), IsTrue());

  // Test round-trip for each individual container
  for (const auto& [container_id, original] :
       network_inspect.value().Containers) {
    // Serialize individual container
    std::string serialized = original.Serialize();

    // Parse it back
    auto parsed_container =
        DockerContainerInfo().Parse(serialized);
    ASSERT_THAT(parsed_container.ok(), IsTrue())
        << "Failed to parse container " << container_id
        << ": " << parsed_container.message();

    const auto& roundtrip = parsed_container.value();

    // Verify every field matches
    ASSERT_THAT(roundtrip.Name, StrEq(original.Name))
        << "Name mismatch for container " << container_id;
    ASSERT_THAT(roundtrip.IPv4Address,
                StrEq(original.IPv4Address))
        << "IPv4Address mismatch for container "
        << container_id;
  }
}

// Test ServiceEntry with all fields populated
TEST_F(ServicesJsonTest, ServiceEntryAllFields) {
  ServiceEntry entry;
  entry.name = "test-service";
  entry.container_name = "cs-test-service-1";
  entry.ip_address = "192.168.1.100";
  entry.host = "test-service";
  entry.port = 9090;
  entry.last_updated = "2026-01-04T12:34:56.789012345Z";

  std::string serialized = entry.Serialize();
  auto parsed = ServiceEntry().Parse(serialized);

  ASSERT_THAT(parsed.ok(), IsTrue());
  ASSERT_THAT(parsed.value().name, StrEq("test-service"));
  ASSERT_THAT(parsed.value().container_name,
              StrEq("cs-test-service-1"));
  ASSERT_THAT(parsed.value().ip_address,
              StrEq("192.168.1.100"));
  ASSERT_THAT(parsed.value().host, StrEq("test-service"));
  ASSERT_THAT(parsed.value().port, Eq(9090));
  ASSERT_THAT(parsed.value().last_updated,
              StrEq("2026-01-04T12:34:56.789012345Z"));
}

// Test ServiceRegistry serialization preserves order
TEST_F(ServicesJsonTest, ServiceRegistryPreservesOrder) {
  ServiceRegistry registry;
  registry.network_name = "test_network";
  registry.prefix = "test";
  registry.last_updated = "2026-01-04T04:13:02.888827919Z";
  registry.uuid = "order-test";

  // Add services in specific order
  std::vector<std::string> expected_names = {
      "alpha", "beta", "gamma"};
  for (const auto& name : expected_names) {
    ServiceEntry entry;
    entry.name = name;
    entry.container_name = "cs-" + name;
    entry.ip_address = "172.18.0.1";
    entry.host = name;
    entry.port = 8080;
    entry.last_updated = "2026-01-04T04:13:02.888827919Z";
    registry.services.push_back(entry);
  }

  std::string serialized = registry.Serialize();
  auto parsed = ServiceRegistry().Parse(serialized);

  ASSERT_THAT(parsed.ok(), IsTrue());
  ASSERT_THAT(parsed.value().services.size(),
              Eq(expected_names.size()));

  for (size_t i = 0; i < expected_names.size(); ++i) {
    ASSERT_THAT(parsed.value().services[i].name,
                StrEq(expected_names[i]));
  }
}
