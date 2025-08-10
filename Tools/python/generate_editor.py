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
    editor_name = f"{arg}"
    
    arg2 = sys.argv[2]
    editor_pannel = f"{arg2}"

    replacements = {
        "EDITOR_NAME": editor_name,
        "EDITOR_PANNEL": editor_pannel
    }

    # === Define base paths ===
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    editor_dir = os.path.join(root, "NullEditor", "src", "Editors")
    template_dir = os.path.join(root, "NullEditor", "Stub")

    # === Output file paths ===
    header_path = os.path.join(editor_dir, f"{editor_name}.h")
    cpp_path = os.path.join(editor_dir, f"{editor_name}.cpp")

    # === Generate files ===
    generate_file(os.path.join(template_dir, "EditorTemplate.h"), header_path, replacements)
    generate_file(os.path.join(template_dir, "EditorTemplate.cpp"), cpp_path, replacements)

    print(f"✅ Generated editor: {editor_name}")

if __name__ == "__main__":
    main()
