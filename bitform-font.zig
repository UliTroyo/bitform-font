// 1. Take ASCII characters from stdin until we have a total of 80 visual
//    characters ready for output. For this, we need the glyph width in
//    chars, not the width in bytes (my previous mistake). We split on
//    consumed whitespace characters. We ignore all other input.
//
// 2. Our data structure should encode four line arrays, or one single
//    array that encodes these 4 lines, where each item has the following
//    information, per glyph index:
//      - The width of the glyph in bytes
//      - What byte preamble each code point has
//      - What the last byte of each code point is
//    Additionally, for each glyph index, we also need the width of the
//    glyph in chars, as mentioned above.
//
// 3. So we take our stdin, until the total glyph width is over 80. Then,
//    we split on the last whitespace index. Then depending on whether
//    the alignment is left/center/right, we optionally pad the line
//    with spaces.
//
// 4. If the entire first line has no non-space code points, we skip it.
//
// 5. We print the two middle lines. For each character in the current
//    input slice, we calculate the total width in bytes, and we begin
//    to fill that buffer with glyph data for that line depending on what
//    byte preamble it has plus the last byte. After the last code point,
//    we add a newline. After each outputtable line, we write to stdout.
//
// 6. On the fourth line, we first determine if there's still input left
//    to process. If there is, we process the width required, and finalize
//    the layout. Then, if the first code point line of this new glyph line
//    is comprised of only spaces, we output the fourth line as-is.
//    Otherwise, we perform a splat operation where any code point index
//    that intersects with a non-space code point gets transformed in
//    order to create a middle line with intersected code points.
//
// 7. We repeat this process for all input.

// TODO:
// - Run experiment to try to pack a single struct array at comptime
// - Run experiment to ty to pack variable widths within a struct
// - etc.

const std = @import("std");

/// One single glyph.
pub const Glyph = struct {
    total_codepoints: u3,
    lines: [4]Line,

    /// A single line comprising a glyph.
    pub const Line = struct {
        total_bytes: u3,
        points: union(enum) {
            one: [1]Point,
            two: [2]Point,
            three: [3]Point,
            four: [4]Point,
        },

        /// A single codepoint within a line.
        pub const Point = struct {
            preamble: enum(u2) { none, short, long },
            last_byte: u8,
        };
    };
};

const ComptimeGlyph = struct { lines: [4][]const u8, total_codepoints: u3 };

const fontdata: [2]ComptimeGlyph = .{
    // ' '
    .{
        .lines = .{
            "  ",
            "  ",
            "  ",
            "  ",
        },
        .total_codepoints = 2,
    },
    // !
    .{
        .lines = .{
            " ",
            "▌",
            "🬐",
            " ",
        },
        .total_codepoints = 1,
    },
};

fn generateLine(line: []const u8) Glyph.Line {
    const total_points = try std.unicode.utf8CountCodepoints(line);
    var point_arr: Glyph.Line = .{
        .points = switch (total_points) {
            1 => .one,
            2 => .two,
            3 => .three,
            4 => .four,
            else => unreachable,
        },
        .total_bytes = undefined,
    };
    var utf8 = (std.unicode.Utf8View.initComptime(line)).iterator();
    var point_idx = 0;

    while (utf8.nextCodepointSlice()) |codepoint| {
        point_arr[point_idx] = Glyph.Line.Point{
            .preamble = switch (codepoint.len) {
                1 => .none,
                3 => .short,
                4 => .long,
                else => @compileError("Invalid codepoint length"),
            },
            .last_byte = codepoint[codepoint.len - 1],
        };
        point_idx += 1;
    }

    return point_arr;
}

fn glyphArrayFromData() [fontdata.len]Glyph {
    const glyphs = glyph_init: {
        var glyph_arr: [fontdata.len]Glyph = undefined;

        for (&glyph_arr, 0..) |*glyph, glyph_idx| {
            const data = fontdata[glyph_idx];
            const total_codepoints = data.total_codepoints;
            glyph.* = Glyph{
                .total_codepoints = total_codepoints,
                .lines = undefined,
            };
            for (&glyph.lines, 0..) |*line, line_idx| {
                const line_str = data.lines[line_idx];
                line.* = comptime generateLine(total_codepoints, line_str);
            }
        }
        break :glyph_init glyph_arr;
    };

    return glyphs;
}

pub fn main() !void {
    const glyph_array: [2]Glyph = comptime glyphArrayFromData();
    // const stdin = std.io.getStdIn().reader();
    const stdout = std.io.getStdOut().writer();

    // var input_buffer: [1024]u8 = undefined;

    // const input_size = try stdin.readAll(&input_buffer);
    // const input = input_buffer[0..input_size];

    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    // var line_buffers: [4]std.ArrayList([]const u8) = .{std.ArrayList([]const u8).init(allocator)} ** 4;

    // for (input) |byte| {
    //     if (byte == ' ' or byte == '!') {
    //         // const idx = byte - 32;
    //         for (0..4) |line_idx| {
    //             try line_buffers[line_idx].appendSlice(glyph_array[0].lines[line_idx]);
    //         }
    //     }
    // }

    // for (0..4) |i| {
    //     try stdout.writeAll(line_buffers[i].items);
    //     try stdout.writeByte('\n');
    // }

    // Stringify to JSON
    const json_string = try std.json.stringifyAlloc(allocator, glyph_array, .{
        .whitespace = .indent_2,
    });
    defer allocator.free(json_string);

    // Write the JSON to stdout
    try stdout.print("{s}\n", .{json_string});
}

// /// These should be in index order.
// const glyphs: []Glyph =

// const fontdata = [69][4][]const u8{
//     // ' '
//     [4][]const u8{
//         "  ", //
//         "  ", //
//         "  ", //
//         "  ", //
//     },
//     // !
//     [4][]const u8{
//         " ", //
//         "▌", //
//         "🬐", //
//         " ", //
//     },
//     // "
//     [4][]const u8{
//         "  ", //
//         "🬄🬄", //
//         "  ", //
//         "  ", //
//     },
//     // #
//     [4][]const u8{
//         "   ", //
//         "🬵🬵🬏", //
//         "🬫🬫🬃", //
//         "   ", //
//     },
//     // $
//     [4][]const u8{
//         " 🬞  ", //
//         "🬤🬸🬰 ", //
//         "🬢🬷🬭🬄", //
//         " 🬁  ", //
//     },
//     // %
//     [4][]const u8{
//         "   ", //
//         "🬃🬞🬃", //
//         "🬖🬀🬏", //
//         "   ", //
//     },
//     // &
//     [4][]const u8{
//         "    ", //
//         "🬤🬡🬃 ", //
//         "🬣🬗🬘🬀", //
//         "    ", //
//     },
//     // '
//     [4][]const u8{
//         "  ", //
//         "🬞🬄", //
//         "  ", //
//         "  ", //
//     },
//     // (
//     [4][]const u8{
//         " 🬭 ", //
//         "🬔  ", //
//         "🬣  ", //
//         " 🬂 ", //
//     },
//     // )
//     [4][]const u8{
//         "🬞🬏 ", //
//         " 🬁🬓", //
//         " 🬞🬄", //
//         "🬁🬀 ", //
//     },
//     // *
//     [4][]const u8{
//         "   ", //
//         "🬢🬣🬃", //
//         "🬀🬄🬀", //
//         "   ", //
//     },
//     // +
//     [4][]const u8{
//         "   ", //
//         " 🬓 ", //
//         "🬂🬕🬀", //
//         "   ", //
//     },
//     // ,
//     [4][]const u8{
//         "  ", //
//         "  ", //
//         " 🬓", //
//         "  🬁 ", //
//     },
//     // -
//     [4][]const u8{
//         "   ", //
//         "   ", //
//         "🬂🬂🬀", //
//         "   ", //
//     },
//     // .
//     [4][]const u8{
//         " ", //
//         " ", //
//         "🬏", //
//         " ", //
//     },
//     // /
//     [4][]const u8{
//         "  ", //
//         "🬞🬄", //
//         "🬔 ", //
//         "  ", //
//     },
//     // 0
//     [4][]const u8{
//         "   ", //
//         "🬜🬊🬓", //
//         "🬪🬵🬄", //
//         "   ", //
//     },
//     // 1
//     [4][]const u8{
//         "  ", //
//         "🬖▌", //
//         " ▌", //
//         "  ", //
//     },
//     // 2
//     [4][]const u8{
//         "   ", //
//         "🬅🬂🬓", //
//         "🬳🬰🬏", //
//         "   ", //
//     },
//     // 3
//     [4][]const u8{
//         "   ", //
//         "🬁🬰🬄", //
//         "🬢🬭🬄", //
//         "   ", //
//     },
//     // 4
//     [4][]const u8{
//         "   ", //
//         "🬦🬧 ", //
//         "🬌🬫🬃", //
//         "   ", //
//     },
//     // 5
//     [4][]const u8{
//         "   ", //
//         "🬴🬰🬀", //
//         "🬢🬭🬄", //
//         "   ", //
//     },
//     // 6
//     [4][]const u8{
//         "   ", //
//         "🬖🬡🬀", //
//         "🬪🬮🬄", //
//         "   ", //
//     },
//     // 7
//     [4][]const u8{
//         "   ", //
//         "🬂🬡🬄", //
//         "🬦🬀 ", //
//         "   ", //
//     },
//     // 8
//     [4][]const u8{
//         "   ", //
//         "🬤🬰🬃", //
//         "🬣🬭🬄", //
//         "   ", //
//     },
//     // 9
//     [4][]const u8{
//         "   ", //
//         "🬔🬂🬓", //
//         "🬠🬰🬄", //
//         "   ", //
//     },
//     // ,
//     [4][]const u8{
//         " ", //
//         "🬃", //
//         "🬃", //
//         " ", //
//     },
//     // ;
//     [4][]const u8{
//         "  ", //
//         " 🬏", //
//         " 🬓", //
//         "  🬁 ", //
//     },
//     // <
//     [4][]const u8{
//         " 🬞 ", //
//         "🬖🬀 ", //
//         "🬁🬢 ", //
//         "   ", //
//     },
//     // =
//     [4][]const u8{
//         "   ", //
//         "🬋🬋🬃", //
//         "🬋🬋🬃", //
//         "   ", //
//     },
//     // >
//     [4][]const u8{
//         "🬞  ", //
//         " 🬈🬏", //
//         "🬞🬅 ", //
//         "   ", //
//     },
//     // ?
//     [4][]const u8{
//         "   ", //
//         "🬅🬡🬃", //
//         " 🬐 ", //
//         "   ", //
//     },
//     // @
//     [4][]const u8{
//         "    ", //
//         "🬜🬡🬒🬓", //
//         "▌🬣🬣🬄", //
//         "    ", //
//     },
//     // A
//     [4][]const u8{
//         "   ", //
//         "🬜🬊🬓", //
//         "▌🬂▌", //
//         "   ", //
//     },
//     // B
//     [4][]const u8{
//         "   ", //
//         "🬕🬂🬓", //
//         "🬲🬰🬃", //
//         "   ", //
//     },
//     // C
//     [4][]const u8{
//         "   ", //
//         "🬔🬂🬃", //
//         "🬣🬭🬃", //
//         "   ", //
//     },
//     // D
//     [4][]const u8{
//         "   ", //
//         "🬕🬈🬏", //
//         "🬲🬵🬄", //
//         "   ", //
//     },
//     // E
//     [4][]const u8{
//         "   ", //
//         "🬕🬂🬀", //
//         "🬴🬰🬏", //
//         "   ", //
//     },
//     // F
//     [4][]const u8{
//         "   ", //
//         "🬕🬂🬀", //
//         "🬕🬂 ", //
//         "   ", //
//     },
//     // G
//     [4][]const u8{
//         "   ", //
//         "🬜🬂🬀", //
//         "🬣🬯🬄", //
//         "   ", //
//     },
//     // H
//     [4][]const u8{
//         "   ", //
//         "▌ ▌", //
//         "🬕🬂▌", //
//         "   ", //
//     },
//     // I
//     [4][]const u8{
//         "   ", //
//         "🬂🬕🬀", //
//         "🬭🬲🬏", //
//         "   ", //
//     },
//     // J
//     [4][]const u8{
//         "   ", //
//         "🬁🬨🬀", //
//         "🬢🬘 ", //
//         "   ", //
//     },
//     // K
//     [4][]const u8{
//         "   ", //
//         "▌🬖🬀", //
//         "🬕🬋🬏", //
//         "   ", //
//     },
//     // L
//     [4][]const u8{
//         "   ", //
//         "▌  ", //
//         "🬲🬭🬏", //
//         "   ", //
//     },
//     // M
//     [4][]const u8{
//         "    ", //
//         "🬛🬏🬖▌", //
//         "▌🬁 ▌", //
//         "    ", //
//     },
//     // N
//     [4][]const u8{
//         "   ", //
//         "🬛🬏▌", //
//         "▌🬈▌", //
//         "   ", //
//     },
//     // O
//     [4][]const u8{
//         "   ", //
//         "🬔🬂🬓", //
//         "🬣🬭🬄", //
//         "   ", //
//     },
//     // P
//     [4][]const u8{
//         "   ", //
//         "🬕🬂🬓", //
//         "▌🬂 ", //
//         "   ", //
//     },
//     // Q
//     [4][]const u8{
//         "   ", //
//         "🬔🬂🬓", //
//         "🬣🬗🬐", //
//         "   ", //
//     },
//     // R
//     [4][]const u8{
//         "   ", //
//         "🬕🬂🬓", //
//         "▌🬂🬓", //
//         "   ", //
//     },
//     // S
//     [4][]const u8{
//         "   ", //
//         "🬳🬂🬃", //
//         "🬢🬰🬃", //
//         "   ", //
//     },
//     // T
//     [4][]const u8{
//         "   ", //
//         "🬂🬕🬀", //
//         " ▌ ", //
//         "   ", //
//     },
//     // U
//     [4][]const u8{
//         "   ", //
//         "▌ ▌", //
//         "🬣🬭🬄", //
//         "   ", //
//     },
//     // V
//     [4][]const u8{
//         "   ", //
//         "▌ ▌", //
//         "🬉🬘 ", //
//         "   ", //
//     },
//     // W
//     [4][]const u8{
//         "    ", //
//         "🬣🬞🬞🬄", //
//         "🬁🬔🬔 ", //
//         "    ", //
//     },
//     // X
//     [4][]const u8{
//         "   ", //
//         "🬣🬞🬄", //
//         "🬖🬈🬏", //
//         "   ", //
//     },
//     // Y
//     [4][]const u8{
//         "   ", //
//         "🬣🬞🬄", //
//         " ▌ ", //
//         "   ", //
//     },
//     // Z
//     [4][]const u8{
//         "   ", //
//         "🬂🬡🬄", //
//         "🬵🬮🬏", //
//         "   ", //
//     },
//     // [
//     [4][]const u8{
//         "🬭 ", //
//         "▌ ", //
//         "▌ ", //
//         "🬂 ", //
//     },
//     // \
//     [4][]const u8{
//         "  ", //
//         "🬣 ", //
//         "🬁🬓", //
//         "  ", //
//     },
//     // ]
//     [4][]const u8{
//         "🬞🬏", //
//         " ▌", //
//         " ▌", //
//         "🬁🬀", //
//     },
//     // ^
//     [4][]const u8{
//         "   ", //
//         "🬖🬈🬏", //
//         "   ", //
//         "   ", //
//     },
//     // _
//     [4][]const u8{
//         "   ", //
//         "   ", //
//         "🬭🬭🬏", //
//         "   ", //
//     },
//     // `
//     [4][]const u8{
//         "  ", //
//         "🬁🬃", //
//         "  ", //
//         "  ", //
//     },
//     // {
//     [4][]const u8{
//         " 🬭 ", //
//         "▐  ", //
//         "🬧  ", //
//         " 🬂 ", //
//     },
//     // |
//     [4][]const u8{
//         " ", //
//         "▌", //
//         "▌", //
//         " ", //
//     },
//     // }
//     [4][]const u8{
//         "🬞🬏 ", //
//         " ▐ ", //
//         " 🬦🬀", //
//         "🬁🬀 ", //
//     },
//     // ~
//     [4][]const u8{
//         "   ", //
//         "🬞🬏🬏", //
//         "🬀🬂 ", //
//         "   ", //
//     },
// };
