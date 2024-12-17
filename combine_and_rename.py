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

# Combine firmware and filesystem images
def combine_firmware_and_fs(source, target, env):
    print("[INFO] Combining firmware and filesystem images...")
    build_dir = env.subst("$BUILD_DIR").replace("\\", "/")
    firmware_path = os.path.join(build_dir, "firmware.bin").replace("\\", "/")
    fs_path = os.path.join(build_dir, "littlefs.bin").replace("\\", "/")
    output_path = os.path.join(build_dir, f"firmware-{version}.bin").replace("\\", "/")

    # Verify both input files exist
    if not os.path.isfile(firmware_path):
        print(f"[ERROR] Firmware file not found: {firmware_path}")
        return
    if not os.path.isfile(fs_path):
        print(f"[ERROR] Filesystem image not found: {fs_path}")
        return

    # Command to combine binaries
    command = [
        "esptool.py", "--chip", "esp32", "merge_bin",
        "-o", output_path,
        "--flash_mode", "dio", "--flash_freq", "40m", "--flash_size", "8MB",
        "0x10000", firmware_path,
        "0x290000", fs_path
    ]

    print(f"[INFO] Executing: {' '.join(command)}")

    try:
        result = subprocess.run(command, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        print("[INFO] esptool.py Output:")
        print(result.stdout)
        if result.stderr:
            print("[ERROR] esptool.py Error:")
            print(result.stderr)

        # Verify output file
        if os.path.isfile(output_path):
            print(f"[INFO] Combined firmware created successfully: {output_path}")
        else:
            print(f"[ERROR] Combined firmware not found: {output_path}")

    except subprocess.CalledProcessError as e:
        print("[ERROR] esptool execution failed:")
        print(e.stderr)

# Hook into the build process
env.AddPreAction("buildprog", build_filesystem_image)
env.AddPostAction("buildprog", combine_firmware_and_fs)
