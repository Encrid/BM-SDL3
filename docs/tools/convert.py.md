# tools/convert.py

`convert.py` writes `.toolchain/config/variables.toml`: the sources and the defines of the build of SDL3, for each target.
It reads them from the zig build of SDL3 by castholm, and from the build of SDL_linux_deps. A `[[source]]` pins each of the two.

```sh
python tools/convert.py <build of SDL3> <build of SDL_linux_deps> <folder of SDL3> <output>
```

Run it again when the pin of SDL3 changes. A case checks that its output is the file of the project.

## The defines

1. It reads the block of `SDL_build_config.h.cmake`: a name and an expression of zig on each line.
2. For each target, it evaluates each expression with the system and the features of the CPU of the target.
3. A true value gives `<name>=1`. A false value, or `null`, gives no define.
4. On Linux, a soname of SDL_linux_deps gives its name in quotes.
5. On Linux, the versions of xkbcommon and of libdecor give their numbers.

The evaluator reads only `and`, `or`, `!`, `==`, `!=`, `>=`, numbers, `true`, `false`, and the names of the systems and of the CPU. It runs no code of its input.
An expression that it cannot read stops the tool: `cannot evaluate <name> = <expression>`.

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

- `sdl_root` and `sdl_deps`: the folders of the two sources;
- `sdl_common`, `sdl_libm`, `sdl_windows`, `sdl_linux`, `sdl_macos` and `sdl_wayland`: the lists of the sources;
- `sdl_config_<system>`: the defines that each target of a system shares;
- `sdl_simd_<cpu>`: the other defines of each CPU, which turn the paths of SIMD on and off.

The defines of a CPU must be the same on each system. If they are not, the tool stops: `cpu <cpu> differs between systems: <target>`.

## Functions

| Function | Result |
| --- | --- |
| `config_block(lines)` | Gives the name and the expression of each entry of the build config. |
| `evaluate(name, expr, system, cpu)` | Gives the value of one expression for a target, or nothing. Stops on an expression that it cannot read. |
| `sources_between(lines, first, last, skip)` | Gives the files of the lists between two lines, and leaves out the blocks that start with a line of `skip`. |
| `toml_list(name, items)` | Gives the text of one list of TOML. |
| `emit(per_target, lists)` | Writes the output. |
| `main()` | Reads the four paths, and writes the output or stops with status 1. |
