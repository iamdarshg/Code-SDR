import datetime
import subprocess
import os

def get_git_hash():
    try:
        return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode('ascii').strip()
    except:
        return "unknown"

def generate_version():
    version_major = 2
    version_minor = 1
    version_patch = 0
    git_hash = get_git_hash()
    timestamp_str = datetime.datetime.now().strftime("%Y%m%d")
    timestamp_int = int(timestamp_str)

    version_int = (version_major << 24) | (version_minor << 16) | (version_patch << 8)

    # Verilog version
    with open('verilog/version_auto.v', 'w') as f:
        f.write(f"// Auto-generated version file\n")
        f.write(f"// Generated at {datetime.datetime.now().isoformat()}\n")
        f.write(f"localparam [31:0] VERSION = 32'h{version_int:08x};\n")
        f.write(f"localparam [31:0] BUILD_DATE = 32'd{timestamp_int};\n")

    # Python version
    with open('wideband-sdr-software/version_auto.py', 'w') as f:
        f.write(f"# Auto-generated version file\n")
        f.write(f"# Generated at {datetime.datetime.now().isoformat()}\n")
        f.write(f"VERSION_MAJOR = {version_major}\n")
        f.write(f"VERSION_MINOR = {version_minor}\n")
        f.write(f"VERSION_PATCH = {version_patch}\n")
        f.write(f"VERSION_INT = 0x{version_int:08x}\n")
        f.write(f"GIT_HASH = '{git_hash}'\n")
        f.write(f"BUILD_DATE = {timestamp_int}\n")

if __name__ == "__main__":
    generate_version()
