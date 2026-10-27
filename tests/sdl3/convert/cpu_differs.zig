// Copyright (C) 2026 Enkrid
//
// Under the GNU Affero General Public License, version 3 only: see license.
    .style = .{ .cmake = b.path("include/build_config/SDL_build_config.h.cmake") },
    .include_path = "SDL_build_config.h",
}, .{
    .HAVE_LIBC = windows or linux or macos or emscripten,
    .SDL_VIDEO_DRIVER_RPI = linux and aarch64,
    .SDL_DISABLE_SSE3 = !(x86 and std.Target.x86.featureSetHas(cpu.features, .sse3)),
    .SDL_DISABLE_NEON = !(arm and std.Target.arm.featureSetHas(cpu.features, .neon) or aarch64 and std.Target.aarch64.featureSetHas(cpu.features, .neon)),
});
sdl_mod.addCSourceFiles(.{
    .files = &.{
        "src/SDL.c",
    },
});
const sdl_uclibc_c_files = .{
    "src/libm/e_atan2.c",
};
if (windows) {
    sdl_mod.addCSourceFiles(.{
        .files = &.{
            "src/video/windows/SDL_windowsvideo.c",
        },
    });
}
if (linux) {
    sdl_mod.addCSourceFiles(.{
        .files = &.{
            "src/video/x11/SDL_x11video.c",
        },
    });
}
if (macos) {
    sdl_mod.addCSourceFiles(.{
        .files = &.{
            "src/video/cocoa/SDL_cocoavideo.m",
        },
    });
}
if (emscripten) {
