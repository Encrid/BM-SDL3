# Copyright (C) 2026 Enkrid
#
# Under the GNU Affero General Public License, version 3 only: see license.
import operator
import os
import re
import sys
from pathlib import Path

TARGETS = {'x86_64-windows-gnu': ('windows', 'x86_64'), 'aarch64-windows-gnu': ('windows', 'aarch64'),
           'x86_64-linux-gnu': ('linux', 'x86_64'), 'aarch64-linux-gnu': ('linux', 'aarch64'),
           'x86_64-macos': ('macos', 'core2'), 'aarch64-macos': ('macos', 'aarch64')}
FEATURES = {'x86_64': ('mmx', 'sse', 'sse2'), 'core2': ('mmx', 'sse', 'sse2', 'sse3', 'ssse3'), 'aarch64': ('neon',)}
MARKERS = ('SDL_build_config.h.cmake', '});', 'sdl_mod.addCSourceFiles(.{', 'const sdl_uclibc_c_files = .{',
           'if (windows) {', 'if (linux) {', 'if (macos) {', 'if (emscripten) {')
WAYLAND = ('pub const wayland_c_files = .{', '};')
LISTS = ('sdl_common', 'sdl_libm', 'sdl_windows', 'sdl_linux', 'sdl_macos')
ENTRY = re.compile(r'\s*\.([A-Z]\w*) = (.+),')
FILE = re.compile(r'\s*"([^"]+)",')
CPU = re.compile(r'\b(?:x86|arm|aarch64|loongarch)\b')
FEATURE = re.compile(r'std\.Target\.\w+\.featureSetHas\(cpu\.features, \.(\w+)\)')
TOKEN = re.compile(r'\w+|[=!>]=|\S')
SONAME = re.compile(r'if \(linux_deps_values\) \|x\| b\.fmt\("\\"\{s\}\\"", \.\{x\.(\w+_soname)\}\) else ""')
NUMBER = re.compile(r'if \(linux_deps_values != null\) @as\(i64, (\d+)\) else null')
VERSION = re.compile(r'if \(linux_deps_values\) \|x\| @as\(i64, @intCast\(x\.(\w+_version)\.(major|minor|patch)\)\)'
                     r' else null')
SONAMES = re.compile(r'pub const (\w+_soname) = "([^"]*)";')
VERSIONS = re.compile(r'pub const (\w+_version): std\.SemanticVersion = '
                      r'\.\{ \.major = (\d+), \.minor = (\d+), \.patch = (\d+) \};')
JOINS = (('or', operator.or_), ('and', operator.and_))
COMPARES = {'==': operator.eq, '!=': operator.ne, '>=': operator.ge}
USAGE = 'usage: python tools/convert.py <build of SDL3> <build of SDL_linux_deps> <folder of SDL3> <output or ->'


def config_block(lines):
    return [match.groups() for match in map(ENTRY.fullmatch, lines) if match]


def evaluate(name, expr, target, deps):
    system, cpu = TARGETS[target]
    soname, number, version = (form.fullmatch(expr) for form in (SONAME, NUMBER, VERSION))
    if soname:
        return '"%s"' % deps[soname.group(1)] if deps is not None else ''
    if number:
        return int(number.group(1)) if deps is not None else None
    if version:
        return deps[version.group(1)][version.group(2)] if deps is not None else None
    names = {'true': True, 'false': False, 'null': None, 'windows': system == 'windows', 'linux': system == 'linux',
             'macos': system == 'macos', 'emscripten': False, 'x86': target.startswith('x86_64'), 'arm': False,
             'aarch64': target.startswith('aarch64'), 'loongarch': False, 'glibc_2_x': 31,
             'emscripten_pthreads': False, 'msvc': False, 'musl': False}
    tokens = TOKEN.findall(FEATURE.sub(lambda m: str(m.group(1) in FEATURES[cpu]).lower(), expr)) + ['']

    def stop(why):
        sys.exit(why % (name, expr))

    def boolean(value):
        if not isinstance(value, bool):
            stop('not a boolean: %s = %s')
        return value

    def atom():
        token = tokens.pop(0)
        if token == '!':
            return not boolean(atom())
        if token == '(':
            value = joined(0)
            return value if tokens.pop(0) == ')' else stop('cannot evaluate %s = %s')
        if token in names:
            return names[token]
        return int(token) if token.isdigit() else stop('cannot evaluate %s = %s')

    def compared():
        value = atom()
        return COMPARES[tokens.pop(0)](value, atom()) if tokens[0] in COMPARES else value

    def joined(depth):
        if depth == len(JOINS):
            return compared()
        word, apply = JOINS[depth]
        value = joined(depth + 1)
        while tokens[0] == word:
            tokens.pop(0)
            value = apply(boolean(value), boolean(joined(depth + 1)))
        return value
    value = joined(0)
    return value if tokens == [''] else stop('cannot evaluate %s = %s')


def sources_between(lines, first, last, skip):
    kept, depth = [], 0
    for line in lines[first + 1:last]:
        if depth or line.strip() in skip:
            depth += line.count('{') - line.count('}')
        else:
            kept.append(line)
    return [match.group(1) for match in map(FILE.fullmatch, kept) if match]


def toml_list(name, items):
    quoted = ['    %s,\n' % (("'%s'" if '"' in item else '"%s"') % item) for item in items]
    return '%s = [%s]\n' % (name, '\n' + ''.join(quoted) if quoted else '')


def emit(per_target, lists, folders):
    systems, cpus = {}, {}
    for target, defines in per_target.items():
        system, cpu = TARGETS[target]
        systems.setdefault(system, [define for define, on_cpu in defines if not on_cpu])
        simd = [define for define, on_cpu in defines if on_cpu]
        if cpus.setdefault(cpu, simd) != simd:
            sys.exit('cpu %s differs between systems: %s' % (cpu, target))
    lists = dict(lists, **{'sdl_config_' + system: defines for system, defines in systems.items()},
                 **{'sdl_simd_' + cpu: defines for cpu, defines in cpus.items()})
    return ('[variables]\n' + ''.join('%s = "%s"\n' % folder for folder in folders.items())
            + ''.join(toml_list(name, items) for name, items in lists.items()))


def main():
    if len(sys.argv) != 5:
        print(USAGE)
        sys.exit(2)
    build, linux_deps, folder, out = sys.argv[1:]
    lines = Path(build).read_text(encoding='utf-8').splitlines()
    text = Path(linux_deps).read_text(encoding='utf-8')
    deps = dict(SONAMES.findall(text))
    deps.update((key, dict(zip(('major', 'minor', 'patch'), map(int, parts))))
                for key, *parts in VERSIONS.findall(text))

    def marked(rows, markers):
        at = [-1]
        for marker in markers:
            at.append(next(n for n in range(at[-1] + 1, len(rows)) if marker in rows[n]))
        return at[1:]
    at = marked(lines, MARKERS)
    entries = config_block(lines[at[0]:at[1]])
    lists = {name: sources_between(lines, first, last, ('if (msvc) {',) if name == 'sdl_windows' else ())
             for name, first, last in zip(LISTS, at[2:], at[3:])}
    missing = [file for files in lists.values() for file in files if not (Path(folder) / file).is_file()]
    if missing:
        sys.exit('%s is not in SDL3' % missing[0])
    lists = {name: ['{sdl_root}/' + file for file in files] for name, files in lists.items()}
    rows = text.splitlines()
    lists['sdl_wayland'] = ['{sdl_deps}/' + file for file in sources_between(rows, *marked(rows, WAYLAND), ())]
    per_target = {target: [] for target in TARGETS}
    for target, (system, _) in TARGETS.items():
        for name, expr in entries:
            value = evaluate(name, expr, target, deps if system == 'linux' else None)
            if value is not None and value is not False and value != '':
                per_target[target].append(('%s=%s' % (name, 1 if value is True else value), bool(CPU.search(expr))))
    folders = {'sdl_root': Path(os.path.relpath(folder)).as_posix(),
               'sdl_deps': Path(os.path.relpath(os.path.dirname(linux_deps))).as_posix()}
    output = emit(per_target, lists, folders)
    if out == '-':
        sys.stdout.write(output)
    else:
        Path(out).write_text(output, encoding='utf-8', newline='\n')


if __name__ == '__main__':
    main()
