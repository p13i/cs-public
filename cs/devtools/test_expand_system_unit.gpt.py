#!/usr/bin/env python3
# cs/devtools/test_expand_system_unit.gpt.py
"""
Unit tests for expand_system.gpt.py functions.

Run with: python3 cs/devtools/test_expand_system_unit.gpt.py
Or via Bazel: bazel run //cs/devtools:test_expand_system_unit
"""

import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import MagicMock, patch

# Import from expand_system module
import importlib.util

# Load expand_system.gpt.py as a module
expand_system_path = Path(__file__).parent / "expand_system.gpt.py"
spec = importlib.util.spec_from_file_location("expand_system", expand_system_path)
expand_system = importlib.util.module_from_spec(spec)
spec.loader.exec_module(expand_system)

# Import functions
get_replicas = expand_system.get_replicas
get_user_id = expand_system.get_user_id
get_network_config = expand_system.get_network_config
map_storage_volumes = expand_system.map_storage_volumes
get_service_command = expand_system.get_service_command
get_depends_on = expand_system.get_depends_on
compute_directory_hash = expand_system.compute_directory_hash
generate_ngrok_entrypoint = expand_system.generate_ngrok_entrypoint


class TestGetReplicas(unittest.TestCase):
    """Test get_replicas(service, env) - reads replicas (int or env-to-int dict)"""

    def test_int_passthrough(self):
        """replicas: 3 returns 3 for any env"""
        service = {"replicas": 3}
        self.assertEqual(get_replicas(service, "prod"), 3)
        self.assertEqual(get_replicas(service, "dev"), 3)

    def test_env_specific_dict(self):
        """replicas: { prod: 5, dev: 1 } returns env-specific value"""
        service = {"replicas": {"prod": 5, "dev": 1}}
        self.assertEqual(get_replicas(service, "prod"), 5)
        self.assertEqual(get_replicas(service, "dev"), 1)

    def test_dict_fallback_to_prod(self):
        """replicas: { prod: 3 } returns 3 for unknown env"""
        service = {"replicas": {"prod": 3}}
        self.assertEqual(get_replicas(service, "staging"), 3)

    def test_missing_key_defaults_to_one(self):
        """Service with no replicas key returns 1"""
        service = {}
        self.assertEqual(get_replicas(service, "prod"), 1)


class TestUserID(unittest.TestCase):
    """Test get_user_id() - maps security isolation to user IDs"""

    def test_unprivileged(self):
        """unprivileged isolation maps to myappuser"""
        security = {"isolation": "unprivileged"}
        self.assertEqual(get_user_id(security), "8877:8877")

    def test_privileged(self):
        """privileged isolation maps to root"""
        security = {"isolation": "privileged"}
        self.assertEqual(get_user_id(security), "0:0")

    def test_user_namespaced(self):
        """user-namespaced uses dynamic UID/GID from environment"""
        security = {"isolation": "user-namespaced"}
        self.assertEqual(get_user_id(security), "${UID}:${GID}")

    def test_default_isolation(self):
        """Empty security config defaults to unprivileged"""
        security = {}
        self.assertEqual(get_user_id(security), "8877:8877")

    def test_unknown_isolation(self):
        """Unknown isolation types default to unprivileged"""
        security = {"isolation": "custom-isolation"}
        self.assertEqual(get_user_id(security), "8877:8877")


class TestNetworkConfig(unittest.TestCase):
    """Test get_network_config() - converts abstract network to docker config"""

    def test_public_interface(self):
        """public interface should expose ports on host"""
        network = {"interface": "public", "port": 8080}
        result = get_network_config(network, "prod")
        self.assertEqual(result, {"ports": ["8080:8080"]})

    def test_private_interface(self):
        """private interface should only expose internally"""
        network = {"interface": "private", "port": 8080}
        result = get_network_config(network, "prod")
        self.assertEqual(result, {"expose": ["8080"]})

    def test_management_interface(self):
        """management interface should expose ports on host"""
        network = {"interface": "management", "port": 4040}
        result = get_network_config(network, "prod")
        self.assertEqual(result, {"ports": ["4040:4040"]})

    def test_env_specific_interface(self):
        """Environment-specific interface configuration"""
        network = {"interface": {"prod": "private", "dev": "public"}, "port": 8080}
        result_prod = get_network_config(network, "prod")
        result_dev = get_network_config(network, "dev")
        self.assertEqual(result_prod, {"expose": ["8080"]})
        self.assertEqual(result_dev, {"ports": ["8080:8080"]})

    def test_no_port(self):
        """No port specified should return empty config"""
        network = {"interface": "public"}
        result = get_network_config(network, "prod")
        self.assertEqual(result, {})

    def test_default_interface(self):
        """No interface specified defaults to private"""
        network = {"port": 8080}
        result = get_network_config(network, "prod")
        self.assertEqual(result, {"expose": ["8080"]})


class TestMapStorageVolumes(unittest.TestCase):
    """Test map_storage_volumes() - converts abstract storage to docker volumes"""

    def test_database_volume(self):
        """database-volume maps to database-volume:/data"""
        storage_list = [{"mount": "database-volume", "access": "read-write"}]
        result = map_storage_volumes(storage_list, {})
        self.assertEqual(result, ["database-volume:/data"])

    def test_read_only_mount(self):
        """read-only access adds :ro suffix"""
        storage_list = [{"mount": "database-volume", "access": "read-only"}]
        result = map_storage_volumes(storage_list, {})
        self.assertEqual(result, ["database-volume:/data:ro"])

    def test_multiple_mounts(self):
        """Multiple storage mounts should all be mapped"""
        storage_list = [
            {"mount": "database-volume", "access": "read-write"},
            {"mount": "custom-storage", "access": "read-write"},
        ]
        result = map_storage_volumes(storage_list, {})
        self.assertEqual(
            result, ["database-volume:/data", "custom-storage:/data/custom_storage"]
        )

    def test_unknown_storage(self):
        """Unknown storage types use default naming"""
        storage_list = [{"mount": "custom-storage", "access": "read-write"}]
        result = map_storage_volumes(storage_list, {})
        self.assertEqual(result, ["custom-storage:/data/custom_storage"])


class TestServiceCommand(unittest.TestCase):
    """Test get_service_command() - generates command arrays for services"""

    def test_database_service(self):
        """database-service gets special data_dir argument"""
        service = {"network": {"port": 8080}}
        cmd = get_service_command("database-service", service)
        self.assertEqual(
            cmd, ["/main", "--host=0.0.0.0", "--data_dir=/data", "--port=8080"]
        )

    def test_service_registry(self):
        """service-registry gets host and port arguments"""
        service = {"network": {"port": 8080}}
        cmd = get_service_command("service-registry", service)
        self.assertEqual(cmd, ["/main", "--host=0.0.0.0", "--port=8080"])

    def test_web_service_with_config(self):
        """Web services with VERSION/COMMIT config get those args"""
        service = {"network": {"port": 8080}, "config": ["VERSION", "COMMIT"]}
        cmd = get_service_command("www-trycopilot-ai", service)
        self.assertEqual(
            cmd,
            [
                "/main",
                "--host=0.0.0.0",
                "--port=8080",
                "--version=${VERSION}",
                "--commit=${COMMIT}",
            ],
        )

    def test_web_service_without_config(self):
        """Web services without config just get host and port"""
        service = {"network": {"port": 8080}, "config": []}
        cmd = get_service_command("www-trycopilot-ai", service)
        self.assertEqual(cmd, ["/main", "--host=0.0.0.0", "--port=8080"])

    def test_code_viewer(self):
        """code-viewer service gets same treatment as trycopilot"""
        service = {"network": {"port": 8080}, "config": ["VERSION", "COMMIT"]}
        cmd = get_service_command("code-viewer", service)
        self.assertEqual(
            cmd,
            [
                "/main",
                "--host=0.0.0.0",
                "--port=8080",
                "--version=${VERSION}",
                "--commit=${COMMIT}",
            ],
        )

    def test_load_balancer(self):
        """load-balancer gets standard host/port command (no balancer-state)"""
        service = {"network": {"port": 8080}}
        cmd = get_service_command("load-balancer", service)
        self.assertEqual(cmd, ["/main", "--host=0.0.0.0", "--port=8080"])

    def test_default_port(self):
        """Services without explicit port use 8080"""
        service = {}
        cmd = get_service_command("database-service", service)
        self.assertIn("--port=8080", cmd)


class TestDependsOn(unittest.TestCase):
    """Test get_depends_on() - converts abstract dependencies to docker depends_on"""

    def test_empty_dependencies(self):
        """Empty dependency list returns empty dict"""
        result = get_depends_on([])
        self.assertEqual(result, {})

    def test_none_dependencies(self):
        """None dependencies returns empty dict"""
        result = get_depends_on(None)
        self.assertEqual(result, {})

    def test_string_dependency(self):
        """String dependency gets service_started condition"""
        deps = ["database-service"]
        result = get_depends_on(deps)
        self.assertEqual(result, {"database-service": {"condition": "service_started"}})

    def test_dict_dependency_with_requirement(self):
        """Dict dependency with requirement=available"""
        deps = [
            {
                "service": "www-trycopilot-ai",
                "relationship": "routes-to",
                "requirement": "available",
            }
        ]
        result = get_depends_on(deps)
        self.assertEqual(
            result, {"www-trycopilot-ai": {"condition": "service_started"}}
        )

    def test_dict_dependency_without_requirement(self):
        """Dict dependency without requirement still gets service_started"""
        deps = [{"service": "load-balancer", "relationship": "exposes"}]
        result = get_depends_on(deps)
        self.assertEqual(result, {"load-balancer": {"condition": "service_started"}})

    def test_multiple_dependencies(self):
        """Multiple dependencies of mixed types"""
        deps = [
            "database-service",
            {"service": "load-balancer", "requirement": "available"},
        ]
        result = get_depends_on(deps)
        self.assertEqual(
            result,
            {
                "database-service": {"condition": "service_started"},
                "load-balancer": {"condition": "service_started"},
            },
        )


class TestDirectoryHash(unittest.TestCase):
    """Test compute_directory_hash() - deterministic file content hashing"""

    def test_nonexistent_directory(self):
        """Nonexistent directory returns valid 12-char hash"""
        fake_dir = Path("/nonexistent/fake/directory/xyz123")
        result = compute_directory_hash(fake_dir)
        self.assertEqual(len(result), 12)
        self.assertRegex(result, r"^[a-f0-9]{12}$")

    def test_deterministic_hashing(self):
        """Same directory content produces same hash"""
        with tempfile.TemporaryDirectory() as tmpdir:
            test_dir = Path(tmpdir)
            (test_dir / "file1.txt").write_text("content1")
            (test_dir / "file2.txt").write_text("content2")
            (test_dir / "subdir").mkdir()
            (test_dir / "subdir" / "file3.txt").write_text("content3")

            hash1 = compute_directory_hash(test_dir)
            hash2 = compute_directory_hash(test_dir)

            self.assertEqual(hash1, hash2)
            self.assertEqual(len(hash1), 12)

    def test_different_content_different_hash(self):
        """Different content produces different hash"""
        with tempfile.TemporaryDirectory() as tmpdir1, tempfile.TemporaryDirectory() as tmpdir2:
            dir1 = Path(tmpdir1)
            dir2 = Path(tmpdir2)

            (dir1 / "file.txt").write_text("content1")
            (dir2 / "file.txt").write_text("content2")

            hash1 = compute_directory_hash(dir1)
            hash2 = compute_directory_hash(dir2)

            self.assertNotEqual(hash1, hash2)

    def test_different_filenames_different_hash(self):
        """Different filenames produce different hash even with same content"""
        with tempfile.TemporaryDirectory() as tmpdir1, tempfile.TemporaryDirectory() as tmpdir2:
            dir1 = Path(tmpdir1)
            dir2 = Path(tmpdir2)

            (dir1 / "file1.txt").write_text("same content")
            (dir2 / "file2.txt").write_text("same content")

            hash1 = compute_directory_hash(dir1)
            hash2 = compute_directory_hash(dir2)

            self.assertNotEqual(hash1, hash2)

    def test_hash_includes_subdirectories(self):
        """Hash changes when files in subdirectories change"""
        with tempfile.TemporaryDirectory() as tmpdir:
            test_dir = Path(tmpdir)
            (test_dir / "file.txt").write_text("content")
            hash1 = compute_directory_hash(test_dir)

            # Add subdirectory with file
            (test_dir / "subdir").mkdir()
            (test_dir / "subdir" / "nested.txt").write_text("nested")
            hash2 = compute_directory_hash(test_dir)

            self.assertNotEqual(hash1, hash2)


class TestNgrokEntrypoint(unittest.TestCase):
    """Test generate_ngrok_entrypoint() - generates ngrok config script"""

    def test_single_service_single_domain_prod(self):
        """Basic single service with one domain"""
        expose_config = {"load-balancer": {"prod": ["www.trycopilot.ai"]}}
        services = {"load-balancer": {"network": {"port": 8080}}}

        script = generate_ngrok_entrypoint(expose_config, "prod", services)

        # Check YAML config structure
        self.assertIn("version: '3'", script)
        self.assertIn("agent:", script)
        self.assertIn(
            "authtoken: 382hPPpjmtecc79LYbukRGmD2o3_GArhu6V5osRWjAhSNkfc", script
        )
        self.assertIn("tunnels:", script)

        # Check tunnel configuration
        self.assertIn("www-trycopilot-ai:", script)
        self.assertIn("proto: http", script)
        self.assertIn("addr: load-balancer:8080", script)
        self.assertIn("domain: www.trycopilot.ai", script)

        # Check exec command
        self.assertIn("exec ngrok start --config /tmp/ngrok.yml", script)
        self.assertIn("www-trycopilot-ai", script.split("exec ngrok start")[1])

    def test_multiple_domains(self):
        """Multiple domains create multiple tunnel entries"""
        expose_config = {
            "load-balancer": {
                "prod": ["www.trycopilot.ai", "code.trycopilot.ai", "www.cite.pub"]
            }
        }
        services = {"load-balancer": {"network": {"port": 8080}}}

        script = generate_ngrok_entrypoint(expose_config, "prod", services)

        # Check all domains present
        self.assertIn("www-trycopilot-ai:", script)
        self.assertIn("code-trycopilot-ai:", script)
        self.assertIn("www-cite-pub:", script)

        # Check all tunnel names in exec command
        exec_line = script.split("exec ngrok start")[1]
        self.assertIn("www-trycopilot-ai", exec_line)
        self.assertIn("code-trycopilot-ai", exec_line)
        self.assertIn("www-cite-pub", exec_line)

    def test_dev_environment_domains(self):
        """Dev environment uses dev-prefixed domains"""
        expose_config = {
            "load-balancer": {
                "prod": ["www.trycopilot.ai"],
                "dev": ["dev.www.trycopilot.ai"],
            }
        }
        services = {"load-balancer": {"network": {"port": 8080}}}

        script = generate_ngrok_entrypoint(expose_config, "dev", services)

        # Check dev domain present
        self.assertIn("dev-www-trycopilot-ai:", script)
        self.assertIn("domain: dev.www.trycopilot.ai", script)

        # Check prod domain NOT present
        self.assertNotIn("domain: www.trycopilot.ai", script)

    def test_domain_name_sanitization(self):
        """Dots in domains converted to dashes for tunnel names"""
        expose_config = {"load-balancer": {"prod": ["www.trycopilot.ai"]}}
        services = {"load-balancer": {"network": {"port": 8080}}}

        script = generate_ngrok_entrypoint(expose_config, "prod", services)

        # Tunnel name should use dashes
        self.assertIn("www-trycopilot-ai:", script)
        # But domain should keep dots
        self.assertIn("domain: www.trycopilot.ai", script)

    def test_service_port_detection(self):
        """Service port should be read from network config"""
        expose_config = {"custom-service": {"prod": ["example.com"]}}
        services = {"custom-service": {"network": {"port": 9090}}}

        script = generate_ngrok_entrypoint(expose_config, "prod", services)

        self.assertIn("addr: custom-service:9090", script)

    def test_default_port_fallback(self):
        """Missing port defaults to 8080"""
        expose_config = {"some-service": {"prod": ["example.com"]}}
        services = {"some-service": {"network": {}}}

        script = generate_ngrok_entrypoint(expose_config, "prod", services)

        self.assertIn("addr: some-service:8080", script)

    def test_empty_domains_skipped(self):
        """Services with no domains for env are skipped"""
        expose_config = {"load-balancer": {"prod": ["www.trycopilot.ai"], "dev": []}}
        services = {"load-balancer": {"network": {"port": 8080}}}

        script = generate_ngrok_entrypoint(expose_config, "dev", services)

        # Should have empty tunnels section
        self.assertIn("tunnels:", script)
        self.assertNotIn("www-trycopilot-ai:", script)


class TestDependsOnEdgeCases(unittest.TestCase):
    """Additional edge case tests for get_depends_on()"""

    def test_mixed_string_and_dict_dependencies(self):
        """Mix of string and dict dependencies"""
        deps = [
            "database-service",
            {"service": "www-trycopilot-ai", "relationship": "routes-to"},
            "another-service",
        ]
        result = get_depends_on(deps)
        self.assertEqual(len(result), 3)
        self.assertIn("database-service", result)
        self.assertIn("www-trycopilot-ai", result)
        self.assertIn("another-service", result)

    def test_dependency_order_preserved(self):
        """Dependencies should maintain order in resulting dict"""
        deps = ["service-a", "service-b", "service-c"]
        result = get_depends_on(deps)
        keys = list(result.keys())
        self.assertEqual(keys, ["service-a", "service-b", "service-c"])


class TestServiceCommandEdgeCases(unittest.TestCase):
    """Additional edge case tests for get_service_command()"""

    def test_service_with_only_version(self):
        """Service with VERSION but not COMMIT"""
        service = {"network": {"port": 8080}, "config": ["VERSION"]}
        cmd = get_service_command("www-trycopilot-ai", service)
        self.assertIn("--version=${VERSION}", cmd)
        self.assertNotIn("--commit=", cmd)

    def test_unknown_service_default(self):
        """Unknown service types get default /main command"""
        service = {"network": {"port": 8080}}
        cmd = get_service_command("unknown-service", service)
        self.assertEqual(cmd, ["/main"])

    def test_custom_port(self):
        """Custom port should be used in command"""
        service = {"network": {"port": 9000}}
        cmd = get_service_command("database-service", service)
        self.assertIn("--port=9000", cmd)


def run_tests():
    """Run all unit tests"""
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()

    # Add all test classes
    suite.addTests(loader.loadTestsFromTestCase(TestGetReplicas))
    suite.addTests(loader.loadTestsFromTestCase(TestUserID))
    suite.addTests(loader.loadTestsFromTestCase(TestNetworkConfig))
    suite.addTests(loader.loadTestsFromTestCase(TestMapStorageVolumes))
    suite.addTests(loader.loadTestsFromTestCase(TestServiceCommand))
    suite.addTests(loader.loadTestsFromTestCase(TestDependsOn))
    suite.addTests(loader.loadTestsFromTestCase(TestDirectoryHash))
    suite.addTests(loader.loadTestsFromTestCase(TestNgrokEntrypoint))
    suite.addTests(loader.loadTestsFromTestCase(TestDependsOnEdgeCases))
    suite.addTests(loader.loadTestsFromTestCase(TestServiceCommandEdgeCases))

    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)

    return 0 if result.wasSuccessful() else 1


if __name__ == "__main__":
    sys.exit(run_tests())
