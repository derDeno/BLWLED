import os
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

# Extract the VERSION define from your code
def get_version_from_code():
    version = "0.0.0"  # Default version if not found
    try:
        with open("src/config.h", "r") as file:
            for line in file:
                if "#define VERSION" in line:
                    version = line.split()[-1].strip('"')
                    break
    except Exception as e:
        print(f"[ERROR] Failed to read version: {str(e)}")
    return version

# Generate the output firmware name
version = get_version_from_code()
firmware_name = f"firmware-{version}.bin"
print(f"[INFO] Target firmware name: {firmware_name}")

# Clean and rebuild the filesystem
def build_filesystem_image(source, target, env):
    print("[INFO] Cleaning and rebuilding filesystem image...")
    env.Execute("pio run -t cleanfs")
    result = env.Execute("pio run --target buildfs")
    if result != 0:
        print(f"[ERROR] Failed to build filesystem image.")
    else:
        print(f"[INFO] Filesystem image built successfully.")

# Rename the firmware after building
def rename_firmware(source, target, env):
    print("[INFO] Rename firmware script triggered...")
    build_dir = env.subst("$BUILD_DIR")
    firmware_path = os.path.join(build_dir, "firmware.bin")
    renamed_path = os.path.join(build_dir, firmware_name)
    
    if os.path.exists(firmware_path):
        try:
            os.rename(firmware_path, renamed_path)
            print(f"[INFO] Firmware renamed to: {renamed_path}")
        except Exception as e:
            print(f"[ERROR] Failed to rename firmware: {str(e)}")
    else:
        print(f"[ERROR] Firmware file not found: {firmware_path}")

# Hook into the build process
env.AddPreAction("buildprog", build_filesystem_image)
env.AddPostAction("buildprog", rename_firmware)
