import os
import gzip
from pathlib import Path
from css_html_js_minify import process_single_js_file, process_single_css_file

Import("env")


def compress_file(file_path):
    """Compress a file using gzip."""
    compressed_file_path = file_path.with_suffix(file_path.suffix + ".gz")
    with open(file_path, 'rb') as f_in:
        with gzip.open(compressed_file_path, 'wb') as f_out:
            f_out.writelines(f_in)
    print(f"Compressed: {file_path} -> {compressed_file_path}")

def minify_file(file_path, file_type):
    """Minify a JavaScript or CSS file."""
    if file_type == "js":
        process_single_js_file(str(file_path), overwrite=True)
    elif file_type == "css":
        process_single_css_file(str(file_path), overwrite=True)
    print(f"Minified: {file_path}")
    
def process_directory(source, target, env):
    """Process the data directory."""
    base_data_dir = Path("data")

    if not base_data_dir.exists():
        print("The 'data' folder does not exist.")
        return

    # Compress HTML files
    for html_file in base_data_dir.rglob("*.html"):
        compress_file(html_file)

    # Minify and compress JavaScript files
    js_dir = base_data_dir / "assets/js"
    if js_dir.exists():
        for js_file in js_dir.rglob("*.js"):
            minify_file(js_file, "js")
            compress_file(js_file)

    # Minify and compress CSS files
    css_dir = base_data_dir / "assets/css"
    if css_dir.exists():
        for css_file in css_dir.rglob("*.css"):
            minify_file(css_file, "css")
            compress_file(css_file)



env.AddPreAction("buildprog", process_directory)
