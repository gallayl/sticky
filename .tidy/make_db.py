import json
import shlex
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
src = json.load(open(os.path.join(ROOT, "compile_commands.json")))

# GCC-only / xtensa-only flags that mainline clang rejects as errors.
DROP = {
    "-mlongcalls",
    "-mtext-section-literals",
    "-falign-functions=4",
    "-free",
    "-fipa-pta",
    "-fno-tree-switch-conversion",
}

out = []
for e in src:
    if e["file"] != "src/main.cpp":
        continue
    args = shlex.split(e["command"])
    args = [a for a in args if a not in DROP]
    # Use clang driver; keep xtensa target defines so framework headers pick
    # the right code paths, but parse with the host so headers resolve.
    args[0] = "clang++"
    # Cross-parse for xtensa: don't let clang default to the host target and
    # pull in /usr/include (x86-64 glibc), which collides with newlib. Use the
    # toolchain sysroot and add its newlib + libstdc++ includes explicitly.
    tc = "/home/lgallay/.platformio/packages/toolchain-xtensa"
    gcc_ver = "10.3.0"
    args += [
        "-nostdinc",
        "-nostdinc++",
        f"-isystem{tc}/xtensa-lx106-elf/include/c++/{gcc_ver}",
        f"-isystem{tc}/xtensa-lx106-elf/include/c++/{gcc_ver}/xtensa-lx106-elf",
        f"-isystem{tc}/lib/gcc/xtensa-lx106-elf/{gcc_ver}/include",
        f"-isystem{tc}/lib/gcc/xtensa-lx106-elf/{gcc_ver}/include-fixed",
        f"-isystem{tc}/xtensa-lx106-elf/include",
        "-D__XTENSA__",
        "-D__ets__",
        # lwip's arch.h falls back to `typedef int ssize_t` unless SSIZE_MAX is
        # defined, which clashes with newlib's `long` ssize_t. Define it so lwip
        # uses the system type instead.
        "-DSSIZE_MAX=__LONG_MAX__",
        "-Wno-unknown-attributes",
        "-Wno-unknown-warning-option",
        "-Wno-attributes",
        # GNU extensions the xtensa framework relies on (void* arithmetic in
        # pgmspace.h) that clang would otherwise reject.
        "-Wno-pointer-arith",
        "-fms-extensions",
    ]
    out.append({"directory": e["directory"], "arguments": args, "file": e["file"]})

json.dump(out, open(os.path.join(ROOT, ".tidy", "compile_commands.json"), "w"), indent=2)
print("wrote", len(out), "entries")
