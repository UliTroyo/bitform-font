 I want to create a simple CLI in C (for Linux and macOS only) that takes in an ascii string through stdin and then outputs ascii art from an ascii font I made to stdout.  The CLI should accept only ascii text, and should be case-insensitive, but convert all text to upper case, which is the only case my font supports. My font uses the Unicode block sextant mosaic characters from the Symbols for Legacy Computing block. The x-height of most glyphs is 2 characters tall. I already have glyph data for my font; this is what it looks like:

```c
typedef struct {
  char ch;
  const char *lines[4];
  int width; // computed as the max length of the center two rows
} Glyph;

Glyph fontData[] = {
    {'#', {"", "🬵🬵🬏", "🬫🬫🬃", ""}, 3},
    {'(', {" 🬭 ", "🬔  ", "🬣  ", " 🬂 "}, 3},
    {')', {"🬞🬏 ", " 🬁🬓", " 🬞🬄", "🬁🬀 "}, 3},
    {'H', {"", "▌ ▌", "🬕🬂▌", ""}, 3},
    {'I', {"", "🬂🬕🬀", "🬭🬲🬏", ""}, 3},
    ... and so on, for all ascii characters
};
```
Notice that though all glyph data contains four lines, the top row and bottom row of characters are the empty string. When printing, these should be padded using ` ` to the width of the glyph (so for example, 1 ` ` character for the "!" glyph, and 3 for the "#").

- The lines to be output should be assembled from my glyph data, from the uppercased input string.
- All whitespace between glyphs should be replaced by a single ` ` before processing.
- All output lines should be concatenated with a newline `\n` and output all at once.

- The maximum string input is 31 characters total. Because this input is ascii, this means the input should never exceed 31 * 8 bits. If the input exceeds this amount, we should circuit break and output an error message saying the input is too long. This also means our input buffer can also be of predetermined size, and we don't need to allocate memory for more.

- I have precalculated that because of the maximum string input length and because of the baximum glyph size in bits, we will never exceed 2048 bits for our output string. This means we should use a 2048 buffer rather than allocate using malloc to process our output. Just trust me on this.

Before output, if no glyph extends into the first row, rather than outputting a row of spaces, the entire line should not be output (we should start from the second line instead). Likewise, if no glyph extends into the last row, the entire line should not be output.

There's an important rule, though: if the stdin string starts with the comment delimiters `#`, `//`, `--`, `/*`, `<!--` or `;` (even if preceded by whitespace), these should NOT count as output glyphs, but instead should verbatim be the first character in each of the 4 lines followed by a single space, as this means that this text is intended as commented text to be copy-pasted into a text editor. In the case of `/*` and `<!--`, if a corresponding `*/` or `-->` exists, it (and any text after it) should be ignored, and instead of prepending the delimiter to each line, we should instead output `/*` or `<!--` to stdout, then the 4 lines, then the corresponding comment closing delimiter, for a total of 6 lines in this case. Of course, as previously stated, any lines that only contain spaces should be skipped, rather than outputting a line with the comment delimiter followed by spaces. Comment delimiters in rows that contain glyphs (so excluding those for `/*` and `<!--` and their counterpart) count toward the maximum length. Our 2048-bit buffer already takes into account all newlines, the null terminator, and possible comment delimiter length.

*Example 1:*
Input:
`hi`
Output:
`▌ ▌🬂🬕🬀`
`🬕🬂▌🬭🬲🬏`
Total width: 6ch
Description: Even though our input string is lowercase, our output is our uppercase glyphs.

*Example 2:*
Input:
`   #  (HI)    `
Output:
`#  🬭       🬞🬏 `
`# 🬔  ▌ ▌🬂🬕🬀 🬁🬓`
`# 🬣  🬕🬂▌🬭🬲🬏 🬞🬄`
`#  🬂       🬁🬀 `
Total width: 14ch
Description: Whitespace before the `#` is ignored, whitespace after `#` is collapsed into a single space. This time, although `HI` is identical to the previous example, because `()` glyphs extend into the first and last line, the output is rows tall.

*Example 3:*
Input:
`<!--hI-->ignore`
Output:
`<!--`
`▌ ▌🬂🬕🬀`
`🬕🬂▌🬭🬲🬏`
`-->`
Total width: 6ch
Description: The input `hI` should be identical to the first example, but because of the `<!--` delimiter, the first line is `<!--` and the last line is `-->`. From the input, everything after `-->` is also ignored, so the string part `ignore` is not transformed into glyphs.

*Example 4:*
Input:
`##hi`
Output:
`# 🬵🬵🬏▌ ▌🬂🬕🬀`
`# 🬫🬫🬃🬕🬂▌🬭🬲🬏`
Total width: 11ch
Description: The input string starts with a comment delimiter `#`, so our output contains that delimiter at the start of both output lines followed by a space character. The second character is also `#`, but this one DOES get turned into its glyph representation.

Passing in the `--help` or `-h` flag to the CLI should predictably short-circuit any other operation and print out well-formatted help output like the following:
```
ASCII Art Generator

Usage: ascii-art [options]

Options:
  --help, -h    Show this help message

Input is read from stdin and must be ASCII text.
```

I don't need code output right now, I just want to know if any steps are unclear for you.
