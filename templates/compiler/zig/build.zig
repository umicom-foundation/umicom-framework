// Umicom Zig application template | Sammy Hegab | Umicom Foundation | MIT
const std = @import("std");
pub fn build(b: *std.Build) void {
    const executable = b.addExecutable(.{ .name = "umicom-zig", .root_module = b.createModule(.{ .root_source_file = b.path("src/main.zig") }) });
    b.installArtifact(executable);
}
