import os
import sys
import re
import json

def generate_file(template_path, output_path, replacements):
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    with open(template_path, "r") as f:
        content = f.read()
    
    for key, val in replacements.items():
        # Replace {{KEY}}, {{ KEY }}, {{    KEY    }}, etc.
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
        return  # Line already present

    if before_line:
        # Insert before a specific line
        for i, l in enumerate(lines):
            if before_line in l:
                lines.insert(i, line + '\n')
                break
        else:
            lines.append(line + '\n')  # fallback
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
        (i for i, m in enumerate(modules) if m.get("type") == "NRenderer"), 
        len(modules)
    )

    modules.insert(insert_index, {"type": module_name})
    data["modules"] = modules

    with open(json_path, "w") as f:
        json.dump(data, f, indent=4)
    
    print(f"Inserted {module_name} into Assets.json before NRenderer")


def main():
    if len(sys.argv) != 2:
        print("Usage: python generate_component.py ComponentName")
        return

    name = sys.argv[1]
    component_name = name + "Component"
    system_name = name + "System"

    replacements = {
        "COMPONENT_NAME": component_name,
        "SYSTEM_NAME": system_name
    }
    
    

    base_dir = os.path.join("NullEngine", "src", "Null", "Engine", "Submodules", "ECS")
    component_header = os.path.join(base_dir, "Components", f"{component_name}.h")
    system_header    = os.path.join(base_dir, "Systems", f"{system_name}.h")
    system_cpp       = os.path.join(base_dir, "Systems", f"{system_name}.cpp")

    template_base = os.path.join("NullEngine", "Stub", "Templates")
    generate_file(os.path.join(template_base, "ComponentTemplate.h"), component_header, replacements)
    generate_file(os.path.join(template_base, "SystemTemplate.h"), system_header, replacements)
    generate_file(os.path.join(template_base, "SystemTemplate.cpp"), system_cpp, replacements)
    
        # === File Paths ===
    nincludes_path = os.path.join("NullEngine", "src", "NIncludes.h")
    application_path = os.path.join("NullEngine", "src", "Null", "Application.cpp")

    # === Include for NIncludes.h ===
    relative_include_path = f"Null/Engine/Submodules/ECS/Components/{name}Component.h"
    append_once(nincludes_path, f'#include "{relative_include_path}"')

    relative_include_path = f"Null/Engine/Submodules/ECS/Systems/{name}System.h"
    append_once(nincludes_path, f'#include "{relative_include_path}"')

    # === AddCreateFunction in Application.cpp ===
    system_name = f"{name}System"
    add_func_line = f'\t\tAddCreateFunction<{system_name}>([&engine]() {{ engine.Add<NULLENGINE::{system_name}>(); }});'

    # Insert before some existing AddCreateFunction or fallback to end
    append_once(application_path, add_func_line, before_line="AddCreateFunction<NFramebufferManager>")

      # Add to Modules.json
    script_dir = os.path.dirname(os.path.abspath(__file__))
    modules_path = os.path.join(script_dir, "..", "..", "Assets", "Modules", "Modules.json")
    append_to_module_assets_json(system_name, modules_path)
    
    print(f"Generated files for {name}")

if __name__ == "__main__":
    main()
