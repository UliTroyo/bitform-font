 I need your help to create a simple CLI in C (for Linux and macOS only) that takes in an ascii string through stdin and then outputs ascii art from an ascii font I made to stdout. This is the specification:

# ASCII Art Generator Technical Specification

## Overview
A command-line interface (CLI) tool for Linux and macOS that converts ASCII text input into ASCII art using custom font glyphs from the Unicode Symbols for Legacy Computing block.

## Input Requirements

### Input Method
- Input MUST be piped in through stdin (e.g., `echo "hi" | ascii-art`)
- Program should exit with an error message if stdin is a TTY (i.e., if input is not piped)
- Direct keyboard input is not supported

### Character Handling
- Maximum input length: 31 characters
- Valid input characters: ASCII codes 32-126 (inclusive)
- Case-insensitive processing (all text converted to uppercase)
- All whitespace sequences collapsed to single spaces
- Invalid characters outside ASCII 32-126 range trigger error and program termination

### Buffer Specifications
- Input buffer: Fixed size based on maximum 31 characters (248 bits)
- Output buffer: Fixed size 2048 bits (accommodates all possible outputs including comments, newlines, and null terminator)
- No dynamic memory allocation required

## Font System

### Glyph Structure
```c
typedef struct {
  char ch;
  const char *lines[4];
  int width; // computed as max length of center two rows
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

### Glyph Characteristics
- Four lines per glyph (top, middle-top, middle-bottom, bottom)
- x-height of 2 characters for most glyphs
- Empty top/bottom lines padded with spaces to match glyph width
- If no glyphs in output extend to first/last line, those lines are omitted

## Comment Handling

### Comment Delimiters
- Single-line: #, //, --, ;
- Multi-line: /*, <!--

### Comment Processing Rules
1. Leading whitespace before comment delimiters is stripped
2. For single-line comments:
   - Delimiter + single space prepended to each output line
   - Delimiter counts toward maximum length
3. For multi-line comments (/* and <!--):
   - First line contains only opening delimiter
   - Last line contains only closing delimiter
   - Any content after closing delimiter (*/ or -->) is ignored
   - Opening/closing delimiters do not count toward maximum length
4. No nested comment handling:
   - Only first comment delimiter is processed
   - After stripping first delimiter, remaining content processed as regular input
   - Additional comment delimiters treated as regular characters

## Output Format
- Lines concatenated with newline (\n) character
- Output sent to stdout as single write operation
- Lines containing only spaces are omitted
- Each line padded to match widest glyph in sequence

## Error Handling
1. Input exceeds 31 characters: Error message about excessive length
2. Invalid ASCII characters: Error message about invalid input
3. TTY input detected: Error message indicating input must be piped
4. No input: Display help message

## CLI Options
- `--help` or `-h`: Display help message and exit
- Help message format:
```
ASCII Art Generator

Usage: echo "text" | ascii-art [options]

Options:
  --help, -h    Show this help message

Input must be piped through stdin and must be ASCII text.
```

## Examples

### Basic Usage
Input: `echo "hi" | ascii-art`
Output:
```
▌ ▌🬂🬕🬀
🬕🬂▌🬭🬲🬏
```

### With Comments
Input: `echo "<!--hI-->ignore" | ascii-art`
Output:
```
<!--
▌ ▌🬂🬕🬀
🬕🬂▌🬭🬲🬏
-->
```

### With Padding
Input: `echo "   #  (HI)    " | ascii-art`
Output:
```
#  🬭       🬞🬏 
# 🬔  ▌ ▌🬂🬕🬀 🬁🬓
# 🬣  🬕🬂▌🬭🬲🬏 🬞🬄
#  🬂       🬁🬀 
```
