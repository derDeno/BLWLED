import os
import shutil
import re

Import("env")

# This function extracts the VERSION string from src/config.h
def get_version():
    version_file = os.path.join("src", "config.h")
    with open(version_file, "r", encoding="utf-8") as f:
        content = f.read()
    # Regex to find a line with #define VERSION "X.Y.Z"
    match = re.search(r'#define\s+VERSION\s+"([^"]+)"', content)
    if match:
        return match.group(1)
    else:
        raise ValueError("VERSION not found in src/config.h")

# This function extracts the version for the filesystem image from data/version.txt
def get_fs_version():
    version_file = os.path.join("data", "version.txt")
    if os.path.exists(version_file):
        with open(version_file, "r", encoding="utf-8") as f:
            return f.read().strip()  # Remove any surrounding whitespace/newlines
    else:
        raise ValueError("Filesystem version file 'data/version.txt' not found")
    

# This is the main entry point for the script when run as a post-build step.
# The 'env' and 'projenv' variables are provided by PlatformIO.
def after_build(source, target, env):
    # Extract the versions
    firmware_version = get_version()
    try:
        fs_version = get_fs_version()
    except ValueError as e:
        print(f"Warning: {e}")
        fs_version = "x"  # Use a placeholder if the version file is missing

    # Get build directory path
    build_dir = env.subst("$BUILD_DIR")  # typically .pio/build/<environment>

    # Prepare output directory in the project root
    output_dir = os.path.join(env["PROJECT_DIR"], "build")
    
    # Clear the output directory before copying
    if os.path.exists(output_dir):
        shutil.rmtree(output_dir)
    os.makedirs(output_dir)

    # Paths to generated binaries
    firmware_src = os.path.join(build_dir, "firmware.bin")
    littlefs_src = os.path.join(build_dir, "littlefs.bin")

    # Construct new filenames with version
    firmware_dest = os.path.join(output_dir, f"firmware_{firmware_version}.bin")
    littlefs_dest = os.path.join(output_dir, f"littlefs_{fs_version}.bin")

    # Copy and rename if the source files exist
    if os.path.exists(firmware_src):
        shutil.copy(firmware_src, firmware_dest)
        print(f"Copied and renamed firmware to {firmware_dest}")
    else:
        print("Warning: firmware.bin not found")

    if os.path.exists(littlefs_src):
        shutil.copy(littlefs_src, littlefs_dest)
        print(f"Copied and renamed filesystem to {littlefs_dest}")
    else:
        print("Warning: littlefs.bin not found")



# Add the post-action after building the main firmware
env.AddPostAction("buildprog", after_build)