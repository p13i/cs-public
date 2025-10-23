resolved = [
    {
        "original_rule_class": "local_repository",
        "original_attributes": {
            "name": "bazel_tools",
            "path": "/home/pramo/.cache/bazel/_bazel_pramo/install/5309d864f9edb3a2e8380ffc84e6b95c/embedded_tools",
        },
        "native": 'local_repository(name = "bazel_tools", path = __embedded_dir__ + "/" + "embedded_tools")',
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:git.bzl%git_repository",
        "definition_information": "Repository emsdk instantiated at:\n  /home/pramo/cs/WORKSPACE:10:15: in <toplevel>\nRepository rule git_repository defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/git.bzl:189:33: in <toplevel>\n",
        "original_attributes": {
            "name": "emsdk",
            "remote": "https://github.com/emscripten-core/emsdk.git",
            "commit": "a896e3d066448b3530dbcaa48869fafefd738f57",
            "strip_prefix": "bazel",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:git.bzl%git_repository",
                "attributes": {
                    "remote": "https://github.com/emscripten-core/emsdk.git",
                    "commit": "a896e3d066448b3530dbcaa48869fafefd738f57",
                    "shallow_since": "",
                    "init_submodules": False,
                    "recursive_init_submodules": False,
                    "verbose": False,
                    "strip_prefix": "bazel",
                    "patches": [],
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": "",
                    "workspace_file_content": "",
                    "name": "emsdk",
                },
                "output_tree_hash": "0ecae4fcc3e7b765cf08f477b2069e90b3850dfa97686b5daaca8d8b8aa9590b",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository build_bazel_rules_nodejs instantiated at:\n  /home/pramo/cs/WORKSPACE:19:11: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/emsdk/deps.bzl:20:10: in deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "build_bazel_rules_nodejs",
            "generator_name": "build_bazel_rules_nodejs",
            "generator_function": "deps",
            "generator_location": None,
            "urls": [
                "https://github.com/bazelbuild/rules_nodejs/releases/download/5.8.0/rules_nodejs-5.8.0.tar.gz"
            ],
            "sha256": "dcc55f810142b6cf46a44d0180a5a7fb923c04a5061e2e8d8eb05ccccc60864b",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "",
                    "urls": [
                        "https://github.com/bazelbuild/rules_nodejs/releases/download/5.8.0/rules_nodejs-5.8.0.tar.gz"
                    ],
                    "sha256": "dcc55f810142b6cf46a44d0180a5a7fb923c04a5061e2e8d8eb05ccccc60864b",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": "",
                    "workspace_file_content": "",
                    "name": "build_bazel_rules_nodejs",
                },
                "output_tree_hash": "4fce8710c98c58b5a5a93fe39b255582275afb65bc61b951e92143cbed5c6967",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository rules_nodejs instantiated at:\n  /home/pramo/cs/WORKSPACE:19:11: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/emsdk/deps.bzl:14:10: in deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "rules_nodejs",
            "generator_name": "rules_nodejs",
            "generator_function": "deps",
            "generator_location": None,
            "urls": [
                "https://github.com/bazelbuild/rules_nodejs/releases/download/5.8.0/rules_nodejs-core-5.8.0.tar.gz"
            ],
            "sha256": "08337d4fffc78f7fe648a93be12ea2fc4e8eb9795a4e6aa48595b66b34555626",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "",
                    "urls": [
                        "https://github.com/bazelbuild/rules_nodejs/releases/download/5.8.0/rules_nodejs-core-5.8.0.tar.gz"
                    ],
                    "sha256": "08337d4fffc78f7fe648a93be12ea2fc4e8eb9795a4e6aa48595b66b34555626",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": "",
                    "workspace_file_content": "",
                    "name": "rules_nodejs",
                },
                "output_tree_hash": "3faf2c9166d97cb3f33bdb66cae8df7dfececf1edf6558e3d745aac56dc1ddb8",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository bazel_skylib instantiated at:\n  /home/pramo/cs/WORKSPACE:19:11: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/emsdk/deps.bzl:5:10: in deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "bazel_skylib",
            "generator_name": "bazel_skylib",
            "generator_function": "deps",
            "generator_location": None,
            "urls": [
                "https://github.com/bazelbuild/bazel-skylib/releases/download/1.1.1/bazel-skylib-1.1.1.tar.gz",
                "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.1.1/bazel-skylib-1.1.1.tar.gz",
            ],
            "sha256": "c6966ec828da198c5d9adbaa94c05e3a1c7f21bd012a0b29ba8ddbccb2c93b0d",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "",
                    "urls": [
                        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.1.1/bazel-skylib-1.1.1.tar.gz",
                        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.1.1/bazel-skylib-1.1.1.tar.gz",
                    ],
                    "sha256": "c6966ec828da198c5d9adbaa94c05e3a1c7f21bd012a0b29ba8ddbccb2c93b0d",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": "",
                    "workspace_file_content": "",
                    "name": "bazel_skylib",
                },
                "output_tree_hash": "ec8087f03267ba09d29db54ca9f5a2227132cd78f8797f746c927d0fef549ac5",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository rules_rust instantiated at:\n  /home/pramo/cs/WORKSPACE:33:13: in <toplevel>\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "rules_rust",
            "urls": [
                "https://github.com/bazelbuild/rules_rust/releases/download/0.48.0/rules_rust-v0.48.0.tar.gz"
            ],
            "integrity": "sha256-Weev1uz2QztBlDA88JX6A1N72SucD1V8lBsaliM0TTg=",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "",
                    "urls": [
                        "https://github.com/bazelbuild/rules_rust/releases/download/0.48.0/rules_rust-v0.48.0.tar.gz"
                    ],
                    "sha256": "",
                    "integrity": "sha256-Weev1uz2QztBlDA88JX6A1N72SucD1V8lBsaliM0TTg=",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": "",
                    "workspace_file_content": "",
                    "name": "rules_rust",
                },
                "output_tree_hash": "523530e824f527c97242017154ec9ad5cd157e92ff275b92ed94a1a707ca3089",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository rules_python instantiated at:\n  /home/pramo/cs/WORKSPACE:51:13: in <toplevel>\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "rules_python",
            "url": "https://github.com/bazel-contrib/rules_python/releases/download/0.36.0/rules_python-0.36.0.tar.gz",
            "sha256": "ca77768989a7f311186a29747e3e95c936a41dffac779aff6b443db22290d913",
            "strip_prefix": "rules_python-0.36.0",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://github.com/bazel-contrib/rules_python/releases/download/0.36.0/rules_python-0.36.0.tar.gz",
                    "urls": [],
                    "sha256": "ca77768989a7f311186a29747e3e95c936a41dffac779aff6b443db22290d913",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "rules_python-0.36.0",
                    "add_prefix": "",
                    "type": "",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": "",
                    "workspace_file_content": "",
                    "name": "rules_python",
                },
                "output_tree_hash": "3d84636f828f9e26bcac5769172d9e8735e0d56dc37aa7cbff9b344a4179c4cf",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_python//python/private:internal_config_repo.bzl%internal_config_repo",
        "definition_information": "Repository rules_python_internal instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:31:10: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule internal_config_repo defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/internal_config_repo.bzl:93:39: in <toplevel>\n",
        "original_attributes": {
            "name": "rules_python_internal",
            "generator_name": "rules_python_internal",
            "generator_function": "py_repositories",
            "generator_location": None,
        },
        "repositories": [
            {
                "rule_class": "@@rules_python//python/private:internal_config_repo.bzl%internal_config_repo",
                "attributes": {
                    "name": "rules_python_internal",
                    "generator_name": "rules_python_internal",
                    "generator_function": "py_repositories",
                    "generator_location": None,
                },
                "output_tree_hash": "89a89f7c588b9474a014ffb55b9cd4c26804473e3ec0af8cb08769c376372b30",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository rules_cc instantiated at:\n  /home/pramo/cs/WORKSPACE:41:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:60:10: in rules_rust_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "rules_cc",
            "generator_name": "rules_cc",
            "generator_function": "rules_rust_dependencies",
            "generator_location": None,
            "urls": [
                "https://github.com/bazelbuild/rules_cc/releases/download/0.0.9/rules_cc-0.0.9.tar.gz"
            ],
            "sha256": "2037875b9a4456dce4a79d112a8ae885bbc4aad968e6587dca6e64f3a0900cdf",
            "strip_prefix": "rules_cc-0.0.9",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "",
                    "urls": [
                        "https://github.com/bazelbuild/rules_cc/releases/download/0.0.9/rules_cc-0.0.9.tar.gz"
                    ],
                    "sha256": "2037875b9a4456dce4a79d112a8ae885bbc4aad968e6587dca6e64f3a0900cdf",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "rules_cc-0.0.9",
                    "add_prefix": "",
                    "type": "",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": "",
                    "workspace_file_content": "",
                    "name": "rules_cc",
                },
                "output_tree_hash": "eefc332fe980e25b58e7a4bd9fccd9e1a06dcb06f81423ce66248b4f1e7f8f74",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_python//python/private/pypi:pip_repository.bzl%pip_repository",
        "definition_information": "Repository multiset_deps instantiated at:\n  /home/pramo/cs/WORKSPACE:72:10: in <toplevel>\nRepository rule pip_repository defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/pip_repository.bzl:222:33: in <toplevel>\n",
        "original_attributes": {
            "name": "multiset_deps",
            "requirements_lock": "//cs/q/multiset:requirements.txt",
        },
        "repositories": [
            {
                "rule_class": "@@rules_python//python/private/pypi:pip_repository.bzl%pip_repository",
                "attributes": {
                    "name": "multiset_deps",
                    "requirements_lock": "//cs/q/multiset:requirements.txt",
                },
                "output_tree_hash": "066cbc258c46cc310515d4182b7adf6391a6ec9fdca381353394b73880c58572",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:local.bzl%local_repository",
        "definition_information": "Repository rules_java_builtin instantiated at:\n  /DEFAULT.WORKSPACE:12:36: in <toplevel>\nRepository rule local_repository defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/local.bzl:64:35: in <toplevel>\n",
        "original_attributes": {
            "name": "rules_java_builtin",
            "path": "/home/pramo/.cache/bazel/_bazel_pramo/install/5309d864f9edb3a2e8380ffc84e6b95c/rules_java",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:local.bzl%local_repository",
                "attributes": {
                    "name": "rules_java_builtin",
                    "path": "/home/pramo/.cache/bazel/_bazel_pramo/install/5309d864f9edb3a2e8380ffc84e6b95c/rules_java",
                },
                "output_tree_hash": "23156af102e8441d4b3e5358092fc1dce333786289d48b1df6503ecb8c735cf3",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:local.bzl%local_repository",
        "definition_information": "Repository internal_platforms_do_not_use instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:153:6: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule local_repository defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/local.bzl:64:35: in <toplevel>\n",
        "original_attributes": {
            "name": "internal_platforms_do_not_use",
            "generator_name": "internal_platforms_do_not_use",
            "generator_function": "maybe",
            "generator_location": None,
            "path": "/home/pramo/.cache/bazel/_bazel_pramo/install/5309d864f9edb3a2e8380ffc84e6b95c/platforms",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:local.bzl%local_repository",
                "attributes": {
                    "name": "internal_platforms_do_not_use",
                    "generator_name": "internal_platforms_do_not_use",
                    "generator_function": "maybe",
                    "generator_location": None,
                    "path": "/home/pramo/.cache/bazel/_bazel_pramo/install/5309d864f9edb3a2e8380ffc84e6b95c/platforms",
                },
                "output_tree_hash": "db797f5ddb49595460e727f2c71af1b3adfed4d65132bbe31bd9d3a06bd95dba",
            }
        ],
    },
    {
        "original_rule_class": "@@internal_platforms_do_not_use//host:extension.bzl%host_platform_repo",
        "definition_information": "Repository host_platform instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:165:6: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule host_platform_repo defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/internal_platforms_do_not_use/host/extension.bzl:51:37: in <toplevel>\n",
        "original_attributes": {
            "name": "host_platform",
            "generator_name": "host_platform",
            "generator_function": "maybe",
            "generator_location": None,
        },
        "repositories": [
            {
                "rule_class": "@@internal_platforms_do_not_use//host:extension.bzl%host_platform_repo",
                "attributes": {
                    "name": "host_platform",
                    "generator_name": "host_platform",
                    "generator_function": "maybe",
                    "generator_location": None,
                },
                "output_tree_hash": "7bb7732a410e479305fb8602fbfbe14a04e932eed9f8384852c03def646e87d5",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository platforms instantiated at:\n  /home/pramo/cs/WORKSPACE:41:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:51:10: in rules_rust_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "platforms",
            "generator_name": "platforms",
            "generator_function": "rules_rust_dependencies",
            "generator_location": None,
            "urls": [
                "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.8/platforms-0.0.8.tar.gz",
                "https://github.com/bazelbuild/platforms/releases/download/0.0.8/platforms-0.0.8.tar.gz",
            ],
            "sha256": "8150406605389ececb6da07cbcb509d5637a3ab9a24bc69b1101531367d89d74",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "",
                    "urls": [
                        "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.8/platforms-0.0.8.tar.gz",
                        "https://github.com/bazelbuild/platforms/releases/download/0.0.8/platforms-0.0.8.tar.gz",
                    ],
                    "sha256": "8150406605389ececb6da07cbcb509d5637a3ab9a24bc69b1101531367d89d74",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": "",
                    "workspace_file_content": "",
                    "name": "platforms",
                },
                "output_tree_hash": "28f7677a8a23ca0375359bd258c37723a79c09ba218477c816fc327a0be3552f",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_nodejs//nodejs/private:toolchains_repo.bzl%toolchains_repo",
        "definition_information": "Repository nodejs_toolchains instantiated at:\n  /home/pramo/cs/WORKSPACE:23:22: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/emsdk/emscripten_deps.bzl:116:26: in emscripten_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/build_bazel_rules_nodejs/internal/node/node_repositories.bzl:50:31: in node_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_nodejs/nodejs/repositories.bzl:417:20: in nodejs_register_toolchains\nRepository rule toolchains_repo defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_nodejs/nodejs/private/toolchains_repo.bzl:127:34: in <toplevel>\n",
        "original_attributes": {
            "name": "nodejs_toolchains",
            "generator_name": "nodejs_toolchains",
            "generator_function": "emscripten_deps",
            "generator_location": None,
            "user_node_repository_name": "nodejs",
        },
        "repositories": [
            {
                "rule_class": "@@rules_nodejs//nodejs/private:toolchains_repo.bzl%toolchains_repo",
                "attributes": {
                    "name": "nodejs_toolchains",
                    "generator_name": "nodejs_toolchains",
                    "generator_function": "emscripten_deps",
                    "generator_location": None,
                    "user_node_repository_name": "nodejs",
                },
                "output_tree_hash": "9d1cdbde62627f38d6066fcb7c51f3d02c685662707e1e80e1471d6a6150a8cc",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_freebsd_x86_64__wasm32-unknown-unknown__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_freebsd_x86_64__wasm32-unknown-unknown__stable",
            "generator_name": "rust_freebsd_x86_64__wasm32-unknown-unknown__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:freebsd",
            ],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:none"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_freebsd_x86_64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_freebsd_x86_64__wasm32-unknown-unknown__stable",
                    "generator_name": "rust_freebsd_x86_64__wasm32-unknown-unknown__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:freebsd",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:none",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_freebsd_x86_64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "dbe2f256519f0c761d438b5a0412fd2231d1f0d886656233058be03c8dbc3966",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remote_jdk8_windows_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:370:22: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:349:34: in remote_jdk8_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remote_jdk8_windows_toolchain_config_repo",
            "generator_name": "remote_jdk8_windows_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_windows//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_windows//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remote_jdk8_windows_toolchain_config_repo",
                    "generator_name": "remote_jdk8_windows_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_windows//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_windows//:jdk",\n)\n',
                },
                "output_tree_hash": "8d0b08c18f215c185d64efe72054a5ffef36325906c34ebf1d3c710d4ba5c685",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_linux_x86_64__wasm32-unknown-unknown__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_linux_x86_64__wasm32-unknown-unknown__stable",
            "generator_name": "rust_linux_x86_64__wasm32-unknown-unknown__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:x86_64", "@platforms//os:linux"],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:none"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_linux_x86_64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_linux_x86_64__wasm32-unknown-unknown__stable",
                    "generator_name": "rust_linux_x86_64__wasm32-unknown-unknown__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:linux",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:none",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_linux_x86_64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "7d3342a4df39d55e72b610b5792fcb984c37ed023e97317205b4e918f85aeddf",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rustfmt_1.79.0__aarch64-unknown-linux-gnu instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:258:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:925:31: in rustfmt_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rustfmt_1.79.0__aarch64-unknown-linux-gnu",
            "generator_name": "rustfmt_1.79.0__aarch64-unknown-linux-gnu",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:aarch64", "@platforms//os:linux"],
            "toolchain": "@rustfmt_1.79.0__aarch64-unknown-linux-gnu_tools//:rustfmt_toolchain",
            "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rustfmt_1.79.0__aarch64-unknown-linux-gnu",
                    "generator_name": "rustfmt_1.79.0__aarch64-unknown-linux-gnu",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:linux",
                    ],
                    "toolchain": "@rustfmt_1.79.0__aarch64-unknown-linux-gnu_tools//:rustfmt_toolchain",
                    "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
                },
                "output_tree_hash": "27de2e5f8bf466010d8d5998daaabccc87de568d83eb451be9cbbad01d415fd9",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_windows_x86_64__wasm32-unknown-unknown__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_windows_x86_64__wasm32-unknown-unknown__stable",
            "generator_name": "rust_windows_x86_64__wasm32-unknown-unknown__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:windows",
            ],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:none"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_windows_x86_64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_windows_x86_64__wasm32-unknown-unknown__stable",
                    "generator_name": "rust_windows_x86_64__wasm32-unknown-unknown__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:windows",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:none",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_windows_x86_64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "e96916ee129fce7c11ab84da7b17e52ad0a8ece23e5240ce505f501213693ee2",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_darwin_aarch64__wasm32-unknown-unknown__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_darwin_aarch64__wasm32-unknown-unknown__stable",
            "generator_name": "rust_darwin_aarch64__wasm32-unknown-unknown__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:aarch64", "@platforms//os:osx"],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:none"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_darwin_aarch64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_darwin_aarch64__wasm32-unknown-unknown__stable",
                    "generator_name": "rust_darwin_aarch64__wasm32-unknown-unknown__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:osx",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:none",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_darwin_aarch64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "46213f0b32dff6fa137f9f37075a51fe6e61d16d42ee532848ac1325e123038a",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rustfmt_1.79.0__aarch64-apple-darwin instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:258:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:925:31: in rustfmt_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rustfmt_1.79.0__aarch64-apple-darwin",
            "generator_name": "rustfmt_1.79.0__aarch64-apple-darwin",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:aarch64", "@platforms//os:osx"],
            "toolchain": "@rustfmt_1.79.0__aarch64-apple-darwin_tools//:rustfmt_toolchain",
            "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rustfmt_1.79.0__aarch64-apple-darwin",
                    "generator_name": "rustfmt_1.79.0__aarch64-apple-darwin",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:osx",
                    ],
                    "toolchain": "@rustfmt_1.79.0__aarch64-apple-darwin_tools//:rustfmt_toolchain",
                    "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
                },
                "output_tree_hash": "ed4bdd7bba7e6e2bd4b57e937d267cce118f005ca3ac0f316d0fd92e387b7442",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rustfmt_1.79.0__x86_64-unknown-linux-gnu instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:258:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:925:31: in rustfmt_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rustfmt_1.79.0__x86_64-unknown-linux-gnu",
            "generator_name": "rustfmt_1.79.0__x86_64-unknown-linux-gnu",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:x86_64", "@platforms//os:linux"],
            "toolchain": "@rustfmt_1.79.0__x86_64-unknown-linux-gnu_tools//:rustfmt_toolchain",
            "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rustfmt_1.79.0__x86_64-unknown-linux-gnu",
                    "generator_name": "rustfmt_1.79.0__x86_64-unknown-linux-gnu",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:linux",
                    ],
                    "toolchain": "@rustfmt_1.79.0__x86_64-unknown-linux-gnu_tools//:rustfmt_toolchain",
                    "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
                },
                "output_tree_hash": "42b59e9c9cc897f35794793cfbc96eb12245dc638051afeef15e98cde42e60f8",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_linux_x86_64__wasm32-wasi__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_linux_x86_64__wasm32-wasi__stable",
            "generator_name": "rust_linux_x86_64__wasm32-wasi__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:x86_64", "@platforms//os:linux"],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:wasi"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_linux_x86_64__wasm32-wasi__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_linux_x86_64__wasm32-wasi__stable",
                    "generator_name": "rust_linux_x86_64__wasm32-wasi__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:linux",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:wasi",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_linux_x86_64__wasm32-wasi__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "c5882086c4de904189ba28c249438c68a90990336ef92a237f3b1d24881ac002",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk11_win_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:371:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:353:34: in remote_jdk11_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk11_win_toolchain_config_repo",
            "generator_name": "remotejdk11_win_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_win//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_win//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk11_win_toolchain_config_repo",
                    "generator_name": "remotejdk11_win_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_win//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_win//:jdk",\n)\n',
                },
                "output_tree_hash": "d0587a4ecc9323d5cf65314b2d284b520ffb5ee1d3231cc6601efa13dadcc0f4",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk11_macos_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:371:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:353:34: in remote_jdk11_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk11_macos_toolchain_config_repo",
            "generator_name": "remotejdk11_macos_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_macos//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_macos//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk11_macos_toolchain_config_repo",
                    "generator_name": "remotejdk11_macos_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_macos//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_macos//:jdk",\n)\n',
                },
                "output_tree_hash": "45b3b36d22d3e614745e7a5e838351c32fe0eabb09a4a197bac0f4d416a950ce",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk11_linux_ppc64le_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:371:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:353:34: in remote_jdk11_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk11_linux_ppc64le_toolchain_config_repo",
            "generator_name": "remotejdk11_linux_ppc64le_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_ppc64le//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_ppc64le//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk11_linux_ppc64le_toolchain_config_repo",
                    "generator_name": "remotejdk11_linux_ppc64le_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_ppc64le//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_ppc64le//:jdk",\n)\n',
                },
                "output_tree_hash": "3272b586976beea589d09ea8029fd5d714da40127c8850e3480991c2440c5825",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk11_linux_s390x_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:371:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:353:34: in remote_jdk11_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk11_linux_s390x_toolchain_config_repo",
            "generator_name": "remotejdk11_linux_s390x_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_s390x//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_s390x//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk11_linux_s390x_toolchain_config_repo",
                    "generator_name": "remotejdk11_linux_s390x_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_s390x//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_s390x//:jdk",\n)\n',
                },
                "output_tree_hash": "244e11245106a8495ac4744a90023b87008e3e553766ba11d47a9fe5b4bb408d",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remote_jdk8_macos_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:370:22: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:349:34: in remote_jdk8_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remote_jdk8_macos_toolchain_config_repo",
            "generator_name": "remote_jdk8_macos_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_macos//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_macos//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remote_jdk8_macos_toolchain_config_repo",
                    "generator_name": "remote_jdk8_macos_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_macos//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_macos//:jdk",\n)\n',
                },
                "output_tree_hash": "e0d82dc2dbe8ec49d859811afe4973ec36226875a39ac7fc8419e91e7e9c89fb",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk11_win_arm64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:371:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:353:34: in remote_jdk11_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk11_win_arm64_toolchain_config_repo",
            "generator_name": "remotejdk11_win_arm64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_win_arm64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_win_arm64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk11_win_arm64_toolchain_config_repo",
                    "generator_name": "remotejdk11_win_arm64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_win_arm64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_win_arm64//:jdk",\n)\n',
                },
                "output_tree_hash": "c237bd9668de9b6437c452c020ea5bc717ff80b1a5ffd581adfdc7d4a6c5fe03",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_windows_x86_64__x86_64-pc-windows-msvc__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_windows_x86_64__x86_64-pc-windows-msvc__stable",
            "generator_name": "rust_windows_x86_64__x86_64-pc-windows-msvc__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:windows",
            ],
            "target_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:windows",
            ],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_windows_x86_64__x86_64-pc-windows-msvc__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_windows_x86_64__x86_64-pc-windows-msvc__stable",
                    "generator_name": "rust_windows_x86_64__x86_64-pc-windows-msvc__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:windows",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:windows",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_windows_x86_64__x86_64-pc-windows-msvc__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "1b5be10bf235eb244a3a092805fc8106be3ace17d977950b07bf1290d4d118b6",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remote_jdk8_linux_aarch64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:370:22: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:349:34: in remote_jdk8_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remote_jdk8_linux_aarch64_toolchain_config_repo",
            "generator_name": "remote_jdk8_linux_aarch64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux_aarch64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remote_jdk8_linux_aarch64_toolchain_config_repo",
                    "generator_name": "remote_jdk8_linux_aarch64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux_aarch64//:jdk",\n)\n',
                },
                "output_tree_hash": "c9c795851cffbf2a808bfc7cccea597c3b3fef46cfefa084f7e9de7e90b65447",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rustfmt_1.79.0__x86_64-pc-windows-msvc instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:258:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:925:31: in rustfmt_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rustfmt_1.79.0__x86_64-pc-windows-msvc",
            "generator_name": "rustfmt_1.79.0__x86_64-pc-windows-msvc",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:windows",
            ],
            "toolchain": "@rustfmt_1.79.0__x86_64-pc-windows-msvc_tools//:rustfmt_toolchain",
            "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rustfmt_1.79.0__x86_64-pc-windows-msvc",
                    "generator_name": "rustfmt_1.79.0__x86_64-pc-windows-msvc",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:windows",
                    ],
                    "toolchain": "@rustfmt_1.79.0__x86_64-pc-windows-msvc_tools//:rustfmt_toolchain",
                    "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
                },
                "output_tree_hash": "dd76a979b16aceb7cecd02516f0afeb88d7eaba975278d6c73ca49b8e76384ee",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rustfmt_1.79.0__aarch64-pc-windows-msvc instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:258:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:925:31: in rustfmt_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rustfmt_1.79.0__aarch64-pc-windows-msvc",
            "generator_name": "rustfmt_1.79.0__aarch64-pc-windows-msvc",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:aarch64",
                "@platforms//os:windows",
            ],
            "toolchain": "@rustfmt_1.79.0__aarch64-pc-windows-msvc_tools//:rustfmt_toolchain",
            "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rustfmt_1.79.0__aarch64-pc-windows-msvc",
                    "generator_name": "rustfmt_1.79.0__aarch64-pc-windows-msvc",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:windows",
                    ],
                    "toolchain": "@rustfmt_1.79.0__aarch64-pc-windows-msvc_tools//:rustfmt_toolchain",
                    "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
                },
                "output_tree_hash": "c87ea15e0da3865ac24070beff8fe2f3a123c0a6f546ace2f878e77225c6be6a",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_windows_aarch64__aarch64-pc-windows-msvc__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_windows_aarch64__aarch64-pc-windows-msvc__stable",
            "generator_name": "rust_windows_aarch64__aarch64-pc-windows-msvc__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:aarch64",
                "@platforms//os:windows",
            ],
            "target_compatible_with": [
                "@platforms//cpu:aarch64",
                "@platforms//os:windows",
            ],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_windows_aarch64__aarch64-pc-windows-msvc__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_windows_aarch64__aarch64-pc-windows-msvc__stable",
                    "generator_name": "rust_windows_aarch64__aarch64-pc-windows-msvc__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:windows",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:windows",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_windows_aarch64__aarch64-pc-windows-msvc__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "ca084fd046926273b03189a6898cdf45f5fc94db23a7615e2681aa5ce063b8aa",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rustfmt_1.79.0__x86_64-unknown-freebsd instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:258:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:925:31: in rustfmt_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rustfmt_1.79.0__x86_64-unknown-freebsd",
            "generator_name": "rustfmt_1.79.0__x86_64-unknown-freebsd",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:freebsd",
            ],
            "toolchain": "@rustfmt_1.79.0__x86_64-unknown-freebsd_tools//:rustfmt_toolchain",
            "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rustfmt_1.79.0__x86_64-unknown-freebsd",
                    "generator_name": "rustfmt_1.79.0__x86_64-unknown-freebsd",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:freebsd",
                    ],
                    "toolchain": "@rustfmt_1.79.0__x86_64-unknown-freebsd_tools//:rustfmt_toolchain",
                    "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
                },
                "output_tree_hash": "34a50b62ae81c48271d4d735bae34c0edfb6c2e7b2411f6ac7183b3ef7470c3d",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_python//python/private:toolchains_repo.bzl%toolchains_repo",
        "definition_information": "Repository python_3_11_toolchains instantiated at:\n  /home/pramo/cs/WORKSPACE:64:27: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/python_register_toolchains.bzl:175:20: in python_register_toolchains\nRepository rule toolchains_repo defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/toolchains_repo.bzl:107:34: in <toplevel>\n",
        "original_attributes": {
            "name": "python_3_11_toolchains",
            "generator_name": "python_3_11_toolchains",
            "generator_function": "python_register_toolchains",
            "generator_location": None,
            "python_version": "3.11.9",
            "set_python_version_constraint": False,
            "user_repository_name": "python_3_11",
        },
        "repositories": [
            {
                "rule_class": "@@rules_python//python/private:toolchains_repo.bzl%toolchains_repo",
                "attributes": {
                    "name": "python_3_11_toolchains",
                    "generator_name": "python_3_11_toolchains",
                    "generator_function": "python_register_toolchains",
                    "generator_location": None,
                    "python_version": "3.11.9",
                    "set_python_version_constraint": False,
                    "user_repository_name": "python_3_11",
                },
                "output_tree_hash": "b17cd1a2861c8145c2f22292deb07439bd8b17d15ccde020e6f3d74b31948403",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_linux_aarch64__wasm32-wasi__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_linux_aarch64__wasm32-wasi__stable",
            "generator_name": "rust_linux_aarch64__wasm32-wasi__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:aarch64", "@platforms//os:linux"],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:wasi"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_linux_aarch64__wasm32-wasi__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_linux_aarch64__wasm32-wasi__stable",
                    "generator_name": "rust_linux_aarch64__wasm32-wasi__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:linux",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:wasi",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_linux_aarch64__wasm32-wasi__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "d50ac493425e7113b8e5125b167d79b5de6140f52c791a1a704f48b912a3f266",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk11_linux_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:371:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:353:34: in remote_jdk11_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk11_linux_toolchain_config_repo",
            "generator_name": "remotejdk11_linux_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk11_linux_toolchain_config_repo",
                    "generator_name": "remotejdk11_linux_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux//:jdk",\n)\n',
                },
                "output_tree_hash": "0a170bf4f31e6c4621aeb4d4ce4b75b808be2f3a63cb55dc8172c27707d299ab",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_freebsd_x86_64__wasm32-wasi__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_freebsd_x86_64__wasm32-wasi__stable",
            "generator_name": "rust_freebsd_x86_64__wasm32-wasi__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:freebsd",
            ],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:wasi"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_freebsd_x86_64__wasm32-wasi__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_freebsd_x86_64__wasm32-wasi__stable",
                    "generator_name": "rust_freebsd_x86_64__wasm32-wasi__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:freebsd",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:wasi",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_freebsd_x86_64__wasm32-wasi__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "6c990145ff1175d534f900f743c97be980c1bcdcf8a9237302383d4e4e7efadd",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remote_jdk8_linux_s390x_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:370:22: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:349:34: in remote_jdk8_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remote_jdk8_linux_s390x_toolchain_config_repo",
            "generator_name": "remote_jdk8_linux_s390x_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux_s390x//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux_s390x//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remote_jdk8_linux_s390x_toolchain_config_repo",
                    "generator_name": "remote_jdk8_linux_s390x_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux_s390x//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux_s390x//:jdk",\n)\n',
                },
                "output_tree_hash": "f1e3f0b4884e21863a7c19a3a12a8995ed4162e02bd07cbb61b42799fc2d7359",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_linux_x86_64__x86_64-unknown-linux-gnu__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_linux_x86_64__x86_64-unknown-linux-gnu__stable",
            "generator_name": "rust_linux_x86_64__x86_64-unknown-linux-gnu__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:x86_64", "@platforms//os:linux"],
            "target_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:linux",
            ],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_linux_x86_64__x86_64-unknown-linux-gnu__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_linux_x86_64__x86_64-unknown-linux-gnu__stable",
                    "generator_name": "rust_linux_x86_64__x86_64-unknown-linux-gnu__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:linux",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:linux",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_linux_x86_64__x86_64-unknown-linux-gnu__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "05bbdf6b149cbbd643ec0bea693f8c735cbc83cf9613e87197d1adbe691cb698",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rustfmt_1.79.0__x86_64-apple-darwin instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:258:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:925:31: in rustfmt_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rustfmt_1.79.0__x86_64-apple-darwin",
            "generator_name": "rustfmt_1.79.0__x86_64-apple-darwin",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:x86_64", "@platforms//os:osx"],
            "toolchain": "@rustfmt_1.79.0__x86_64-apple-darwin_tools//:rustfmt_toolchain",
            "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rustfmt_1.79.0__x86_64-apple-darwin",
                    "generator_name": "rustfmt_1.79.0__x86_64-apple-darwin",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:osx",
                    ],
                    "toolchain": "@rustfmt_1.79.0__x86_64-apple-darwin_tools//:rustfmt_toolchain",
                    "toolchain_type": "@rules_rust//rust/rustfmt:toolchain_type",
                },
                "output_tree_hash": "521a0e00d0fe45b56ae1ece4f27284a0d040f00ddf485d00f7511d9a6e178356",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk11_macos_aarch64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:371:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:353:34: in remote_jdk11_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk11_macos_aarch64_toolchain_config_repo",
            "generator_name": "remotejdk11_macos_aarch64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_macos_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_macos_aarch64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk11_macos_aarch64_toolchain_config_repo",
                    "generator_name": "remotejdk11_macos_aarch64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_macos_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_macos_aarch64//:jdk",\n)\n',
                },
                "output_tree_hash": "ca1d067909669aa58188026a7da06d43bdec74a3ba5c122af8a4c3660acd8d8f",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk17_win_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:372:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:357:34: in remote_jdk17_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk17_win_toolchain_config_repo",
            "generator_name": "remotejdk17_win_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_win//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_win//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk17_win_toolchain_config_repo",
                    "generator_name": "remotejdk17_win_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_win//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_win//:jdk",\n)\n',
                },
                "output_tree_hash": "170c3c9a35e502555dc9f04b345e064880acbf7df935f673154011356f4aad34",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk17_linux_s390x_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:372:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:357:34: in remote_jdk17_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk17_linux_s390x_toolchain_config_repo",
            "generator_name": "remotejdk17_linux_s390x_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_s390x//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_s390x//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk17_linux_s390x_toolchain_config_repo",
                    "generator_name": "remotejdk17_linux_s390x_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_s390x//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_s390x//:jdk",\n)\n',
                },
                "output_tree_hash": "6ba1870e09fccfdcd423f4169b966a73f8e9deaff859ec6fb3b626ed61ebd8b5",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk21_linux_aarch64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:373:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:361:34: in remote_jdk21_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk21_linux_aarch64_toolchain_config_repo",
            "generator_name": "remotejdk21_linux_aarch64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_aarch64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk21_linux_aarch64_toolchain_config_repo",
                    "generator_name": "remotejdk21_linux_aarch64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_aarch64//:jdk",\n)\n',
                },
                "output_tree_hash": "bb33021f243382d2fb849ec204c5c8be5083c37e081df71d34a84324687cf001",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk11_linux_aarch64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:371:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:353:34: in remote_jdk11_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk11_linux_aarch64_toolchain_config_repo",
            "generator_name": "remotejdk11_linux_aarch64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_aarch64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk11_linux_aarch64_toolchain_config_repo",
                    "generator_name": "remotejdk11_linux_aarch64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_11"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "11"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk11_linux_aarch64//:jdk",\n)\n',
                },
                "output_tree_hash": "bef508c068dd47d605f62c53ab0628f1f7f5101fdcc8ada09b2067b36c47931f",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_linux_aarch64__wasm32-unknown-unknown__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_linux_aarch64__wasm32-unknown-unknown__stable",
            "generator_name": "rust_linux_aarch64__wasm32-unknown-unknown__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:aarch64", "@platforms//os:linux"],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:none"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_linux_aarch64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_linux_aarch64__wasm32-unknown-unknown__stable",
                    "generator_name": "rust_linux_aarch64__wasm32-unknown-unknown__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:linux",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:none",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_linux_aarch64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "020bcc09e69e0a5967d65f87bbf04408f53f04e807535e7e04f072f702d3564f",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_darwin_aarch64__aarch64-apple-darwin__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_darwin_aarch64__aarch64-apple-darwin__stable",
            "generator_name": "rust_darwin_aarch64__aarch64-apple-darwin__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:aarch64", "@platforms//os:osx"],
            "target_compatible_with": ["@platforms//cpu:aarch64", "@platforms//os:osx"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_darwin_aarch64__aarch64-apple-darwin__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_darwin_aarch64__aarch64-apple-darwin__stable",
                    "generator_name": "rust_darwin_aarch64__aarch64-apple-darwin__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:osx",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:osx",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_darwin_aarch64__aarch64-apple-darwin__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "bab5bf9a446ff8881777de8fc151872c220e98ec3adf32e12670e3b2c0277755",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk21_linux_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:373:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:361:34: in remote_jdk21_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk21_linux_toolchain_config_repo",
            "generator_name": "remotejdk21_linux_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk21_linux_toolchain_config_repo",
                    "generator_name": "remotejdk21_linux_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux//:jdk",\n)\n',
                },
                "output_tree_hash": "ee548ad054c9b75286ff3cd19792e433a2d1236378d3a0d8076fca0bb1a88e05",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_windows_aarch64__wasm32-unknown-unknown__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_windows_aarch64__wasm32-unknown-unknown__stable",
            "generator_name": "rust_windows_aarch64__wasm32-unknown-unknown__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:aarch64",
                "@platforms//os:windows",
            ],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:none"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_windows_aarch64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_windows_aarch64__wasm32-unknown-unknown__stable",
                    "generator_name": "rust_windows_aarch64__wasm32-unknown-unknown__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:windows",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:none",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_windows_aarch64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "a4c6154326bb5d44e56c2917aabfac9ac723721b59880708ea571f1bac15f580",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_darwin_x86_64__wasm32-wasi__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_darwin_x86_64__wasm32-wasi__stable",
            "generator_name": "rust_darwin_x86_64__wasm32-wasi__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:x86_64", "@platforms//os:osx"],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:wasi"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_darwin_x86_64__wasm32-wasi__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_darwin_x86_64__wasm32-wasi__stable",
                    "generator_name": "rust_darwin_x86_64__wasm32-wasi__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:osx",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:wasi",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_darwin_x86_64__wasm32-wasi__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "12ec4d991ec32fbe542e955b7178babfaef2b07a1eef3a919a4b4c61e26ec84b",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remote_jdk8_linux_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:370:22: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:349:34: in remote_jdk8_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remote_jdk8_linux_toolchain_config_repo",
            "generator_name": "remote_jdk8_linux_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remote_jdk8_linux_toolchain_config_repo",
                    "generator_name": "remote_jdk8_linux_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_linux//:jdk",\n)\n',
                },
                "output_tree_hash": "b6a178fc0ca08a4473490f1c5d0f9f633db0ca0f2834c69dd08ce8290cf9ca86",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_linux_aarch64__aarch64-unknown-linux-gnu__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_linux_aarch64__aarch64-unknown-linux-gnu__stable",
            "generator_name": "rust_linux_aarch64__aarch64-unknown-linux-gnu__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:aarch64", "@platforms//os:linux"],
            "target_compatible_with": [
                "@platforms//cpu:aarch64",
                "@platforms//os:linux",
            ],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_linux_aarch64__aarch64-unknown-linux-gnu__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_linux_aarch64__aarch64-unknown-linux-gnu__stable",
                    "generator_name": "rust_linux_aarch64__aarch64-unknown-linux-gnu__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:linux",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:linux",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_linux_aarch64__aarch64-unknown-linux-gnu__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "6f8b49acd9bf587adc29e88f682283961efbe7ea494c6ec4ab35c0d925a03b3b",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_analyzer_1.79.0 instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:208:10: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:776:31: in rust_analyzer_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_analyzer_1.79.0",
            "generator_name": "rust_analyzer_1.79.0",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [],
            "target_compatible_with": [],
            "toolchain": "@rust_analyzer_1.79.0_tools//:rust_analyzer_toolchain",
            "toolchain_type": "@rules_rust//rust/rust_analyzer:toolchain_type",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_analyzer_1.79.0",
                    "generator_name": "rust_analyzer_1.79.0",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [],
                    "target_compatible_with": [],
                    "toolchain": "@rust_analyzer_1.79.0_tools//:rust_analyzer_toolchain",
                    "toolchain_type": "@rules_rust//rust/rust_analyzer:toolchain_type",
                },
                "output_tree_hash": "b798aaab29dc822708a53c0c1f0dcc85094bf21673f49de22b49b02f85111570",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk21_win_arm64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:373:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:361:34: in remote_jdk21_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk21_win_arm64_toolchain_config_repo",
            "generator_name": "remotejdk21_win_arm64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_win_arm64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_win_arm64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk21_win_arm64_toolchain_config_repo",
                    "generator_name": "remotejdk21_win_arm64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_win_arm64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_win_arm64//:jdk",\n)\n',
                },
                "output_tree_hash": "9bbdbb329eeba27bc482582360abc6e3351d9a9a07ee11cba3a0026c90223e85",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_windows_aarch64__wasm32-wasi__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_windows_aarch64__wasm32-wasi__stable",
            "generator_name": "rust_windows_aarch64__wasm32-wasi__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:aarch64",
                "@platforms//os:windows",
            ],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:wasi"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_windows_aarch64__wasm32-wasi__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_windows_aarch64__wasm32-wasi__stable",
                    "generator_name": "rust_windows_aarch64__wasm32-wasi__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:windows",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:wasi",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_windows_aarch64__wasm32-wasi__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "bf24fb0c9b2eeebc81bcf73ef958a4ded40f69d5f91a0c462c87a3699ba61164",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_darwin_x86_64__x86_64-apple-darwin__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_darwin_x86_64__x86_64-apple-darwin__stable",
            "generator_name": "rust_darwin_x86_64__x86_64-apple-darwin__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:x86_64", "@platforms//os:osx"],
            "target_compatible_with": ["@platforms//cpu:x86_64", "@platforms//os:osx"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_darwin_x86_64__x86_64-apple-darwin__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_darwin_x86_64__x86_64-apple-darwin__stable",
                    "generator_name": "rust_darwin_x86_64__x86_64-apple-darwin__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:osx",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:osx",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_darwin_x86_64__x86_64-apple-darwin__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "504124ae838e8a54860c4c43bff0a618aa6dd2699126f9a3e4947c208272092c",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_windows_x86_64__wasm32-wasi__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_windows_x86_64__wasm32-wasi__stable",
            "generator_name": "rust_windows_x86_64__wasm32-wasi__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:windows",
            ],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:wasi"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_windows_x86_64__wasm32-wasi__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_windows_x86_64__wasm32-wasi__stable",
                    "generator_name": "rust_windows_x86_64__wasm32-wasi__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:windows",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:wasi",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_windows_x86_64__wasm32-wasi__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "3e55930520d3077bb9262cfdda218c694222ebf5888512a6e8b6d02703d8c6f8",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_darwin_x86_64__wasm32-unknown-unknown__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_darwin_x86_64__wasm32-unknown-unknown__stable",
            "generator_name": "rust_darwin_x86_64__wasm32-unknown-unknown__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:x86_64", "@platforms//os:osx"],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:none"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_darwin_x86_64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_darwin_x86_64__wasm32-unknown-unknown__stable",
                    "generator_name": "rust_darwin_x86_64__wasm32-unknown-unknown__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:osx",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:none",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_darwin_x86_64__wasm32-unknown-unknown__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "964d86127a343e1edeb124c6c4df878c82ce68729e8dd9c6e990a90dcbc09e99",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk21_macos_aarch64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:373:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:361:34: in remote_jdk21_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk21_macos_aarch64_toolchain_config_repo",
            "generator_name": "remotejdk21_macos_aarch64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_macos_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_macos_aarch64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk21_macos_aarch64_toolchain_config_repo",
                    "generator_name": "remotejdk21_macos_aarch64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_macos_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_macos_aarch64//:jdk",\n)\n',
                },
                "output_tree_hash": "706d910cc6809ea7f77fa4f938a4f019dd90d9dad927fb804a14b04321300a36",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk21_linux_ppc64le_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:373:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:361:34: in remote_jdk21_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk21_linux_ppc64le_toolchain_config_repo",
            "generator_name": "remotejdk21_linux_ppc64le_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_ppc64le//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_ppc64le//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk21_linux_ppc64le_toolchain_config_repo",
                    "generator_name": "remotejdk21_linux_ppc64le_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_ppc64le//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_ppc64le//:jdk",\n)\n',
                },
                "output_tree_hash": "7886e497d586c3f3c8225685281b0940e9aa699af208dc98de3db8839e197be3",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk21_linux_s390x_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:373:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:361:34: in remote_jdk21_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk21_linux_s390x_toolchain_config_repo",
            "generator_name": "remotejdk21_linux_s390x_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_s390x//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_s390x//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk21_linux_s390x_toolchain_config_repo",
                    "generator_name": "remotejdk21_linux_s390x_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_s390x//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:s390x"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_linux_s390x//:jdk",\n)\n',
                },
                "output_tree_hash": "30b78e0951c37c2d7ae1318f83045ff42ef261dbb93c5b4fd3ba963e12cf68d6",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk21_win_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:373:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:361:34: in remote_jdk21_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk21_win_toolchain_config_repo",
            "generator_name": "remotejdk21_win_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_win//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_win//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk21_win_toolchain_config_repo",
                    "generator_name": "remotejdk21_win_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_win//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_win//:jdk",\n)\n',
                },
                "output_tree_hash": "87012328b07a779503deec0ef47132a0de50efd69afe7df87619bcc07b1dc4ed",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk21_macos_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:373:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:361:34: in remote_jdk21_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk21_macos_toolchain_config_repo",
            "generator_name": "remotejdk21_macos_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_macos//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_macos//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk21_macos_toolchain_config_repo",
                    "generator_name": "remotejdk21_macos_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_21"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "21"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk21_macos//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk21_macos//:jdk",\n)\n',
                },
                "output_tree_hash": "434446eddb7f6a3dcc7a2a5330ed9ab26579c5142c19866b197475a695fbb32f",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk17_linux_ppc64le_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:372:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:357:34: in remote_jdk17_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk17_linux_ppc64le_toolchain_config_repo",
            "generator_name": "remotejdk17_linux_ppc64le_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_ppc64le//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_ppc64le//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk17_linux_ppc64le_toolchain_config_repo",
                    "generator_name": "remotejdk17_linux_ppc64le_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_ppc64le//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:ppc"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_ppc64le//:jdk",\n)\n',
                },
                "output_tree_hash": "fdc8ae00f2436bfc46b2f54c84f2bd84122787ede232a4d61ffc284bfe6f61ec",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk17_macos_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:372:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:357:34: in remote_jdk17_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk17_macos_toolchain_config_repo",
            "generator_name": "remotejdk17_macos_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_macos//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_macos//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk17_macos_toolchain_config_repo",
                    "generator_name": "remotejdk17_macos_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_macos//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_macos//:jdk",\n)\n',
                },
                "output_tree_hash": "41aa7b3317f8d9001746e908454760bf544ffaa058abe22f40711246608022ba",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk17_linux_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:372:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:357:34: in remote_jdk17_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk17_linux_toolchain_config_repo",
            "generator_name": "remotejdk17_linux_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk17_linux_toolchain_config_repo",
                    "generator_name": "remotejdk17_linux_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:x86_64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux//:jdk",\n)\n',
                },
                "output_tree_hash": "f0f07fe0f645f2dc7b8c9953c7962627e1c7425cc52f543729dbff16cd20e461",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk17_linux_aarch64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:372:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:357:34: in remote_jdk17_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk17_linux_aarch64_toolchain_config_repo",
            "generator_name": "remotejdk17_linux_aarch64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_aarch64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk17_linux_aarch64_toolchain_config_repo",
                    "generator_name": "remotejdk17_linux_aarch64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:linux", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_linux_aarch64//:jdk",\n)\n',
                },
                "output_tree_hash": "b169b01ac1a169d7eb5e3525454c3e408e9127993ac0f578dc2c5ad183fd4e3e",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk17_win_arm64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:372:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:357:34: in remote_jdk17_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk17_win_arm64_toolchain_config_repo",
            "generator_name": "remotejdk17_win_arm64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_win_arm64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_win_arm64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk17_win_arm64_toolchain_config_repo",
                    "generator_name": "remotejdk17_win_arm64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_win_arm64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:windows", "@platforms//cpu:arm64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_win_arm64//:jdk",\n)\n',
                },
                "output_tree_hash": "86b129d9c464a9b08f97eca7d8bc5bdb3676b581f8aac044451dbdfaa49e69d3",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remotejdk17_macos_aarch64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:372:23: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:357:34: in remote_jdk17_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remotejdk17_macos_aarch64_toolchain_config_repo",
            "generator_name": "remotejdk17_macos_aarch64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_macos_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_macos_aarch64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remotejdk17_macos_aarch64_toolchain_config_repo",
                    "generator_name": "remotejdk17_macos_aarch64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_17"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "17"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remotejdk17_macos_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remotejdk17_macos_aarch64//:jdk",\n)\n',
                },
                "output_tree_hash": "0eb17f6d969bc665a21e55d29eb51e88a067159ee62cf5094b17658a07d3accb",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/sh:sh_configure.bzl%sh_config",
        "definition_information": "Repository local_config_sh instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:187:13: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/sh/sh_configure.bzl:83:14: in sh_configure\nRepository rule sh_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/sh/sh_configure.bzl:72:28: in <toplevel>\n",
        "original_attributes": {
            "name": "local_config_sh",
            "generator_name": "local_config_sh",
            "generator_function": "sh_configure",
            "generator_location": None,
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/sh:sh_configure.bzl%sh_config",
                "attributes": {
                    "name": "local_config_sh",
                    "generator_name": "local_config_sh",
                    "generator_function": "sh_configure",
                    "generator_location": None,
                },
                "output_tree_hash": "7bf5ba89669bcdf4526f821bc9f1f9f49409ae9c61c4e8f21c9f17e06c475127",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_darwin_aarch64__wasm32-wasi__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_darwin_aarch64__wasm32-wasi__stable",
            "generator_name": "rust_darwin_aarch64__wasm32-wasi__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": ["@platforms//cpu:aarch64", "@platforms//os:osx"],
            "target_compatible_with": ["@platforms//cpu:wasm32", "@platforms//os:wasi"],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_darwin_aarch64__wasm32-wasi__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_darwin_aarch64__wasm32-wasi__stable",
                    "generator_name": "rust_darwin_aarch64__wasm32-wasi__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:aarch64",
                        "@platforms//os:osx",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:wasm32",
                        "@platforms//os:wasi",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_darwin_aarch64__wasm32-wasi__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "0ae4424a3d167534491c353d6eb04c1437d8ecb324a05742e674e574a5720b59",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
        "definition_information": "Repository remote_jdk8_macos_aarch64_toolchain_config_repo instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:93:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:370:22: in rules_java_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:349:34: in remote_jdk8_repos\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/java/repositories.bzl:333:14: in _remote_jdk_repos_for_version\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:57:22: in remote_java_repository\nRepository rule _toolchain_config defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/remote_java_repository.bzl:27:36: in <toplevel>\n",
        "original_attributes": {
            "name": "remote_jdk8_macos_aarch64_toolchain_config_repo",
            "generator_name": "remote_jdk8_macos_aarch64_toolchain_config_repo",
            "generator_function": "rules_java_dependencies",
            "generator_location": None,
            "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_macos_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_macos_aarch64//:jdk",\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:remote_java_repository.bzl%_toolchain_config",
                "attributes": {
                    "name": "remote_jdk8_macos_aarch64_toolchain_config_repo",
                    "generator_name": "remote_jdk8_macos_aarch64_toolchain_config_repo",
                    "generator_function": "rules_java_dependencies",
                    "generator_location": None,
                    "build_file": '\nconfig_setting(\n    name = "prefix_version_setting",\n    values = {"java_runtime_version": "remotejdk_8"},\n    visibility = ["//visibility:private"],\n)\nconfig_setting(\n    name = "version_setting",\n    values = {"java_runtime_version": "8"},\n    visibility = ["//visibility:private"],\n)\nalias(\n    name = "version_or_prefix_version_setting",\n    actual = select({\n        ":version_setting": ":version_setting",\n        "//conditions:default": ":prefix_version_setting",\n    }),\n    visibility = ["//visibility:private"],\n)\ntoolchain(\n    name = "toolchain",\n    target_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:runtime_toolchain_type",\n    toolchain = "@remote_jdk8_macos_aarch64//:jdk",\n)\ntoolchain(\n    name = "bootstrap_runtime_toolchain",\n    # These constraints are not required for correctness, but prevent fetches of remote JDK for\n    # different architectures. As every Java compilation toolchain depends on a bootstrap runtime in\n    # the same configuration, this constraint will not result in toolchain resolution failures.\n    exec_compatible_with = ["@platforms//os:macos", "@platforms//cpu:aarch64"],\n    target_settings = [":version_or_prefix_version_setting"],\n    toolchain_type = "@bazel_tools//tools/jdk:bootstrap_runtime_toolchain_type",\n    toolchain = "@remote_jdk8_macos_aarch64//:jdk",\n)\n',
                },
                "output_tree_hash": "4d721d8b0731cfb50f963f8b55c7bef9f572de0e2f251f07a12c722ef1acbb2f",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
        "definition_information": "Repository rust_freebsd_x86_64__x86_64-unknown-freebsd__stable instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:640:31: in rust_toolchain_repository\nRepository rule toolchain_repository_proxy defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:514:45: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_freebsd_x86_64__x86_64-unknown-freebsd__stable",
            "generator_name": "rust_freebsd_x86_64__x86_64-unknown-freebsd__stable",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "exec_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:freebsd",
            ],
            "target_compatible_with": [
                "@platforms//cpu:x86_64",
                "@platforms//os:freebsd",
            ],
            "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
            "toolchain": "@rust_freebsd_x86_64__x86_64-unknown-freebsd__stable_tools//:rust_toolchain",
            "toolchain_type": "@rules_rust//rust:toolchain",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%toolchain_repository_proxy",
                "attributes": {
                    "name": "rust_freebsd_x86_64__x86_64-unknown-freebsd__stable",
                    "generator_name": "rust_freebsd_x86_64__x86_64-unknown-freebsd__stable",
                    "generator_function": "rust_register_toolchains",
                    "generator_location": None,
                    "exec_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:freebsd",
                    ],
                    "target_compatible_with": [
                        "@platforms//cpu:x86_64",
                        "@platforms//os:freebsd",
                    ],
                    "target_settings": ["@rules_rust//rust/toolchain/channel:stable"],
                    "toolchain": "@rust_freebsd_x86_64__x86_64-unknown-freebsd__stable_tools//:rust_toolchain",
                    "toolchain_type": "@rules_rust//rust:toolchain",
                },
                "output_tree_hash": "75dd987a2f82b067aabc71d5008192c7a84215d37ef617fd35400b73f0e42634",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/cpp:cc_configure.bzl%cc_autoconf_toolchains",
        "definition_information": "Repository local_config_cc_toolchains instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:181:13: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/cpp/cc_configure.bzl:148:27: in cc_configure\nRepository rule cc_autoconf_toolchains defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/cpp/cc_configure.bzl:47:41: in <toplevel>\n",
        "original_attributes": {
            "name": "local_config_cc_toolchains",
            "generator_name": "local_config_cc_toolchains",
            "generator_function": "cc_configure",
            "generator_location": None,
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/cpp:cc_configure.bzl%cc_autoconf_toolchains",
                "attributes": {
                    "name": "local_config_cc_toolchains",
                    "generator_name": "local_config_cc_toolchains",
                    "generator_function": "cc_configure",
                    "generator_location": None,
                },
                "output_tree_hash": "f95f3d84ac75b4a4d9817af803f1d998a755bd9c20c700c79fa82cb159e98cfc",
            }
        ],
    },
    {
        "original_rule_class": "local_config_platform",
        "original_attributes": {"name": "local_config_platform"},
        "native": "local_config_platform(name = 'local_config_platform')",
    },
    {
        "original_rule_class": "@@rules_java_builtin//toolchains:local_java_repository.bzl%_local_java_repository_rule",
        "definition_information": "Repository local_jdk instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:85:6: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/local_java_repository.bzl:335:32: in local_java_repository\nRepository rule _local_java_repository_rule defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_java_builtin/toolchains/local_java_repository.bzl:290:46: in <toplevel>\n",
        "original_attributes": {
            "name": "local_jdk",
            "generator_name": "local_jdk",
            "generator_function": "maybe",
            "generator_location": None,
            "build_file_content": 'load("@rules_java//java:defs.bzl", "java_runtime")\n\npackage(default_visibility = ["//visibility:public"])\n\nexports_files(["WORKSPACE", "BUILD.bazel"])\n\nfilegroup(\n    name = "jre",\n    srcs = glob(\n        [\n            "jre/bin/**",\n            "jre/lib/**",\n        ],\n        allow_empty = True,\n        # In some configurations, Java browser plugin is considered harmful and\n        # common antivirus software blocks access to npjp2.dll interfering with Bazel,\n        # so do not include it in JRE on Windows.\n        exclude = ["jre/bin/plugin2/**"],\n    ),\n)\n\nfilegroup(\n    name = "jdk-bin",\n    srcs = glob(\n        ["bin/**"],\n        # The JDK on Windows sometimes contains a directory called\n        # "%systemroot%", which is not a valid label.\n        exclude = ["**/*%*/**"],\n    ),\n)\n\n# This folder holds security policies.\nfilegroup(\n    name = "jdk-conf",\n    srcs = glob(\n        ["conf/**"],\n        allow_empty = True,\n    ),\n)\n\nfilegroup(\n    name = "jdk-include",\n    srcs = glob(\n        ["include/**"],\n        allow_empty = True,\n    ),\n)\n\nfilegroup(\n    name = "jdk-lib",\n    srcs = glob(\n        ["lib/**", "release"],\n        allow_empty = True,\n        exclude = [\n            "lib/missioncontrol/**",\n            "lib/visualvm/**",\n        ],\n    ),\n)\n\njava_runtime(\n    name = "jdk",\n    srcs = [\n        ":jdk-bin",\n        ":jdk-conf",\n        ":jdk-include",\n        ":jdk-lib",\n        ":jre",\n    ],\n    # Provide the \'java` binary explicitly so that the correct path is used by\n    # Bazel even when the host platform differs from the execution platform.\n    # Exactly one of the two globs will be empty depending on the host platform.\n    # When --incompatible_disallow_empty_glob is enabled, each individual empty\n    # glob will fail without allow_empty = True, even if the overall result is\n    # non-empty.\n    java = glob(["bin/java.exe", "bin/java"], allow_empty = True)[0],\n    version = {RUNTIME_VERSION},\n)\n\nfilegroup(\n    name = "jdk-jmods",\n    srcs = glob(\n        ["jmods/**"],\n        allow_empty = True,\n    ),\n)\n\njava_runtime(\n    name = "jdk-with-jmods",\n    srcs = [\n        ":jdk-bin",\n        ":jdk-conf",\n        ":jdk-include",\n        ":jdk-lib",\n        ":jdk-jmods",\n        ":jre",\n    ],\n    java = glob(["bin/java.exe", "bin/java"], allow_empty = True)[0],\n    version = {RUNTIME_VERSION},\n)\n',
            "java_home": "",
            "version": "",
        },
        "repositories": [
            {
                "rule_class": "@@rules_java_builtin//toolchains:local_java_repository.bzl%_local_java_repository_rule",
                "attributes": {
                    "name": "local_jdk",
                    "generator_name": "local_jdk",
                    "generator_function": "maybe",
                    "generator_location": None,
                    "build_file_content": 'load("@rules_java//java:defs.bzl", "java_runtime")\n\npackage(default_visibility = ["//visibility:public"])\n\nexports_files(["WORKSPACE", "BUILD.bazel"])\n\nfilegroup(\n    name = "jre",\n    srcs = glob(\n        [\n            "jre/bin/**",\n            "jre/lib/**",\n        ],\n        allow_empty = True,\n        # In some configurations, Java browser plugin is considered harmful and\n        # common antivirus software blocks access to npjp2.dll interfering with Bazel,\n        # so do not include it in JRE on Windows.\n        exclude = ["jre/bin/plugin2/**"],\n    ),\n)\n\nfilegroup(\n    name = "jdk-bin",\n    srcs = glob(\n        ["bin/**"],\n        # The JDK on Windows sometimes contains a directory called\n        # "%systemroot%", which is not a valid label.\n        exclude = ["**/*%*/**"],\n    ),\n)\n\n# This folder holds security policies.\nfilegroup(\n    name = "jdk-conf",\n    srcs = glob(\n        ["conf/**"],\n        allow_empty = True,\n    ),\n)\n\nfilegroup(\n    name = "jdk-include",\n    srcs = glob(\n        ["include/**"],\n        allow_empty = True,\n    ),\n)\n\nfilegroup(\n    name = "jdk-lib",\n    srcs = glob(\n        ["lib/**", "release"],\n        allow_empty = True,\n        exclude = [\n            "lib/missioncontrol/**",\n            "lib/visualvm/**",\n        ],\n    ),\n)\n\njava_runtime(\n    name = "jdk",\n    srcs = [\n        ":jdk-bin",\n        ":jdk-conf",\n        ":jdk-include",\n        ":jdk-lib",\n        ":jre",\n    ],\n    # Provide the \'java` binary explicitly so that the correct path is used by\n    # Bazel even when the host platform differs from the execution platform.\n    # Exactly one of the two globs will be empty depending on the host platform.\n    # When --incompatible_disallow_empty_glob is enabled, each individual empty\n    # glob will fail without allow_empty = True, even if the overall result is\n    # non-empty.\n    java = glob(["bin/java.exe", "bin/java"], allow_empty = True)[0],\n    version = {RUNTIME_VERSION},\n)\n\nfilegroup(\n    name = "jdk-jmods",\n    srcs = glob(\n        ["jmods/**"],\n        allow_empty = True,\n    ),\n)\n\njava_runtime(\n    name = "jdk-with-jmods",\n    srcs = [\n        ":jdk-bin",\n        ":jdk-conf",\n        ":jdk-include",\n        ":jdk-lib",\n        ":jdk-jmods",\n        ":jre",\n    ],\n    java = glob(["bin/java.exe", "bin/java"], allow_empty = True)[0],\n    version = {RUNTIME_VERSION},\n)\n',
                    "java_home": "",
                    "version": "",
                },
                "output_tree_hash": "c43122226ef45731ba3cd0cb4a8134cc890c54becb72588ee2bab2bcfb280b81",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_nodejs//nodejs/private:nodejs_repo_host_os_alias.bzl%nodejs_repo_host_os_alias",
        "definition_information": "Repository nodejs instantiated at:\n  /home/pramo/cs/WORKSPACE:23:22: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/emsdk/emscripten_deps.bzl:116:26: in emscripten_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/build_bazel_rules_nodejs/internal/node/node_repositories.bzl:50:31: in node_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_nodejs/nodejs/repositories.bzl:407:30: in nodejs_register_toolchains\nRepository rule nodejs_repo_host_os_alias defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_nodejs/nodejs/private/nodejs_repo_host_os_alias.bzl:44:44: in <toplevel>\n",
        "original_attributes": {
            "name": "nodejs",
            "generator_name": "nodejs",
            "generator_function": "emscripten_deps",
            "generator_location": None,
            "user_node_repository_name": "nodejs",
        },
        "repositories": [
            {
                "rule_class": "@@rules_nodejs//nodejs/private:nodejs_repo_host_os_alias.bzl%nodejs_repo_host_os_alias",
                "attributes": {
                    "name": "nodejs",
                    "generator_name": "nodejs",
                    "generator_function": "emscripten_deps",
                    "generator_location": None,
                    "user_node_repository_name": "nodejs",
                },
                "output_tree_hash": "dfcef2c94a6d5ac103acfb7fb9e5d939cb62badc32e61d9585d6dccf12275298",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository rules_rust_tinyjson instantiated at:\n  /home/pramo/cs/WORKSPACE:41:24: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:97:10: in rules_rust_dependencies\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "rules_rust_tinyjson",
            "generator_name": "rules_rust_tinyjson",
            "generator_function": "rules_rust_dependencies",
            "generator_location": None,
            "url": "https://static.crates.io/crates/tinyjson/tinyjson-2.5.1.crate",
            "sha256": "9ab95735ea2c8fd51154d01e39cf13912a78071c2d89abc49a7ef102a7dd725a",
            "strip_prefix": "tinyjson-2.5.1",
            "type": "tar.gz",
            "build_file": "@@rules_rust//util/process_wrapper:BUILD.tinyjson.bazel",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://static.crates.io/crates/tinyjson/tinyjson-2.5.1.crate",
                    "urls": [],
                    "sha256": "9ab95735ea2c8fd51154d01e39cf13912a78071c2d89abc49a7ef102a7dd725a",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "tinyjson-2.5.1",
                    "add_prefix": "",
                    "type": "tar.gz",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file": "@@rules_rust//util/process_wrapper:BUILD.tinyjson.bazel",
                    "build_file_content": "",
                    "workspace_file_content": "",
                    "name": "rules_rust_tinyjson",
                },
                "output_tree_hash": "b5e38aea8f8031862eeac76aba2a0182efd90e58d2fd6e222e5feeeff07cb330",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__build instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__build",
            "generator_name": "pypi__build",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/e2/03/f3c8ba0a6b6e30d7d18c40faab90807c9bb5e9a1e3b2fe2008af624a9c97/build-1.2.1-py3-none-any.whl",
            "sha256": "75e10f767a433d9a86e50d83f418e83efc18ede923ee5ff7df93b6cb0306c5d4",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/e2/03/f3c8ba0a6b6e30d7d18c40faab90807c9bb5e9a1e3b2fe2008af624a9c97/build-1.2.1-py3-none-any.whl",
                    "urls": [],
                    "sha256": "75e10f767a433d9a86e50d83f418e83efc18ede923ee5ff7df93b6cb0306c5d4",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__build",
                },
                "output_tree_hash": "50f91f9563a09712c00ef32dcd3a79c1cec80bbbc4e38d70d9e08fe5e2cd6f1e",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__click instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__click",
            "generator_name": "pypi__click",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/00/2e/d53fa4befbf2cfa713304affc7ca780ce4fc1fd8710527771b58311a3229/click-8.1.7-py3-none-any.whl",
            "sha256": "ae74fb96c20a0277a1d615f1e4d73c8414f5a98db8b799a7931d1582f3390c28",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/00/2e/d53fa4befbf2cfa713304affc7ca780ce4fc1fd8710527771b58311a3229/click-8.1.7-py3-none-any.whl",
                    "urls": [],
                    "sha256": "ae74fb96c20a0277a1d615f1e4d73c8414f5a98db8b799a7931d1582f3390c28",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__click",
                },
                "output_tree_hash": "14d99b4820cf65acb6a4872bcda0a7100b36daf16517a2ff4458631377d1d8eb",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__colorama instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__colorama",
            "generator_name": "pypi__colorama",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/d1/d6/3965ed04c63042e047cb6a3e6ed1a63a35087b6a609aa3a15ed8ac56c221/colorama-0.4.6-py2.py3-none-any.whl",
            "sha256": "4f1d9991f5acc0ca119f9d443620b77f9d6b33703e51011c16baf57afb285fc6",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/d1/d6/3965ed04c63042e047cb6a3e6ed1a63a35087b6a609aa3a15ed8ac56c221/colorama-0.4.6-py2.py3-none-any.whl",
                    "urls": [],
                    "sha256": "4f1d9991f5acc0ca119f9d443620b77f9d6b33703e51011c16baf57afb285fc6",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__colorama",
                },
                "output_tree_hash": "0a1eaaa1c058ea84e7866b9ff5a4b74c426ace9305c769f0a2449c13e1d360d8",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__importlib_metadata instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__importlib_metadata",
            "generator_name": "pypi__importlib_metadata",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/2d/0a/679461c511447ffaf176567d5c496d1de27cbe34a87df6677d7171b2fbd4/importlib_metadata-7.1.0-py3-none-any.whl",
            "sha256": "30962b96c0c223483ed6cc7280e7f0199feb01a0e40cfae4d4450fc6fab1f570",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/2d/0a/679461c511447ffaf176567d5c496d1de27cbe34a87df6677d7171b2fbd4/importlib_metadata-7.1.0-py3-none-any.whl",
                    "urls": [],
                    "sha256": "30962b96c0c223483ed6cc7280e7f0199feb01a0e40cfae4d4450fc6fab1f570",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__importlib_metadata",
                },
                "output_tree_hash": "54dbfde520b826edd9cfff04dd95eed5e86403099afe3915230a818d5c2baa81",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__installer instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__installer",
            "generator_name": "pypi__installer",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/e5/ca/1172b6638d52f2d6caa2dd262ec4c811ba59eee96d54a7701930726bce18/installer-0.7.0-py3-none-any.whl",
            "sha256": "05d1933f0a5ba7d8d6296bb6d5018e7c94fa473ceb10cf198a92ccea19c27b53",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/e5/ca/1172b6638d52f2d6caa2dd262ec4c811ba59eee96d54a7701930726bce18/installer-0.7.0-py3-none-any.whl",
                    "urls": [],
                    "sha256": "05d1933f0a5ba7d8d6296bb6d5018e7c94fa473ceb10cf198a92ccea19c27b53",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__installer",
                },
                "output_tree_hash": "72a21bbab0205585058806361db384b59e9494d37ea3f3fea6f9553ba78b9783",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__more_itertools instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__more_itertools",
            "generator_name": "pypi__more_itertools",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/50/e2/8e10e465ee3987bb7c9ab69efb91d867d93959095f4807db102d07995d94/more_itertools-10.2.0-py3-none-any.whl",
            "sha256": "686b06abe565edfab151cb8fd385a05651e1fdf8f0a14191e4439283421f8684",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/50/e2/8e10e465ee3987bb7c9ab69efb91d867d93959095f4807db102d07995d94/more_itertools-10.2.0-py3-none-any.whl",
                    "urls": [],
                    "sha256": "686b06abe565edfab151cb8fd385a05651e1fdf8f0a14191e4439283421f8684",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__more_itertools",
                },
                "output_tree_hash": "ea064146ac562c35cfab95ac639eb67e6f25a0910ddecdd9c0249c4b6a512abc",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__packaging instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__packaging",
            "generator_name": "pypi__packaging",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/49/df/1fceb2f8900f8639e278b056416d49134fb8d84c5942ffaa01ad34782422/packaging-24.0-py3-none-any.whl",
            "sha256": "2ddfb553fdf02fb784c234c7ba6ccc288296ceabec964ad2eae3777778130bc5",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/49/df/1fceb2f8900f8639e278b056416d49134fb8d84c5942ffaa01ad34782422/packaging-24.0-py3-none-any.whl",
                    "urls": [],
                    "sha256": "2ddfb553fdf02fb784c234c7ba6ccc288296ceabec964ad2eae3777778130bc5",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__packaging",
                },
                "output_tree_hash": "17c1de05a93eb51e70c2233be572d10bee09e863b366caf731311daa29a06384",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__pep517 instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__pep517",
            "generator_name": "pypi__pep517",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/25/6e/ca4a5434eb0e502210f591b97537d322546e4833dcb4d470a48c375c5540/pep517-0.13.1-py3-none-any.whl",
            "sha256": "31b206f67165b3536dd577c5c3f1518e8fbaf38cbc57efff8369a392feff1721",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/25/6e/ca4a5434eb0e502210f591b97537d322546e4833dcb4d470a48c375c5540/pep517-0.13.1-py3-none-any.whl",
                    "urls": [],
                    "sha256": "31b206f67165b3536dd577c5c3f1518e8fbaf38cbc57efff8369a392feff1721",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__pep517",
                },
                "output_tree_hash": "317618e9b4cbab12a0a4de5f5acf53f0bc9e08a3e84844032095561e339ebc27",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__pip instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__pip",
            "generator_name": "pypi__pip",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/8a/6a/19e9fe04fca059ccf770861c7d5721ab4c2aebc539889e97c7977528a53b/pip-24.0-py3-none-any.whl",
            "sha256": "ba0d021a166865d2265246961bec0152ff124de910c5cc39f1156ce3fa7c69dc",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/8a/6a/19e9fe04fca059ccf770861c7d5721ab4c2aebc539889e97c7977528a53b/pip-24.0-py3-none-any.whl",
                    "urls": [],
                    "sha256": "ba0d021a166865d2265246961bec0152ff124de910c5cc39f1156ce3fa7c69dc",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__pip",
                },
                "output_tree_hash": "20f6c2528daa34bb62e5fd33a511fffaea7783e70a6cf227e8d4814d35809c63",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/cpp:cc_configure.bzl%cc_autoconf",
        "definition_information": "Repository local_config_cc instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:181:13: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/cpp/cc_configure.bzl:149:16: in cc_configure\nRepository rule cc_autoconf defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/cpp/cc_configure.bzl:109:30: in <toplevel>\n",
        "original_attributes": {
            "name": "local_config_cc",
            "generator_name": "local_config_cc",
            "generator_function": "cc_configure",
            "generator_location": None,
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/cpp:cc_configure.bzl%cc_autoconf",
                "attributes": {
                    "name": "local_config_cc",
                    "generator_name": "local_config_cc",
                    "generator_function": "cc_configure",
                    "generator_location": None,
                },
                "output_tree_hash": "051f762e015ab6817363ad5dadc731abb0e5bd673ac739fca7036f82c262bbd7",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__pip_tools instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__pip_tools",
            "generator_name": "pypi__pip_tools",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/0d/dc/38f4ce065e92c66f058ea7a368a9c5de4e702272b479c0992059f7693941/pip_tools-7.4.1-py3-none-any.whl",
            "sha256": "4c690e5fbae2f21e87843e89c26191f0d9454f362d8acdbd695716493ec8b3a9",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/0d/dc/38f4ce065e92c66f058ea7a368a9c5de4e702272b479c0992059f7693941/pip_tools-7.4.1-py3-none-any.whl",
                    "urls": [],
                    "sha256": "4c690e5fbae2f21e87843e89c26191f0d9454f362d8acdbd695716493ec8b3a9",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__pip_tools",
                },
                "output_tree_hash": "64cf70b8895c0bcb69d88b01dfd3bba31f529d96ad0f330bfe662deed7b99dfe",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__pyproject_hooks instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__pyproject_hooks",
            "generator_name": "pypi__pyproject_hooks",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/ae/f3/431b9d5fe7d14af7a32340792ef43b8a714e7726f1d7b69cc4e8e7a3f1d7/pyproject_hooks-1.1.0-py3-none-any.whl",
            "sha256": "7ceeefe9aec63a1064c18d939bdc3adf2d8aa1988a510afec15151578b232aa2",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/ae/f3/431b9d5fe7d14af7a32340792ef43b8a714e7726f1d7b69cc4e8e7a3f1d7/pyproject_hooks-1.1.0-py3-none-any.whl",
                    "urls": [],
                    "sha256": "7ceeefe9aec63a1064c18d939bdc3adf2d8aa1988a510afec15151578b232aa2",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__pyproject_hooks",
                },
                "output_tree_hash": "4538dd5822635d546182c4741e5ce6df19628e06e0bea7a09025847131b4eb8a",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/osx:xcode_configure.bzl%xcode_autoconf",
        "definition_information": "Repository local_config_xcode instantiated at:\n  /DEFAULT.WORKSPACE.SUFFIX:184:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/osx/xcode_configure.bzl:312:19: in xcode_configure\nRepository rule xcode_autoconf defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/osx/xcode_configure.bzl:297:33: in <toplevel>\n",
        "original_attributes": {
            "name": "local_config_xcode",
            "generator_name": "local_config_xcode",
            "generator_function": "xcode_configure",
            "generator_location": None,
            "xcode_locator": "@bazel_tools//tools/osx:xcode_locator.m",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/osx:xcode_configure.bzl%xcode_autoconf",
                "attributes": {
                    "name": "local_config_xcode",
                    "generator_name": "local_config_xcode",
                    "generator_function": "xcode_configure",
                    "generator_location": None,
                    "xcode_locator": "@bazel_tools//tools/osx:xcode_locator.m",
                },
                "output_tree_hash": "ec026961157bb71cf68d1b568815ad68147ed16f318161bc0da40f6f3d7d79fd",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__setuptools instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__setuptools",
            "generator_name": "pypi__setuptools",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/de/88/70c5767a0e43eb4451c2200f07d042a4bcd7639276003a9c54a68cfcc1f8/setuptools-70.0.0-py3-none-any.whl",
            "sha256": "54faa7f2e8d2d11bcd2c07bed282eef1046b5c080d1c32add737d7b5817b1ad4",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/de/88/70c5767a0e43eb4451c2200f07d042a4bcd7639276003a9c54a68cfcc1f8/setuptools-70.0.0-py3-none-any.whl",
                    "urls": [],
                    "sha256": "54faa7f2e8d2d11bcd2c07bed282eef1046b5c080d1c32add737d7b5817b1ad4",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__setuptools",
                },
                "output_tree_hash": "011cda8258c7468fd132f90bc1ad0e812908317d958e2021765e27e6bcd881ef",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__tomli instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__tomli",
            "generator_name": "pypi__tomli",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/97/75/10a9ebee3fd790d20926a90a2547f0bf78f371b2f13aa822c759680ca7b9/tomli-2.0.1-py3-none-any.whl",
            "sha256": "939de3e7a6161af0c887ef91b7d41a53e7c5a1ca976325f429cb46ea9bc30ecc",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/97/75/10a9ebee3fd790d20926a90a2547f0bf78f371b2f13aa822c759680ca7b9/tomli-2.0.1-py3-none-any.whl",
                    "urls": [],
                    "sha256": "939de3e7a6161af0c887ef91b7d41a53e7c5a1ca976325f429cb46ea9bc30ecc",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__tomli",
                },
                "output_tree_hash": "f89d41b981419c535db607008944c95c6ec70fbe0c4c578fbf9b982e4d0f5561",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__wheel instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__wheel",
            "generator_name": "pypi__wheel",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/7d/cd/d7460c9a869b16c3dd4e1e403cce337df165368c71d6af229a74699622ce/wheel-0.43.0-py3-none-any.whl",
            "sha256": "55c570405f142630c6b9f72fe09d9b67cf1477fcf543ae5b8dcb1f5b7377da81",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/7d/cd/d7460c9a869b16c3dd4e1e403cce337df165368c71d6af229a74699622ce/wheel-0.43.0-py3-none-any.whl",
                    "urls": [],
                    "sha256": "55c570405f142630c6b9f72fe09d9b67cf1477fcf543ae5b8dcb1f5b7377da81",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__wheel",
                },
                "output_tree_hash": "b71f3a3f23df0924ba00c25d6f383f3c596c44459780b0a083ad27cc4a25a68f",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository pypi__zipp instantiated at:\n  /home/pramo/cs/WORKSPACE:62:16: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/py_repositories.bzl:49:14: in py_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/deps.bzl:133:14: in pypi_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "pypi__zipp",
            "generator_name": "pypi__zipp",
            "generator_function": "py_repositories",
            "generator_location": None,
            "url": "https://files.pythonhosted.org/packages/da/55/a03fd7240714916507e1fcf7ae355bd9d9ed2e6db492595f1a67f61681be/zipp-3.18.2-py3-none-any.whl",
            "sha256": "dce197b859eb796242b0622af1b8beb0a722d52aa2f57133ead08edd5bf5374e",
            "type": "zip",
            "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://files.pythonhosted.org/packages/da/55/a03fd7240714916507e1fcf7ae355bd9d9ed2e6db492595f1a67f61681be/zipp-3.18.2-py3-none-any.whl",
                    "urls": [],
                    "sha256": "dce197b859eb796242b0622af1b8beb0a722d52aa2f57133ead08edd5bf5374e",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "",
                    "add_prefix": "",
                    "type": "zip",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": 'package(default_visibility = ["//visibility:public"])\n\nload("@rules_python//python:defs.bzl", "py_library")\n\npy_library(\n    name = "lib",\n    srcs = glob(["**/*.py"]),\n    data = glob(["**/*"], exclude=[\n        # These entries include those put into user-installed dependencies by\n        # data_exclude to avoid non-determinism.\n        "**/*.py",\n        "**/*.pyc",\n        "**/*.pyc.*",  # During pyc creation, temp files named *.pyc.NNN are created\n        "**/* *",\n        "**/*.dist-info/RECORD",\n        "BUILD",\n        "WORKSPACE",\n    ]),\n    # This makes this directory a top-level in the python import\n    # search path for anything that depends on this.\n    imports = ["."],\n)\n',
                    "workspace_file_content": "",
                    "name": "pypi__zipp",
                },
                "output_tree_hash": "442888943147baa68f717665095652ceed83dc570a998d82b376ae80758cbcb7",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_python//python/private:python_repository.bzl%python_repository",
        "definition_information": "Repository python_3_11_x86_64-unknown-linux-gnu instantiated at:\n  /home/pramo/cs/WORKSPACE:64:27: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/python_register_toolchains.bzl:132:26: in python_register_toolchains\nRepository rule python_repository defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/python_repository.bzl:296:36: in <toplevel>\n",
        "original_attributes": {
            "name": "python_3_11_x86_64-unknown-linux-gnu",
            "generator_name": "python_3_11_x86_64-unknown-linux-gnu",
            "generator_function": "python_register_toolchains",
            "generator_location": None,
            "patches": [],
            "platform": "x86_64-unknown-linux-gnu",
            "python_version": "3.11.9",
            "release_filename": "20240726/cpython-3.11.9+20240726-x86_64-unknown-linux-gnu-install_only.tar.gz",
            "sha256": "f6e955dc9ddfcad74e77abe6f439dac48ebca14b101ed7c85a5bf3206ed2c53d",
            "strip_prefix": "python",
            "urls": [
                "https://github.com/indygreg/python-build-standalone/releases/download/20240726/cpython-3.11.9+20240726-x86_64-unknown-linux-gnu-install_only.tar.gz"
            ],
        },
        "repositories": [
            {
                "rule_class": "@@rules_python//python/private:python_repository.bzl%python_repository",
                "attributes": {
                    "auth_patterns": {},
                    "coverage_tool": "",
                    "distutils": None,
                    "distutils_content": "",
                    "ignore_root_user_error": False,
                    "name": "python_3_11_x86_64-unknown-linux-gnu",
                    "netrc": "",
                    "patch_strip": 1,
                    "patches": [],
                    "platform": "x86_64-unknown-linux-gnu",
                    "python_version": "3.11.9",
                    "release_filename": "20240726/cpython-3.11.9+20240726-x86_64-unknown-linux-gnu-install_only.tar.gz",
                    "sha256": "f6e955dc9ddfcad74e77abe6f439dac48ebca14b101ed7c85a5bf3206ed2c53d",
                    "strip_prefix": "python",
                    "urls": [
                        "https://github.com/indygreg/python-build-standalone/releases/download/20240726/cpython-3.11.9+20240726-x86_64-unknown-linux-gnu-install_only.tar.gz"
                    ],
                },
                "output_tree_hash": "5726b42c28a38ff8f095999c1eae46c4eded259cccf601b712dc81aac11eb08c",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:git.bzl%git_repository",
        "definition_information": "Repository googletest instantiated at:\n  /home/pramo/cs/WORKSPACE:3:19: in <toplevel>\nRepository rule git_repository defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/git.bzl:189:33: in <toplevel>\n",
        "original_attributes": {
            "name": "googletest",
            "remote": "https://github.com/google/googletest",
            "commit": "52eb8108c5bdec04579160ae17225d66034bd723",
            "build_file": "//:gmock.BUILD",
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:git.bzl%git_repository",
                "attributes": {
                    "remote": "https://github.com/google/googletest",
                    "commit": "52eb8108c5bdec04579160ae17225d66034bd723",
                    "shallow_since": "",
                    "init_submodules": False,
                    "recursive_init_submodules": False,
                    "verbose": False,
                    "strip_prefix": "",
                    "patches": [],
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file": "//:gmock.BUILD",
                    "build_file_content": "",
                    "workspace_file_content": "",
                    "name": "googletest",
                },
                "output_tree_hash": "989ce35bcdbb5ec95c00abb10a945a9d7759a518a20a8d9a911235d63d2bbdf7",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_python//python/private/pypi:whl_library.bzl%whl_library",
        "definition_information": "Repository multiset_deps_hypothesis instantiated at:\n  /home/pramo/cs/WORKSPACE:82:13: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/multiset_deps/requirements.bzl:104:20: in install_deps\nRepository rule whl_library defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/whl_library.bzl:453:30: in <toplevel>\n",
        "original_attributes": {
            "name": "multiset_deps_hypothesis",
            "generator_name": "multiset_deps_hypothesis",
            "generator_function": "install_deps",
            "generator_location": None,
            "group_deps": [],
            "repo": "multiset_deps",
            "repo_prefix": "multiset_deps_",
            "requirement": "hypothesis==6.112.0",
            "download_only": False,
            "enable_implicit_namespace_pkgs": False,
            "environment": {},
            "envsubst": [],
            "extra_pip_args": [],
            "isolated": True,
            "pip_data_exclude": [],
            "python_interpreter": "python3",
            "quiet": True,
            "timeout": 600,
        },
        "repositories": [
            {
                "rule_class": "@@rules_python//python/private/pypi:whl_library.bzl%whl_library",
                "attributes": {
                    "name": "multiset_deps_hypothesis",
                    "generator_name": "multiset_deps_hypothesis",
                    "generator_function": "install_deps",
                    "generator_location": None,
                    "group_deps": [],
                    "repo": "multiset_deps",
                    "repo_prefix": "multiset_deps_",
                    "requirement": "hypothesis==6.112.0",
                    "download_only": False,
                    "enable_implicit_namespace_pkgs": False,
                    "environment": {},
                    "envsubst": [],
                    "extra_pip_args": [],
                    "isolated": True,
                    "pip_data_exclude": [],
                    "python_interpreter": "python3",
                    "quiet": True,
                    "timeout": 600,
                },
                "output_tree_hash": "07eb36d9a838e7c489152160aa2c6558a91a64363d903b4bfdc85bc1d5b4c06d",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_nodejs//nodejs:repositories.bzl%node_repositories",
        "definition_information": "Repository nodejs_linux_amd64 instantiated at:\n  /home/pramo/cs/WORKSPACE:23:22: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/emsdk/emscripten_deps.bzl:116:26: in emscripten_deps\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/build_bazel_rules_nodejs/internal/node/node_repositories.bzl:50:31: in node_repositories\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_nodejs/nodejs/repositories.bzl:396:26: in nodejs_register_toolchains\nRepository rule node_repositories defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_nodejs/nodejs/repositories.bzl:370:36: in <toplevel>\n",
        "original_attributes": {
            "name": "nodejs_linux_amd64",
            "generator_name": "nodejs_linux_amd64",
            "generator_function": "emscripten_deps",
            "generator_location": None,
            "node_version": "16.6.2",
            "platform": "linux_amd64",
        },
        "repositories": [
            {
                "rule_class": "@@rules_nodejs//nodejs:repositories.bzl%node_repositories",
                "attributes": {
                    "name": "nodejs_linux_amd64",
                    "generator_name": "nodejs_linux_amd64",
                    "generator_function": "emscripten_deps",
                    "generator_location": None,
                    "node_version": "16.6.2",
                    "platform": "linux_amd64",
                },
                "output_tree_hash": "c760c4912a799857010109d6dfbfe3387260ea7fe8299da142873976e6dff1c2",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_python//python/private/pypi:whl_library.bzl%whl_library",
        "definition_information": "Repository multiset_deps_sortedcontainers instantiated at:\n  /home/pramo/cs/WORKSPACE:82:13: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/multiset_deps/requirements.bzl:104:20: in install_deps\nRepository rule whl_library defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/whl_library.bzl:453:30: in <toplevel>\n",
        "original_attributes": {
            "name": "multiset_deps_sortedcontainers",
            "generator_name": "multiset_deps_sortedcontainers",
            "generator_function": "install_deps",
            "generator_location": None,
            "group_deps": [],
            "repo": "multiset_deps",
            "repo_prefix": "multiset_deps_",
            "requirement": "sortedcontainers==2.4.0",
            "download_only": False,
            "enable_implicit_namespace_pkgs": False,
            "environment": {},
            "envsubst": [],
            "extra_pip_args": [],
            "isolated": True,
            "pip_data_exclude": [],
            "python_interpreter": "python3",
            "quiet": True,
            "timeout": 600,
        },
        "repositories": [
            {
                "rule_class": "@@rules_python//python/private/pypi:whl_library.bzl%whl_library",
                "attributes": {
                    "name": "multiset_deps_sortedcontainers",
                    "generator_name": "multiset_deps_sortedcontainers",
                    "generator_function": "install_deps",
                    "generator_location": None,
                    "group_deps": [],
                    "repo": "multiset_deps",
                    "repo_prefix": "multiset_deps_",
                    "requirement": "sortedcontainers==2.4.0",
                    "download_only": False,
                    "enable_implicit_namespace_pkgs": False,
                    "environment": {},
                    "envsubst": [],
                    "extra_pip_args": [],
                    "isolated": True,
                    "pip_data_exclude": [],
                    "python_interpreter": "python3",
                    "quiet": True,
                    "timeout": 600,
                },
                "output_tree_hash": "6e5d0874eebe94937d79685b368d8dc7533995dd64b7a206c93676cc43ba9a8b",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_python//python/private/pypi:whl_library.bzl%whl_library",
        "definition_information": "Repository multiset_deps_attrs instantiated at:\n  /home/pramo/cs/WORKSPACE:82:13: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/multiset_deps/requirements.bzl:104:20: in install_deps\nRepository rule whl_library defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_python/python/private/pypi/whl_library.bzl:453:30: in <toplevel>\n",
        "original_attributes": {
            "name": "multiset_deps_attrs",
            "generator_name": "multiset_deps_attrs",
            "generator_function": "install_deps",
            "generator_location": None,
            "group_deps": [],
            "repo": "multiset_deps",
            "repo_prefix": "multiset_deps_",
            "requirement": "attrs==23.2.0",
            "download_only": False,
            "enable_implicit_namespace_pkgs": False,
            "environment": {},
            "envsubst": [],
            "extra_pip_args": [],
            "isolated": True,
            "pip_data_exclude": [],
            "python_interpreter": "python3",
            "quiet": True,
            "timeout": 600,
        },
        "repositories": [
            {
                "rule_class": "@@rules_python//python/private/pypi:whl_library.bzl%whl_library",
                "attributes": {
                    "name": "multiset_deps_attrs",
                    "generator_name": "multiset_deps_attrs",
                    "generator_function": "install_deps",
                    "generator_location": None,
                    "group_deps": [],
                    "repo": "multiset_deps",
                    "repo_prefix": "multiset_deps_",
                    "requirement": "attrs==23.2.0",
                    "download_only": False,
                    "enable_implicit_namespace_pkgs": False,
                    "environment": {},
                    "envsubst": [],
                    "extra_pip_args": [],
                    "isolated": True,
                    "pip_data_exclude": [],
                    "python_interpreter": "python3",
                    "quiet": True,
                    "timeout": 600,
                },
                "output_tree_hash": "88e4797e1f3e0e7bbbd98a96e0e2a3da1a931b835fcb3c04d3e299c6cb76639f",
            }
        ],
    },
    {
        "original_rule_class": "@@rules_rust//rust:repositories.bzl%rust_toolchain_tools_repository",
        "definition_information": "Repository rust_linux_x86_64__x86_64-unknown-linux-gnu__stable_tools instantiated at:\n  /home/pramo/cs/WORKSPACE:43:25: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:236:14: in rust_register_toolchains\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/utils.bzl:268:18: in maybe\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:1100:61: in rust_repository_set\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:617:36: in rust_toolchain_repository\nRepository rule rust_toolchain_tools_repository defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/rules_rust/rust/repositories.bzl:487:50: in <toplevel>\n",
        "original_attributes": {
            "name": "rust_linux_x86_64__x86_64-unknown-linux-gnu__stable_tools",
            "generator_name": "rust_linux_x86_64__x86_64-unknown-linux-gnu__stable_tools",
            "generator_function": "rust_register_toolchains",
            "generator_location": None,
            "dev_components": False,
            "edition": "2021",
            "exec_triple": "x86_64-unknown-linux-gnu",
            "extra_rustc_flags": [],
            "rustfmt_version": "1.79.0",
            "target_triple": "x86_64-unknown-linux-gnu",
            "urls": ["https://static.rust-lang.org/dist/{}.tar.xz"],
            "version": "1.79.0",
        },
        "repositories": [
            {
                "rule_class": "@@rules_rust//rust:repositories.bzl%rust_toolchain_tools_repository",
                "attributes": {
                    "name": "rust_linux_x86_64__x86_64-unknown-linux-gnu__stable_tools",
                    "allocator_library": "@rules_rust//ffi/cc/allocator_library",
                    "auth": {},
                    "auth_patterns": [],
                    "dev_components": False,
                    "edition": "2021",
                    "exec_triple": "x86_64-unknown-linux-gnu",
                    "extra_exec_rustc_flags": [],
                    "extra_rustc_flags": [],
                    "global_allocator_library": "@rules_rust//ffi/cc/global_allocator_library",
                    "iso_date": "",
                    "netrc": "",
                    "opt_level": {},
                    "rustfmt_version": "1.79.0",
                    "sha256s": {},
                    "target_triple": "x86_64-unknown-linux-gnu",
                    "urls": ["https://static.rust-lang.org/dist/{}.tar.xz"],
                    "version": "1.79.0",
                },
                "output_tree_hash": "b1cce64d522862f3b5caab5b71afb03547c7e5602c4eb005b0369f0e6b009284",
            }
        ],
    },
    {
        "original_rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
        "definition_information": "Repository emscripten_bin_linux instantiated at:\n  /home/pramo/cs/WORKSPACE:23:22: in <toplevel>\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/emsdk/emscripten_deps.bzl:121:21: in emscripten_deps\nRepository rule http_archive defined at:\n  /home/pramo/.cache/bazel/_bazel_pramo/efcf6f74c271144ec05dd1219b75449a/external/bazel_tools/tools/build_defs/repo/http.bzl:387:31: in <toplevel>\n",
        "original_attributes": {
            "name": "emscripten_bin_linux",
            "generator_name": "emscripten_bin_linux",
            "generator_function": "emscripten_deps",
            "generator_location": None,
            "url": "https://storage.googleapis.com/webassembly/emscripten-releases-builds/linux/b90507fcf011da61bacfca613569d882f7749552/wasm-binaries.tbz2",
            "sha256": "5ffa2bab560a9cda6db6ee041a635d10e1ef26c8fc63675d682917b8d3d53263",
            "strip_prefix": "install",
            "type": "tar.bz2",
            "build_file_content": '\npackage(default_visibility = [\'//visibility:public\'])\n\nfilegroup(\n    name = "includes",\n    srcs = glob([\n        "emscripten/cache/sysroot/include/c++/v1/**",\n        "emscripten/cache/sysroot/include/compat/**",\n        "emscripten/cache/sysroot/include/**",\n        "lib/clang/17/include/**",\n    ]),\n)\n\nfilegroup(\n    name = "emcc_common",\n    srcs = [\n        "emscripten/emcc.py",\n        "emscripten/emscripten.py",\n        "emscripten/emscripten-version.txt",\n        "emscripten/cache/sysroot_install.stamp",\n        "emscripten/src/settings.js",\n        "emscripten/src/settings_internal.js",\n    ] + glob(\n        include = [\n            "emscripten/third_party/**",\n            "emscripten/tools/**",\n        ],\n        exclude = [\n            "**/__pycache__/**",\n        ],\n    ),\n)\n\nfilegroup(\n    name = "compiler_files",\n    srcs = [\n        "bin/clang",\n        "bin/clang++",\n        ":emcc_common",\n        ":includes",\n    ],\n)\n\nfilegroup(\n    name = "linker_files",\n    srcs = [\n        "bin/clang",\n        "bin/llvm-ar",\n        "bin/llvm-dwarfdump",\n        "bin/llvm-nm",\n        "bin/llvm-objcopy",\n        "bin/wasm-ctor-eval",\n        "bin/wasm-emscripten-finalize",\n        "bin/wasm-ld",\n        "bin/wasm-metadce",\n        "bin/wasm-opt",\n        "bin/wasm-split",\n        "bin/wasm2js",\n        ":emcc_common",\n    ] + glob(\n        include = [\n            "emscripten/cache/sysroot/lib/**",\n            "emscripten/node_modules/**",\n            "emscripten/src/**",\n        ],\n    ),\n)\n\nfilegroup(\n    name = "ar_files",\n    srcs = [\n        "bin/llvm-ar",\n        "emscripten/emar.py",\n        "emscripten/emscripten-version.txt",\n        "emscripten/src/settings.js",\n        "emscripten/src/settings_internal.js",\n    ] + glob(\n        include = [\n            "emscripten/tools/**",\n        ],\n        exclude = [\n            "**/__pycache__/**",\n        ],\n    ),\n)\n',
        },
        "repositories": [
            {
                "rule_class": "@@bazel_tools//tools/build_defs/repo:http.bzl%http_archive",
                "attributes": {
                    "url": "https://storage.googleapis.com/webassembly/emscripten-releases-builds/linux/b90507fcf011da61bacfca613569d882f7749552/wasm-binaries.tbz2",
                    "urls": [],
                    "sha256": "5ffa2bab560a9cda6db6ee041a635d10e1ef26c8fc63675d682917b8d3d53263",
                    "integrity": "",
                    "netrc": "",
                    "auth_patterns": {},
                    "canonical_id": "",
                    "strip_prefix": "install",
                    "add_prefix": "",
                    "type": "tar.bz2",
                    "patches": [],
                    "remote_file_urls": {},
                    "remote_file_integrity": {},
                    "remote_patches": {},
                    "remote_patch_strip": 0,
                    "patch_tool": "",
                    "patch_args": ["-p0"],
                    "patch_cmds": [],
                    "patch_cmds_win": [],
                    "build_file_content": '\npackage(default_visibility = [\'//visibility:public\'])\n\nfilegroup(\n    name = "includes",\n    srcs = glob([\n        "emscripten/cache/sysroot/include/c++/v1/**",\n        "emscripten/cache/sysroot/include/compat/**",\n        "emscripten/cache/sysroot/include/**",\n        "lib/clang/17/include/**",\n    ]),\n)\n\nfilegroup(\n    name = "emcc_common",\n    srcs = [\n        "emscripten/emcc.py",\n        "emscripten/emscripten.py",\n        "emscripten/emscripten-version.txt",\n        "emscripten/cache/sysroot_install.stamp",\n        "emscripten/src/settings.js",\n        "emscripten/src/settings_internal.js",\n    ] + glob(\n        include = [\n            "emscripten/third_party/**",\n            "emscripten/tools/**",\n        ],\n        exclude = [\n            "**/__pycache__/**",\n        ],\n    ),\n)\n\nfilegroup(\n    name = "compiler_files",\n    srcs = [\n        "bin/clang",\n        "bin/clang++",\n        ":emcc_common",\n        ":includes",\n    ],\n)\n\nfilegroup(\n    name = "linker_files",\n    srcs = [\n        "bin/clang",\n        "bin/llvm-ar",\n        "bin/llvm-dwarfdump",\n        "bin/llvm-nm",\n        "bin/llvm-objcopy",\n        "bin/wasm-ctor-eval",\n        "bin/wasm-emscripten-finalize",\n        "bin/wasm-ld",\n        "bin/wasm-metadce",\n        "bin/wasm-opt",\n        "bin/wasm-split",\n        "bin/wasm2js",\n        ":emcc_common",\n    ] + glob(\n        include = [\n            "emscripten/cache/sysroot/lib/**",\n            "emscripten/node_modules/**",\n            "emscripten/src/**",\n        ],\n    ),\n)\n\nfilegroup(\n    name = "ar_files",\n    srcs = [\n        "bin/llvm-ar",\n        "emscripten/emar.py",\n        "emscripten/emscripten-version.txt",\n        "emscripten/src/settings.js",\n        "emscripten/src/settings_internal.js",\n    ] + glob(\n        include = [\n            "emscripten/tools/**",\n        ],\n        exclude = [\n            "**/__pycache__/**",\n        ],\n    ),\n)\n',
                    "workspace_file_content": "",
                    "name": "emscripten_bin_linux",
                },
                "output_tree_hash": "eeaaa2e064824995de7fc07ce183e08f449e93e3e95ea45aa3dc4d0960e06c18",
            }
        ],
    },
]
