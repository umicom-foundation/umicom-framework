/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/compiler/zig/build.zig
 *
 * PURPOSE:
 *   Provide reusable build source for projects created with Umicom tooling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

// Umicom Zig application template | Sammy Hegab | Umicom Foundation | MIT
const std = @import("std");

/// Register the generated executable and make it available to the install step.
pub fn build(b: *std.Build) void {
    // Keep the source path and module ownership together in one build description.
    const executable = b.addExecutable(.{
        .name = "umicom-zig",
        .root_module = b.createModule(.{
            .root_source_file = b.path("src/main.zig"),
        }),
    });
    // Installation exposes the executable through the normal Zig output folder.
    b.installArtifact(executable);
}
