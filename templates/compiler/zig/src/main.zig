// Umicom Zig application template | Sammy Hegab | Umicom Foundation | MIT
const std = @import("std");
pub fn main() !void { try std.fs.File.stdout().writeAll("Hello from an Umicom Zig project.\n"); }
