Import("env")

def before_build_fs(*args, **kwargs):
    print("Running buildfs target...")
    env.Execute("pio run --target buildfs")


# Add the pre-action before building the main firmware
env.AddPreAction("buildprog", before_build_fs)
