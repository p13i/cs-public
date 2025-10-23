import sys
import os
import json
import glob
import re
import subprocess
import shutil

from typing import List, Dict, Union
from os.path import join, exists, isfile, dirname

REPO_DIR = None
OUT_DIR = None

ACL_JSON = "acl.json"

ANY = "*"

VERBOSE = False


def DLOG(*args):
    if VERBOSE:
        print(*args)

def clean_directory_except_git(directory_path):
    """
    Removes all files and subdirectories within a given directory,
    except for the .git folder.

    Args:
        directory_path (str): The path to the directory to clean.
    """
    if not os.path.isdir(directory_path):
        print(f"Error: '{directory_path}' is not a valid directory.")
        return

    for item in os.listdir(directory_path):
        item_path = os.path.join(directory_path, item)
        if item == ".git":
            print(f"Skipping .git folder: {item_path}")
            continue  # Skip the .git folder
        
        if os.path.isfile(item_path):
            os.remove(item_path)
            print(f"Removed file: {item_path}")
        elif os.path.isdir(item_path):
            shutil.rmtree(item_path)
            print(f"Removed directory: {item_path}")

def CheckACLs(
    acls: List[Dict[str, Union[List[str], bool]]],
    principal: str = ANY,
    resource: str = ANY,
) -> bool:
    return any(
        (
            policy["allow"]
            and (ANY in policy["principals"] or principal in policy["principals"])
            and (ANY in policy["resources"] or resource in policy["resources"])
        )
        for policy in acls["policies"]
    )


def WalkACLs(repo_dir: str, principal: str = ANY, resource: str = ANY) -> bool:
    original_resource = resource
    base_acl_path = join(repo_dir, ACL_JSON)
    curr_dir = join(repo_dir, dirname(resource))
    curr_acl_path = join(repo_dir, curr_dir, ACL_JSON)
    while curr_acl_path != base_acl_path:
        if not exists(curr_acl_path) or not isfile(curr_acl_path):
            DLOG(
                f"No ACL file found at {curr_acl_path}, returning False for CheckACL(principal={principal}, resource={resource})"
            )
            return False
        DLOG(f"Loading ACL at {curr_acl_path}...")
        with open(curr_acl_path, mode="r") as f:
            acls = json.load(f)
        # All subsequent runs on parent directories must check against that directory in the adjacent ACL, not the current resource.
        if not CheckACLs(acls, principal, resource):
            DLOG(
                f"ACL explicitly DENIES {resource} with original_resource={original_resource}"
            )
            return False
        # Iterate up
        curr_dir = dirname(curr_dir)
        DLOG(f"Iterating up to {curr_dir}")
        resource = dirname(resource)
        curr_acl_path = join(repo_dir, curr_dir, ACL_JSON)
    DLOG(f"ACL granted at all levels for principal={principal}, resource={resource}")
    return True


def main(argv: List[str]) -> None:
    if len(argv) != 3:
        print(f"Usage: {argv[0]} <REPO_DIR> <OUT_DIR>")
        return

    global REPO_DIR
    REPO_DIR = argv[1]
    global OUT_DIR
    OUT_DIR = argv[2]

    clean_directory_except_git(OUT_DIR)

    principal = ANY
    resource = ANY
    Allow = lambda r: True  # "renderer/sphere_renderer.cc" in r
    for filename in subprocess.check_output(
        "git ls-files", shell=True, cwd=REPO_DIR
    ).splitlines():
        resource = filename.decode("utf-8")
        if not Allow(resource):
            continue
        if not WalkACLs(REPO_DIR, principal, resource):
            DLOG(f"Walking ACLs failed for {principal} and {resource}")
            continue

        print(f"Copying resource {resource} to OUT_DIR={OUT_DIR}.")
        repo_path = join(REPO_DIR, resource)
        out_path = join(OUT_DIR, resource)
        os.makedirs(dirname(out_path), exist_ok=True)
        with open(repo_path, mode="rb") as src, open(out_path, mode="wb") as dst:
            dst.write(src.read())
            subprocess.check_output(
                "git add " + resource, shell=True, cwd=OUT_DIR
            )

if __name__ == "__main__":
    main(sys.argv)
