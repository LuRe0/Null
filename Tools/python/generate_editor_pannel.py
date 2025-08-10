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


def main():
    # if len(sys.argv) not in [2, 3]:
    #     print("Usage: python generate_editor.py [ManagerName]")
    #     return

    arg = sys.argv[1]
    pannel_name = f"{arg}"
    
    arg2 = sys.argv[2]
    pannel_type = f"{arg2}"

    replacements = {
        "PANNEL_NAME": pannel_name,
        "PANNEL_TYPE": pannel_type
    }

    # === Define base paths ===
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    editor_dir = os.path.join(root, "NullEditor", "src", "Pannels")
    template_dir = os.path.join(root, "NullEditor", "Stub")

    # === Output file paths ===
    header_path = os.path.join(editor_dir, f"{pannel_name}Pannel.h")
    cpp_path = os.path.join(editor_dir, f"{pannel_name}Pannel.cpp")

    # === Generate files ===
    generate_file(os.path.join(template_dir, "PannelTemplate.h"), header_path, replacements)
    generate_file(os.path.join(template_dir, "PannelTemplate.cpp"), cpp_path, replacements)

    print(f"✅ Generated Pannel: {pannel_name}")

if __name__ == "__main__":
    main()
