"""Bazel rules for generating routing.json from system.gpt.yml."""

def _routing_json_impl(ctx):
    """Implementation for routing_json rule that generates routing.json."""
    output = ctx.actions.declare_file(ctx.attr.out or "routing.json")

    # Get the system.gpt.yml file
    system_file = ctx.file.system_yml

    # Use the Python script as a tool
    script = ctx.executable._generate_script

    # Run the script
    ctx.actions.run(
        outputs = [output],
        inputs = [system_file],
        executable = script,
        arguments = [system_file.path, output.path],
    )

    return [DefaultInfo(files = depset([output]))]

routing_json = rule(
    implementation = _routing_json_impl,
    attrs = {
        "system_yml": attr.label(
            doc = "Path to system.gpt.yml file",
            allow_single_file = [".yml", ".yaml"],
            mandatory = True,
        ),
        "out": attr.string(
            doc = "Output filename (default: routing.json)",
            default = "routing.json",
        ),
        "_generate_script": attr.label(
            default = "//cs/devtools:generate_routing",
            executable = True,
            cfg = "exec",
        ),
    },
)
