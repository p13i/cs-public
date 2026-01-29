load("@rules_cc//cc:defs.bzl", "cc_library")

def _collect_files_from_labels(labels):
    files = []
    for l in labels:
        info = l[DefaultInfo]
        files.extend(info.files.to_list())

    # Dedup by path (keep last)
    seen = {}
    for f in files:
        seen[f.path] = f
    return [seen[k] for k in sorted(seen.keys())]

def _derive_symbol_suffix(label_name):
    """Strip _gen then _embedded from label name (e.g. index_embedded_gen -> index)."""
    s = label_name
    if s.endswith("_gen"):
        s = s[:-4]
    if s.endswith("_embedded"):
        s = s[:-9]
    return s

def _embed_files_gen_impl(ctx):
    files = _collect_files_from_labels(ctx.attr.srcs)
    if not files:
        fail("embed_files_gen: no files collected from srcs")

    out_cc = ctx.actions.declare_file(ctx.label.name + "_embed.cc")
    out_h = ctx.actions.declare_file(ctx.label.name + "_embed.h")

    suffix = ctx.attr.symbol_suffix or _derive_symbol_suffix(ctx.label.name)

    args = [
        "--prefix",
        ctx.attr.prefix,
        "--symbol-suffix",
        suffix,
        "--out_cc",
        out_cc.path,
        "--out_h",
        out_h.path,
    ]

    # Use basenames as logical names; the Python tool will sanitize.
    for f in files:
        args.extend(["--file", f.basename, f.path])

    ctx.actions.run(
        inputs = files + [ctx.executable._tool],
        outputs = [out_cc, out_h],
        executable = ctx.executable._tool,
        arguments = args,
        progress_message = "Embedding {} file(s): {}".format(
            len(files),
            ", ".join([f.basename for f in files]),
        ),
    )

    return [DefaultInfo(files = depset([out_cc, out_h]))]

embed_files_gen = rule(
    implementation = _embed_files_gen_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = True),
        "prefix": attr.string(default = "embedded"),
        "symbol_suffix": attr.string(default = ""),
        "_tool": attr.label(
            default = Label("//cs/devtools:embed_files"),
            executable = True,
            cfg = "exec",
        ),
    },
    outputs = {
        "src": "%{name}_embed.cc",
        "hdr": "%{name}_embed.h",
    },
)

def embed_files_cc(name, srcs, prefix = "embedded", symbol_suffix = None, visibility = None):
    """Macro: generates cc_library exposing embedded files.

    Produces:
      - <name>_gen: embed_files_gen (emits .cc/.h)
      - <name>:     cc_library with those sources
    """
    gen_name = name + "_gen"
    suffix = symbol_suffix
    if suffix == None:
        base = name[:-9] if name.endswith("_embedded") else name
        suffix = base
    embed_files_gen(
        name = gen_name,
        srcs = srcs,
        prefix = prefix,
        symbol_suffix = suffix,
    )

    cc_library(
        name = name,
        srcs = [":{}_embed.cc".format(gen_name)],
        hdrs = [":{}_embed.h".format(gen_name)],
        include_prefix = "",  # header is included as "<name>_gen_embed.h"
        strip_include_prefix = "",
        visibility = visibility,
    )
