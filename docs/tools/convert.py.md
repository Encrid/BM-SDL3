# tools/convert.py

`convert.py` writes `.toolchain/config/variables.toml`: the sources and the defines of the build of SDL3, for each target.
It reads them from the zig build of SDL3 by castholm, and from the build of SDL_linux_deps. A `[[source]]` pins each of the two.

```sh
python tools/convert.py <build of SDL3> <build of SDL_linux_deps> <folder of SDL3> <output or ->
```

With `-` as the output, the tool writes to the standard output, so a case can compare the output with the file of the project.
Run it again when the pin of SDL3 changes.
With another count of arguments, the tool writes `usage: python tools/convert.py <build of SDL3> <build of SDL_linux_deps> <folder of SDL3> <output or ->`, and ends with 2.

## The input

From the build of SDL3, the tool reads these parts, each from the line that starts it:

- the block of the config: the line that names `SDL_build_config.h.cmake`, up to the line `});`;
- the common sources: the line `sdl_mod.addCSourceFiles(.{`;
- the sources of libm: the line `const sdl_uclibc_c_files = .{`;
- the sources of each system: the lines `if (windows) {`, `if (linux) {` and `if (macos) {`, up to `if (emscripten) {`.

The tool finds each line after the line before it, in the order of the list. So a line of the same text earlier in the file does not count.
In the sources of Windows, it leaves out the block `if (msvc) {`.

From the build of SDL_linux_deps, it reads each line `pub const <name>_soname = "<soname>";`, each line `pub const <name>_version: std.SemanticVersion = .{ .major = <n>, .minor = <n>, .patch = <n> };`, and the list `wayland_c_files`.

## The defines

1. It reads the block of `SDL_build_config.h.cmake`: a name and an expression of zig on each line.
2. For each target, it evaluates each expression with the system and the features of the CPU of the target.
3. A true value gives `<name>=1`. A false value, `null` and an empty text give no define.
4. On Linux, a soname of SDL_linux_deps gives its name in quotes.
5. On Linux, a version comes from the build of SDL3, or from a version of SDL_linux_deps.

The evaluator runs no code of its input. It reads only these forms:

- `and`, `or`, `!`, `==`, `!=`, `>=`, parentheses, numbers, `true`, `false` and `null`;
- the names of the systems, `x86`, `arm`, `aarch64` and `loongarch`, `glibc_2_x` as 31, and `emscripten_pthreads`, `msvc` and `musl` as false;
- `std.Target.<cpu>.featureSetHas(cpu.features, .<feature>)`, true when the CPU of the target has the feature;
- `if (linux_deps_values) |x| b.fmt("\"{s}\"", .{x.<name>_soname}) else ""`, the soname in quotes;
- `if (linux_deps_values != null) @as(i64, <n>) else null`, the number `<n>`;
- `if (linux_deps_values) |x| @as(i64, @intCast(x.<name>_version.<part>)) else null`, a part of a version.

An expression that it cannot read stops the tool: `cannot evaluate <name> = <expression>`.
`and`, `or` and `!` take only booleans. An operand that is no boolean, for example `null`, stops the tool: `not a boolean: <name> = <expression>`.

| Target | CPU | Features |
| --- | --- | --- |
| `x86_64-windows-gnu`, `x86_64-linux-gnu` | `x86_64` | mmx, sse, sse2 |
| `x86_64-macos` | `core2` | mmx, sse, sse2, sse3, ssse3 |
| `aarch64-windows-gnu`, `aarch64-linux-gnu`, `aarch64-macos` | `aarch64` | neon |

## The sources

The tool takes the files of the lists of the build: the common files, the files of libm, and the files of each system.
On Linux, it also takes the files of wayland of SDL_linux_deps.
A file that the folder of SDL3 does not hold stops the tool: `<file> is not in SDL3`.

## The output

The output holds these variables:

- `sdl_root`: the folder of SDL3, the third argument; `sdl_deps`: the folder of the build of SDL_linux_deps. Each one is a path from the folder in which the tool runs;
- `sdl_common`, `sdl_libm`, `sdl_windows`, `sdl_linux`, `sdl_macos` and `sdl_wayland`: the lists of the sources;
- `sdl_config_<system>`: the defines that each target of a system shares;
- `sdl_simd_<cpu>`: the other defines of each CPU, which turn the paths of SIMD on and off.

An empty list is `[]`.

The defines of a CPU must be the same on each system. If they are not, the tool stops: `cpu <cpu> differs between systems: <target>`.

## Functions

| Function | Result |
| --- | --- |
| `config_block(lines)` | Gives the name and the expression of each entry of the build config. |
| `evaluate(name, expr, target, deps)` | Gives the value of one expression for a target, with the sonames and the versions of SDL_linux_deps in `deps`, or nothing. Stops on an expression that it cannot read, and on an operand that is no boolean. |
| `sources_between(lines, first, last, skip)` | Gives the files of the lists between two lines, and leaves out the blocks that start with a line of `skip`. |
| `toml_list(name, items)` | Gives the text of one list of TOML. |
| `emit(per_target, lists, folders)` | Writes the output: the two folders, the lists of the sources with the files of wayland, and the defines. |
| `main()` | Reads the four paths, and writes the output. Stops with status 1 on a fault, and with 2 on another count of arguments. |
