Import("env")
import os

# Directory containing the files to embed
data_dir = os.path.join(env['PROJECT_DIR'], 'data')

# List to hold all files to embed
embed_files = []

# Walk through the data directory and collect all files
for root, dirs, files in os.walk(data_dir):
    for file in files:
        # Get the full path to the file
        full_path = os.path.join(root, file)
        # Get the relative path to the file from the project directory
        rel_path = os.path.relpath(full_path, env['PROJECT_DIR'])
        embed_files.append(rel_path)

# Set the list of files to embed in the build environment
env['PROJECT_BUILD_EMBED_TXTFILES'] = embed_files
