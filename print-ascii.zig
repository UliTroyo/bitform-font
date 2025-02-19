const std = @import("std");

pub fn main() !void {
    inline for (' '..('~' + 1)) |i| {
        const x: u8 = @intCast(i);
        std.debug.print("{u}", .{x});
    }
    std.debug.print("\n", .{});
}
