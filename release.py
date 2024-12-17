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

# This is the main entry point for the script when run as a post-build step.
# The 'env' and 'projenv' variables are provided by PlatformIO.
def after_build(source, target, env):
    # Extract the version
    version = get_version()

    # Get build directory path
    build_dir = env.subst("$BUILD_DIR")  # typically .pio/build/<environment>

    # Prepare output directory in the project root
    output_dir = os.path.join(env["PROJECT_DIR"], "build")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Paths to generated binaries
    firmware_src = os.path.join(build_dir, "firmware.bin")
    littlefs_src = os.path.join(build_dir, "littlefs.bin")

    # Construct new filenames with version
    firmware_dest = os.path.join(output_dir, f"firmware_{version}.bin")
    littlefs_dest = os.path.join(output_dir, f"littlefs_{version}.bin")

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