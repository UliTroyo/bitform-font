const std = @import("std");

const GlyphLine = packed struct { data: GlyphLineData, total_bytes: u4 };
const GlyphLineData = union(enum) {
    not_empty: GlyphLineBytes,
    is_empty: void,
};
const GlyphWidthInGraphemes = enum { one, two, three, four };
const GraphemeLastBytes = union(GlyphWidthInGraphemes) {
    one: [1]CompressedGrapheme,
    two: [2]CompressedGrapheme,
    three: [3]CompressedGrapheme,
    four: [4]CompressedGrapheme,
};
const GraphemeWidthInBytes = enum { one, three, four };
const CompressedGrapheme = packed struct {
    width: GraphemeWidthInBytes,
    last_byte: u8,
};
const GlyphLineBytes = packed struct {
    bytes: GraphemeLastBytes,
};
const glyph_example = .{GlyphLine{
    .data = .{ //
        .not_empty = .{ .bytes = .{ .two = {} } },
    },
}};
const glyphs = .{
    // GlyphLine array 1
    // GlyphLine array 2
    // GlyphLine array 3
    // GlyphLine array 4
};

pub fn main() !void {
    //
}
