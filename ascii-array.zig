const std = @import("std");

pub fn main() !void {
    const my_arr = init: {
        const first_char = ' ';
        const last_char = '~';
        const total_printable_chars = (last_char + 1) - first_char;

        var initial_value: [total_printable_chars]u8 = undefined;
        for (&initial_value, 0..) |*char, i| {
            char.* = @intCast(i + first_char);
        }
        break :init initial_value;
    };

    std.debug.print(">>>", .{});
    for (&my_arr) |item| {
        std.debug.print("{u}", .{item});
    }
    std.debug.print("<<<\n", .{});
}
