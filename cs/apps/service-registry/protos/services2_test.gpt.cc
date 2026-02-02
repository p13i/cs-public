// cs/apps/service-registry/protos/services2_test.gpt.cc
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

// Sample services2.json from docker network inspect output
constexpr const char* kServices2Json = R"([
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
            "04344d83e03fb5b8353d6e10a99b13782e90019d11753702c42cc646e257b866": {
                "Name": "cs-dev-www-trycopilot-ai-1",
                "EndpointID": "f452b01f21563dc6f5efe071cb69724603b9c3951d8ef3624bfb76e77b426da5",
                "MacAddress": "ae:7b:95:1c:df:0f",
                "IPv4Address": "172.18.0.14/16",
                "IPv6Address": ""
            },
            "0780d62ac8e70c460cba2167415ed3b441fd1d72db0dc8588b8478f43a295690": {
                "Name": "cs-dev-www-trycopilot-ai-4",
                "EndpointID": "a9231ee6d988f185fe52ac867b434ed2525967b0659d359b36fca29dde8744b7",
                "MacAddress": "02:61:7e:c0:47:d5",
                "IPv4Address": "172.18.0.5/16",
                "IPv6Address": ""
            },
            "1e4b28cd44bcb7e3d4d204d15db92dcfa668b375143f0d2cc712baed17cfaa45": {
                "Name": "cs-dev-www-trycopilot-ai-9",
                "EndpointID": "9e6c0ac42022c861001e701a1a3e0efb84e4fa468722bd595be1dc3a7fee249b",
                "MacAddress": "b2:82:c8:a2:8c:5b",
                "IPv4Address": "172.18.0.8/16",
                "IPv6Address": ""
            },
            "216b2632ff6e01198526c64860a793618b630f306880f6f3781515b684e1b9bb": {
                "Name": "cs-service-registry",
                "EndpointID": "06eb5077858e558e2e4945aad1303430df976191d64a097c606cb241b6bd1353",
                "MacAddress": "1e:79:d6:4e:32:e2",
                "IPv4Address": "172.18.0.7/16",
                "IPv6Address": ""
            },
            "3af594954f81c5fa36b10bffe3feb6d4d1c9a06eef69fe5b2d4cc99840f7c715": {
                "Name": "cs-dev-www-trycopilot-ai-2",
                "EndpointID": "50a496c4ebb542f87c1269b6cb31a0e24d1209175274aa858c65432cd3fcf374",
                "MacAddress": "92:d9:cf:ef:26:fd",
                "IPv4Address": "172.18.0.2/16",
                "IPv6Address": ""
            },
            "3cca26ce6d9d914516084a9113650a23cc8326c0d4d021b15272c402e204ca54": {
                "Name": "cs-dev-www-trycopilot-ai-3",
                "EndpointID": "94a54bc84dbd376a5c54c1d289ac7d02efafc384df1c0f73dcd1d96b66f1c6e4",
                "MacAddress": "d6:56:24:85:b6:bc",
                "IPv4Address": "172.18.0.13/16",
                "IPv6Address": ""
            },
            "47b660ecd8bcbed5d4134de578bdb0416b43c8c0c31f11549435aee5f4f467ef": {
                "Name": "cs-dev-www-trycopilot-ai-8",
                "EndpointID": "2c70c901f620540be10b47104e45bf6f6fd31e6d3c25002cf19ec262e10cd0ab",
                "MacAddress": "2e:5b:4f:b8:fa:d6",
                "IPv4Address": "172.18.0.6/16",
                "IPv6Address": ""
            },
            "5a5c2fd5ae6deacff337a11a331442dd3de36daef9803600c68a4d3ba0c116a5": {
                "Name": "cs-dev-www-trycopilot-ai-6",
                "EndpointID": "aa93bcaaaf7d3d5fc239c721a2fa996ac6e4db36595029e595b54ceaca5a2188",
                "MacAddress": "1e:a7:56:34:64:dd",
                "IPv4Address": "172.18.0.11/16",
                "IPv6Address": ""
            },
            "67d3c0e582c4a4b35c79fc2961c6baf313c53f8ec57c863a65c87e403c282861": {
                "Name": "cs-ngrok",
                "EndpointID": "e3a26ee169e52c02575c828de5eb3902a7d87db066b48c23462de7ed2a589c4a",
                "MacAddress": "82:7a:8f:a6:63:4b",
                "IPv4Address": "172.18.0.3/16",
                "IPv6Address": ""
            },
            "6bd913c2375f822453ef16e76baed124e9ebd01273535044d5492a35c569a3d5": {
                "Name": "cs-dev-www-trycopilot-ai-10",
                "EndpointID": "5c911b9797db3e8d911f65fe3d18911d0ea4457803d496d8dc43fd6e5c88b317",
                "MacAddress": "4e:0d:e7:50:2e:1c",
                "IPv4Address": "172.18.0.12/16",
                "IPv6Address": ""
            },
            "c03786ca764991df4990494cdb92e34f8cd6e61785a9ada3b13cbedb67f9476a": {
                "Name": "cs-dev-www-trycopilot-ai-5",
                "EndpointID": "c0562cda5fd9f4e08563e4aa26a4daab9967b9834abd3058879cd8862d1a915a",
                "MacAddress": "02:cc:a9:d4:92:cf",
                "IPv4Address": "172.18.0.10/16",
                "IPv6Address": ""
            },
            "c795dd74048e935e1a06acf07607a5c5b9ee99420d5f35e0095114b3fbf505d6": {
                "Name": "cs-dev-www-trycopilot-ai-7",
                "EndpointID": "f7998697dcde10fcb76d6fe1bcab32cf5cd081beddd03918fa031c393b92d1b4",
                "MacAddress": "9e:fe:2f:bc:1a:77",
                "IPv4Address": "172.18.0.9/16",
                "IPv6Address": ""
            }
        },
        "Status": {
            "IPAM": {
                "Subnets": {
                    "172.18.0.0/16": {
                        "IPsInUse": 16,
                        "DynamicIPsAvailable": 65520
                    }
                }
            }
        }
    }
]
])";

}  // namespace

class Services2JsonTest : public ::testing::Test {};

// Test parsing the full services2.json array
TEST_F(Services2JsonTest, ParseServicesJsonArray) {
  auto parsed = ParseObject(kServices2Json);
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
TEST_F(Services2JsonTest,
       ParseDockerNetworkInspectElement) {
  auto parsed = ParseObject(kServices2Json);
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
  ASSERT_THAT(inspect.Containers.size(), Eq(12))
      << "Expected 12 containers in the network";

  // Verify specific containers
  ASSERT_THAT(
      inspect.Containers.find(
          "216b2632ff6e01198526c64860a793618b630f306880f6f3"
          "781515b684e1b9bb") != inspect.Containers.end(),
      IsTrue());

  auto service_registry = inspect.Containers.at(
      "216b2632ff6e01198526c64860a793618b630f306880f6f37815"
      "15b684e1b9bb");
  ASSERT_THAT(service_registry.Name,
              StrEq("cs-service-registry"));
  ASSERT_THAT(service_registry.IPv4Address,
              StrEq("172.18.0.7/16"));
}

// Test DockerNetworkInspectElement round-trip serialization
TEST_F(Services2JsonTest,
       DockerNetworkInspectElementRoundTrip) {
  // Parse original
  auto parsed = ParseObject(kServices2Json);
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
TEST_F(Services2JsonTest, DockerContainerInfoRoundTrip) {
  DockerContainerInfo original;
  original.Name = "cs-dev-www-trycopilot-ai-1";
  original.IPv4Address = "172.18.0.14/16";

  std::string serialized = original.Serialize();
  auto parsed = DockerContainerInfo().Parse(serialized);

  ASSERT_THAT(parsed.ok(), IsTrue());
  ASSERT_THAT(parsed.value().Name, StrEq(original.Name));
  ASSERT_THAT(parsed.value().IPv4Address,
              StrEq(original.IPv4Address));
}

// Test ServiceEntry round-trip
TEST_F(Services2JsonTest, ServiceEntryRoundTrip) {
  ServiceEntry original;
  original.name = "www-trycopilot-ai";
  original.container_name = "cs-dev-www-trycopilot-ai-1";
  original.ip_address = "172.18.0.14";
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
TEST_F(Services2JsonTest, ServiceRegistryRoundTrip) {
  ServiceRegistry original;
  original.network_name = "cs-dev_default";
  original.prefix = "cs";
  original.last_updated = "2026-01-04T04:13:02.888827919Z";
  original.uuid = "test-uuid-123";

  // Add multiple service entries
  ServiceEntry entry1;
  entry1.name = "www-trycopilot-ai";
  entry1.container_name = "cs-dev-www-trycopilot-ai-1";
  entry1.ip_address = "172.18.0.14";
  entry1.host = "www-trycopilot-ai";
  entry1.port = 8080;
  entry1.last_updated = "2026-01-04T04:13:02.888827919Z";
  original.services.push_back(entry1);

  ServiceEntry entry2;
  entry2.name = "service-registry";
  entry2.container_name = "cs-service-registry";
  entry2.ip_address = "172.18.0.7";
  entry2.host = "service-registry";
  entry2.port = 8080;
  entry2.last_updated = "2026-01-04T04:13:02.888827919Z";
  original.services.push_back(entry2);
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
TEST_F(Services2JsonTest, ServiceRegistryEmptyServices) {
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

// Test extracting containers from the full services2.json
TEST_F(Services2JsonTest,
       ExtractContainersFromServicesJson) {
  auto parsed = ParseObject(kServices2Json);
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
  ASSERT_THAT(other_count, Eq(2))
      << "Expected 2 other containers (service-registry, "
         "ngrok)";

  // Verify specific container names
  bool found_service_registry = false;
  bool found_ngrok = false;

  for (const auto& [id, container] :
       network_inspect.value().Containers) {
    if (container.Name == "cs-service-registry") {
      found_service_registry = true;
      ASSERT_THAT(container.IPv4Address,
                  StrEq("172.18.0.7/16"));
    } else if (container.Name == "cs-ngrok") {
      found_ngrok = true;
      ASSERT_THAT(container.IPv4Address,
                  StrEq("172.18.0.3/16"));
    }
  }

  ASSERT_THAT(found_service_registry, IsTrue());
  ASSERT_THAT(found_ngrok, IsTrue());
}

// Exhaustively verify all containers and their fields from
// services2.json
TEST_F(Services2JsonTest,
       ExhaustiveContainerFieldVerification) {
  // Parse the full JSON structure
  auto parsed = ParseObject(kServices2Json);
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
  ASSERT_THAT(expected_containers.size(), Eq(12))
      << "Expected 12 containers in services2.json";

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
TEST_F(Services2JsonTest, ExhaustiveJsonTreeCrawl) {
  // Parse original JSON
  auto parsed = ParseObject(kServices2Json);
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

  ASSERT_THAT(containers_map.size(), Eq(12));

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
TEST_F(Services2JsonTest,
       ExhaustiveIndividualContainerRoundTrip) {
  // Parse original JSON
  auto parsed = ParseObject(kServices2Json);
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
TEST_F(Services2JsonTest, ServiceEntryAllFields) {
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
TEST_F(Services2JsonTest, ServiceRegistryPreservesOrder) {
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
