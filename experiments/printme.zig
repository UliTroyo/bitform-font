const print = @import("std").debug.print;

pub fn main() void {
    const char: [3]u8 = .{ 226, 150, 140 };
    print("{s}\n", .{char});
}
