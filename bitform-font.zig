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

const fontdata = [69][4][]const u8{
    // ' '
    [4][]const u8{
        "", //
        "  ", //
        "  ", //
        "", //
    },
    // !
    [4][]const u8{
        "", //
        "▌", //
        "🬐", //
        "", //
    },
    // "
    [4][]const u8{
        "", //
        "🬄🬄", //
        "  ", //
        "", //
    },
    // #
    [4][]const u8{
        "", //
        "🬵🬵🬏", //
        "🬫🬫🬃", //
        "", //
    },
    // $
    [4][]const u8{
        " 🬞  ", //
        "🬤🬸🬰 ", //
        "🬢🬷🬭🬄", //
        " 🬁  ", //
    },
    // %
    [4][]const u8{
        "", //
        "🬃🬞🬃", //
        "🬖🬀🬏", //
        "", //
    },
    // &
    [4][]const u8{
        "", //
        "🬤🬡🬃 ", //
        "🬣🬗🬘🬀", //
        "", //
    },
    // '
    [4][]const u8{
        "", //
        "🬞🬄", //
        "  ", //
        "", //
    },
    // (
    [4][]const u8{
        " 🬭 ", //
        "🬔  ", //
        "🬣  ", //
        " 🬂 ", //
    },
    // )
    [4][]const u8{
        "🬞🬏 ", //
        " 🬁🬓", //
        " 🬞🬄", //
        "🬁🬀 ", //
    },
    // *
    [4][]const u8{
        "", //
        "🬢🬣🬃", //
        "🬀🬄🬀", //
        "", //
    },
    // +
    [4][]const u8{
        "", //
        " 🬓 ", //
        "🬂🬕🬀", //
        "", //
    },
    // ,
    [4][]const u8{
        "", //
        "  ", //
        " 🬓", //
        "🬁 ", //
    },
    // -
    [4][]const u8{
        "", //
        "   ", //
        "🬂🬂🬀", //
        "", //
    },
    // .
    [4][]const u8{
        "", //
        " ", //
        "🬏", //
        "", //
    },
    // /
    [4][]const u8{
        "", //
        "🬞🬄", //
        "🬔 ", //
        "", //
    },
    // 0
    [4][]const u8{
        "", //
        "🬜🬊🬓", //
        "🬪🬵🬄", //
        "", //
    },
    // 1
    [4][]const u8{
        "", //
        "🬖▌", //
        " ▌", //
        "", //
    },
    // 2
    [4][]const u8{
        "", //
        "🬅🬂🬓", //
        "🬳🬰🬏", //
        "", //
    },
    // 3
    [4][]const u8{
        "", //
        "🬁🬰🬄", //
        "🬢🬭🬄", //
        "", //
    },
    // 4
    [4][]const u8{
        "", //
        "🬦🬧 ", //
        "🬌🬫🬃", //
        "", //
    },
    // 5
    [4][]const u8{
        "", //
        "🬴🬰🬀", //
        "🬢🬭🬄", //
        "", //
    },
    // 6
    [4][]const u8{
        "", //
        "🬖🬡🬀", //
        "🬪🬮🬄", //
        "", //
    },
    // 7
    [4][]const u8{
        "", //
        "🬂🬡🬄", //
        "🬦🬀 ", //
        "", //
    },
    // 8
    [4][]const u8{
        "", //
        "🬤🬰🬃", //
        "🬣🬭🬄", //
        "", //
    },
    // 9
    [4][]const u8{
        "", //
        "🬔🬂🬓", //
        "🬠🬰🬄", //
        "", //
    },
    // ,
    [4][]const u8{
        "", //
        "🬃", //
        "🬃", //
        "", //
    },
    // ;
    [4][]const u8{
        "", //
        " 🬏", //
        " 🬓", //
        "🬁 ", //
    },
    // <
    [4][]const u8{
        " 🬞 ", //
        "🬖🬀 ", //
        "🬁🬢 ", //
        "", //
    },
    // =
    [4][]const u8{
        "", //
        "🬋🬋🬃", //
        "🬋🬋🬃", //
        "", //
    },
    // >
    [4][]const u8{
        "🬞  ", //
        " 🬈🬏", //
        "🬞🬅 ", //
        "", //
    },
    // ?
    [4][]const u8{
        "", //
        "🬅🬡🬃", //
        " 🬐 ", //
        "", //
    },
    // @
    [4][]const u8{
        "", //
        "🬜🬡🬒🬓", //
        "▌🬣🬣🬄", //
        "", //
    },
    // A
    [4][]const u8{
        "", //
        "🬜🬊🬓", //
        "▌🬂▌", //
        "", //
    },
    // B
    [4][]const u8{
        "", //
        "🬕🬂🬓", //
        "🬲🬰🬃", //
        "", //
    },
    // C
    [4][]const u8{
        "", //
        "🬔🬂🬃", //
        "🬣🬭🬃", //
        "", //
    },
    // D
    [4][]const u8{
        "", //
        "🬕🬈🬏", //
        "🬲🬵🬄", //
        "", //
    },
    // E
    [4][]const u8{
        "", //
        "🬕🬂🬀", //
        "🬴🬰🬏", //
        "", //
    },
    // F
    [4][]const u8{
        "", //
        "🬕🬂🬀", //
        "🬕🬂 ", //
        "", //
    },
    // G
    [4][]const u8{
        "", //
        "🬜🬂🬀", //
        "🬣🬯🬄", //
        "", //
    },
    // H
    [4][]const u8{
        "", //
        "▌ ▌", //
        "🬕🬂▌", //
        "", //
    },
    // I
    [4][]const u8{
        "", //
        "🬂🬕🬀", //
        "🬭🬲🬏", //
        "", //
    },
    // J
    [4][]const u8{
        "", //
        "🬁🬨🬀", //
        "🬢🬘 ", //
        "", //
    },
    // K
    [4][]const u8{
        "", //
        "▌🬖🬀", //
        "🬕🬋🬏", //
        "", //
    },
    // L
    [4][]const u8{
        "", //
        "▌  ", //
        "🬲🬭🬏", //
        "", //
    },
    // M
    [4][]const u8{
        "", //
        "🬛🬏🬖▌", //
        "▌🬁 ▌", //
        "", //
    },
    // N
    [4][]const u8{
        "", //
        "🬛🬏▌", //
        "▌🬈▌", //
        "", //
    },
    // O
    [4][]const u8{
        "", //
        "🬔🬂🬓", //
        "🬣🬭🬄", //
        "", //
    },
    // P
    [4][]const u8{
        "", //
        "🬕🬂🬓", //
        "▌🬂 ", //
        "", //
    },
    // Q
    [4][]const u8{
        "", //
        "🬔🬂🬓", //
        "🬣🬗🬐", //
        "", //
    },
    // R
    [4][]const u8{
        "", //
        "🬕🬂🬓", //
        "▌🬂🬓", //
        "", //
    },
    // S
    [4][]const u8{
        "", //
        "🬳🬂🬃", //
        "🬢🬰🬃", //
        "", //
    },
    // T
    [4][]const u8{
        "", //
        "🬂🬕🬀", //
        " ▌ ", //
        "", //
    },
    // U
    [4][]const u8{
        "", //
        "▌ ▌", //
        "🬣🬭🬄", //
        "", //
    },
    // V
    [4][]const u8{
        "", //
        "▌ ▌", //
        "🬉🬘 ", //
        "", //
    },
    // W
    [4][]const u8{
        "", //
        "🬣🬞🬞🬄", //
        "🬁🬔🬔 ", //
        "", //
    },
    // X
    [4][]const u8{
        "", //
        "🬣🬞🬄", //
        "🬖🬈🬏", //
        "", //
    },
    // Y
    [4][]const u8{
        "", //
        "🬣🬞🬄", //
        " ▌ ", //
        "", //
    },
    // Z
    [4][]const u8{
        "", //
        "🬂🬡🬄", //
        "🬵🬮🬏", //
        "", //
    },
    // [
    [4][]const u8{
        "🬭 ", //
        "▌ ", //
        "▌ ", //
        "🬂 ", //
    },
    // \
    [4][]const u8{
        "", //
        "🬣 ", //
        "🬁🬓", //
        "", //
    },
    // ]
    [4][]const u8{
        "🬞🬏", //
        " ▌", //
        " ▌", //
        "🬁🬀", //
    },
    // ^
    [4][]const u8{
        "", //
        "🬖🬈🬏", //
        "   ", //
        "", //
    },
    // _
    [4][]const u8{
        "", //
        "   ", //
        "🬭🬭🬏", //
        "", //
    },
    // `
    [4][]const u8{
        "", //
        "🬁🬃", //
        "  ", //
        "", //
    },
    // {
    [4][]const u8{
        " 🬭 ", //
        "▐  ", //
        "🬧  ", //
        " 🬂 ", //
    },
    // |
    [4][]const u8{
        "", //
        "▌", //
        "▌", //
        "", //
    },
    // }
    [4][]const u8{
        "🬞🬏 ", //
        " ▐ ", //
        " 🬦🬀", //
        "🬁🬀 ", //
    },
    // ~
    [4][]const u8{
        "", //
        "🬞🬏🬏", //
        "🬀🬂 ", //
        "", //
    },
};
