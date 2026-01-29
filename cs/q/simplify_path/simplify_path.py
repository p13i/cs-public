#!/usr/bin/env python3
# cs/q/simplify_path/simplify_path.py
def SimplifyPath(s: str) -> str:
    stack = []
    for comp in s.split("/"):
        if comp == "" or comp == ".":
            continue
        if comp == "..":
            if stack:
                stack.pop()
        else:
            stack.append(comp)
    return "/" + "/".join(stack)
