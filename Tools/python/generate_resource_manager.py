import os
import sys
import re
import json

def generate_file(template_path, output_path, replacements):
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(template_path, "r") as f:
        content = f.read()
    for key, val in replacements.items():
        pattern = re.compile(r"\{\{\s*" + re.escape(key) + r"\s*\}\}")
        content = pattern.sub(val, content)
    with open(output_path, "w") as f:
        f.write(content)

def append_once(filepath, line, before_line=None):
    if not os.path.exists(filepath):
        with open(filepath, 'w') as f:
            f.write(line + '\n')
        return

    with open(filepath, 'r') as f:
        lines = f.readlines()

    if any(line.strip() == l.strip() for l in lines):
        return

    if before_line:
        for i, l in enumerate(lines):
            if before_line in l:
                lines.insert(i, line + '\n')
                break
        else:
            lines.append(line + '\n')
    else:
        lines.append(line + '\n')

    with open(filepath, 'w') as f:
        f.writelines(lines)

def append_to_module_assets_json(module_name, json_path):
    with open(json_path, "r") as f:
        data = json.load(f)

    modules = data.get("modules", [])
    if any(m.get("type") == module_name for m in modules):
        print(f"{module_name} already exists in Assets.json")
        return

    insert_index = next(
        (i for i, m in enumerate(modules) if m.get("type") == "NShaderManager"),
        len(modules)
    )
    modules.insert(insert_index, {"type": module_name})
    data["modules"] = modules

    with open(json_path, "w") as f:
        json.dump(data, f, indent=4)

    print(f"Inserted {module_name} into Assets.json before NShaderManager")

def main():
    if len(sys.argv) not in [2, 3]:
        print("Usage: python generate_resource_manager.py ResourceType [ManagerName]")
        return

    resource_type = sys.argv[1]
    manager_name = sys.argv[2] if len(sys.argv) == 3 else f"N{resource_type}Manager"
    resource_header = f"Null/Engine/Resources/{resource_type}.h"  # Change if incorrect

    replacements = {
        "RESOURCE_TYPE": resource_type,
        "MANAGER_NAME": manager_name,
        "RESOURCE_HEADER": resource_header
    }

    # === Define base paths ===
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    modules_dir = os.path.join(root, "NullEngine", "src", "Null", "Engine", "Modules")
    template_dir = os.path.join(root, "NullEngine", "Stub", "Templates")
    includes_path = os.path.join(root, "NullEngine", "src", "NIncludes.h")
    application_path = os.path.join(root, "NullEngine", "src", "Null", "Application.cpp")
    modules_json_path = os.path.join(root, "Assets", "Modules", "Modules.json")

    # === Output file paths ===
    header_path = os.path.join(modules_dir, f"{manager_name}.h")
    cpp_path = os.path.join(modules_dir, f"{manager_name}.cpp")

    # === Generate files ===
    generate_file(os.path.join(template_dir, "ResourceManagerTemplate.h"), header_path, replacements)
    generate_file(os.path.join(template_dir, "ResourceManagerTemplate.cpp"), cpp_path, replacements)

    # === Add includes ===
    relative_include_path = f"Null/Engine/Modules/{manager_name}.h"
    append_once(includes_path, f'#include "{relative_include_path}"')

    # === Register in Application.cpp ===
    add_func_line = f'\t\tAddCreateFunction<{manager_name}>([&engine]() {{ engine.Add<{manager_name}>(); }});'
    append_once(application_path, add_func_line, before_line="AddCreateFunction<NShaderManager>")

    # === Add to Modules.json ===
    append_to_module_assets_json(manager_name, modules_json_path)

    print(f"✅ Generated resource manager: {manager_name}")

if __name__ == "__main__":
    main()
