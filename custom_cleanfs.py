import os
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

def clean_filesystem(target, source, env):
    build_dir = env.subst("$BUILD_DIR")
    fs_image_path = os.path.join(build_dir, "littlefs.bin")

    if os.path.exists(fs_image_path):
        os.remove(fs_image_path)
        print(f"[INFO] Filesystem image cleaned: {fs_image_path}")
    else:
        print(f"[INFO] No filesystem image to clean.")

# Add a target called 'cleanfs' with correct arguments
env.AddTarget(
    name="cleanfs",                      # Target name
    actions=[clean_filesystem],          # List of actions to execute
    dependencies=[],                     # No dependencies for this target
    title="Clean Filesystem Image",      # Display title
    description="Remove the filesystem image to force a rebuild"  # Description
)
