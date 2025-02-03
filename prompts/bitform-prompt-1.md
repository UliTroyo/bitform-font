I want to create a simple CLI in C (for Linux and macOS only). It should be able to take a text string containing ascii characters (ignoring case, but accepting lower case) piped through stdin (with the --stdin flag set) or as a file (the name of which are passed in as an argument) and then converting it into any number of lines of text from the strings I gave you, whose data should be embedded into the program at comptime. The lines should be assembled, but checking that no individual line is longer than 80 characters (the length can be customized with the --length flag), wrapping words onto new lines. All lines should be filled with the space character if not filled with any corresponding character data.

The string that will be passed in, whether through stdin or as a file, might start with whitespace characters, or begin with comment symbols from various languages, so <code>\\</code>, <code>#</code>, <code>--</code>, <code>;</code>. All whitespace up to the first non-whitespace character can be ignored, and beyond, any contiguous set of non-newline whitespace characters can be collapsed to a single space character. Whitespace that isn't newlines after the last non-whitespace character in each line in the passed-in text can also be ignored. (So basically, trim the input on both ends.) Newlines represent text breaks, and should be respected. We must note the first set of comment symbols, and put each at the beginning of each output line (followed by a single space character), as this ascii CLI is meant to create text headlines embedded in code comments. So for example, given the following text:
`   # hi    `
we should respond with:
`#       `
`# ▌ ▌🬂🬕🬀`
`# 🬕🬂▌🬭🬲🬏`
`#       `
Note that I included the quotation marks only to demonstrate the strings. The output should include no such marks.

The CLI should also accept --out (or -o) followed by a filename, in which case the output should be redirected to this file. Normally, the output is printed to stdout. Can you help me?
