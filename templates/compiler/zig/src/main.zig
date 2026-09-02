/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/compiler/zig/src/main.zig
 *
 * PURPOSE:
 *   Provide reusable main source for projects created with Umicom tooling.
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

/// Start the generated command and write its welcome text to standard output.
pub fn main() !void {
    // Propagate an output failure so the operating system receives a useful result.
    try std.fs.File.stdout().writeAll("Hello from an Umicom Zig project.\n");
}
