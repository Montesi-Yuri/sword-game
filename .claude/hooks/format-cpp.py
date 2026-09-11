import sys, json, subprocess, os

CPP_EXTENSIONS = {".h", ".hpp", ".cpp", ".cc", ".inl"}

try:
    data = json.load(sys.stdin)
except Exception:
    sys.exit(0)

file_path = data.get("tool_input", {}).get("file_path", "") or data.get("tool_response", {}).get("filePath", "")

if os.path.splitext(file_path)[1].lower() in CPP_EXTENSIONS and os.path.isfile(file_path):
    try:
        subprocess.run(["clang-format", "-i", file_path])
    except Exception:
        pass

sys.exit(0)
