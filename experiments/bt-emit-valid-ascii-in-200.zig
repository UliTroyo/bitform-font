const std = @import("std");

pub fn main() !void {
    const my_arr = init: {
        const first_char = ' ';
        const last_char = '~';

        var initial_value: [200]u8 = undefined;
        for (&initial_value, 0..) |*char, i| {
            if (i >= first_char and i <= last_char) {
                char.* = @intCast(i);
            } else {
                char.* = '-';
            }
        }
        break :init initial_value;
    };

    std.debug.print(">", .{});
    for (&my_arr) |item| {
        std.debug.print("{u}", .{item});
    }
    std.debug.print("<\n", .{});
}
