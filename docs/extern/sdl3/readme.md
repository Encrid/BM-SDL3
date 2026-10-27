# BL-SDL3

SDL3 3.4.18, built from source on 6 targets: the one SDL3 library of the SDL3 modules of Bridge.

## Install

Python 3.11 or a later version.

## Usage

| Command | Result |
| --- | --- |
| `python project.py fetch` | Downloads what the build needs. |
| `python project.py build` | Builds the project. |
| `python project.py pack` | Packs the build for a release. |
| `python project.py test` | Runs the tests. |
| `python project.py test --break` | Runs the tests with their claims made false. Each test must fail. |
| `python project.py test --branches` | Runs the tests on a build with probes. Each branch of the code must run. |
| `python project.py test --leaks` | Runs the tests on a build with counts. Each test must give back what the code takes. |
| `python project.py check` | Checks the project. |
| `python project.py clean` | Removes what the build made. Keeps what `fetch` downloaded. |
| `python project.py distclean` | Removes the folder `build`, also what `fetch` downloaded. |

Each release holds, for each target, the SDL3 library, the headers of SDL3 in `include/SDL3` and the notices of its sources.
It also holds `leaks.toml`, which names the resources of SDL3 for `[leaks] with` of a module.

The library is `SDL3.dll`, `SDL3.so` or `SDL3.dylib`. Its soname on Linux is `SDL3.so`, and its install name on macOS is `@rpath/SDL3.dylib`.
A module links the library itself, so the release holds no import library for Windows.
BL-SDL3 holds no module. Each SDL3 module of Bridge fetches its release, and a deployment holds one copy of the SDL3 library, beside the modules.

These rules let several modules share the one SDL3 of a process:

- Each SDL3 module loads this SDL3 library, so all of them use one queue of events and one state.
- Only the module `sdl3:events` calls `SDL_PumpEvents`, on the main thread, at the start of each round.
- Each other module takes only the types of event that it covers, with `SDL_PeepEvents`.
- No module calls `SDL_PollEvent` or `SDL_WaitEvent`, because they take the events of each module.

## Contributing

| Document | Content |
| --- | --- |
| `docs/contributing/coding_style.md` | How to write the code. |
| `docs/contributing/documentation_style.md` | How to write the documents. |
| `docs/contributing/tests.md` | How to write the tests. |
| `docs/contributing/commits.md` | How to write a commit. |
| `docs/contributing/pull_request.md` | How to write a pull request. |
| `docs/contributing/agreement.md` | The agreement `cla.md`, which each contributor signs. |

## License

The GNU Affero General Public License, version 3 only. The file `license` holds the text.
Each source of third-party code has its notice in `third_party/<source>/license`.
SDL3 is under the zlib licence: `third_party/sdl3/license`.
The Linux build of SDL3 also holds code of SDL_linux_deps: `third_party/sdl3_linux/license` holds its notices.
