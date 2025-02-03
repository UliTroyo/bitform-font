const std = @import("std");

/// A Glyph is our ascii-font entry.
const Glyph = struct {
    ch: u8,
    /// The four “lines” (as slices) that make up the glyph.
    lines: [4][]const u8,
    /// Precomputed width (from the center two lines).
    width: usize,
};

/// Our embedded font data. (Only a few entries shown.)
/// You would include the full set from your JSON.
const fontData: []const Glyph = &[_]Glyph{
    // space
    Glyph{ .ch = ' ', .lines = [_][]const u8{ "" , "  ", "  ", "" }, .width = 2 },
    // exclamation mark; note we ignore bit_flag etc. here because we only need the lines and width.
    Glyph{ .ch = '!', .lines = [_][]const u8{ "" , "▌", "🬐", "" }, .width = 1 },
    // quotation marks (we store the glyph as provided, but in our program we map letters to uppercase)
    Glyph{ .ch = '"', .lines = [_][]const u8{ "" , "🬄🬄", "  ", "" }, .width = 2 },
    // Letter A – we assume our input is case-insensitive and map to uppercase.
    Glyph{ .ch = 'A', .lines = [_][]const u8{ "" , "🬜🬊🬓", "▌🬂▌", "" }, .width = 3 },
    // Letter H
    Glyph{ .ch = 'H', .lines = [_][]const u8{ "" , "▌ ▌", "🬕🬂▌", "" }, .width = 3 },
    // Letter I
    Glyph{ .ch = 'I', .lines = [_][]const u8{ "" , "🬂🬕🬀", "🬭🬲🬏", "" }, .width = 3 },
    // … add the rest of your glyphs here …
};

/// Returns a pointer to the glyph for the given character.
/// If not found, return the space glyph.
fn getGlyph(ch: u8) *const Glyph {
    // Convert to uppercase if letter.
    var uc = ch;
    if (ch >= 'a' and ch <= 'z') {
        uc = ch - 32;
    }
    for (fontData) |glyph| {
        if (glyph.ch == uc) return glyph;
    }
    // default to space if unknown:
    for (fontData) |glyph| {
        if (glyph.ch == ' ') return glyph;
    }
    @panic("space glyph not found");
}

/// Helper: given a string slice, trim leading/trailing whitespace and collapse
/// contiguous non-newline whitespace to a single space.
fn normalizeLine(line: []const u8, allocator: *std.mem.Allocator) ![]u8 {
    var trimmed = std.mem.trim(u8, line, " \t\r");
    // We use a simple algorithm: split on std.isSpace but preserve newlines? 
    // Here each “line” already comes from splitting input on newline.
    // So we collapse spaces inside the line.
    var parts = std.mem.split(trimmed, " \t");
    var outStream = std.ArrayList(u8).init(allocator);
    defer outStream.deinit();
    var first = true;
    for (parts) |part| {
        if (part.len == 0) continue;
        if (!first) try outStream.append(' ');
        try outStream.appendSlice(part);
        first = false;
    }
    return outStream.toOwnedSlice();
}

/// Given the whole input, detect if the first non‐whitespace token is a comment marker.
/// We support comment markers: "#" , "--" , ";" , and "\\".
fn detectCommentPrefix(input: []const u8) []const u8 {
    const trimmed = std.mem.trim(u8, input, " \t\r\n");
    if (std.mem.startsWith(u8, trimmed, "--")) return "--";
    if (trimmed.len > 0) {
        const first = trimmed[0];
        if (first == '#' or first == ';' or first == '\\')
            // We return just the single character.
            return trimmed[0..1];
    }
    return "";
}

/// Build the ascii art for a word. Returns an array of 4 strings (one per glyph row)
/// allocated from the given allocator.
fn buildWordArt(word: []const u8, allocator: *std.mem.Allocator) ![4][]u8 {
    // We know each glyph is 4 lines.
    // We will concatenate the corresponding glyph line for each character.
    var art: [4]std.ArrayList(u8) = .{
        std.ArrayList(u8).init(allocator),
        std.ArrayList(u8).init(allocator),
        std.ArrayList(u8).init(allocator),
        std.ArrayList(u8).init(allocator),
    };
    // Ensure we free these if error occurs later.

    for (word) |c| {
        const glyph = getGlyph(c);
        for (art, 0..) |list, art_index| {
            try list.appendSlice(glyph.lines[art_index]);
        }
    }
    var result: [4][]u8 = undefined;
    for (art, 0..) |list, i| {
        result[i] = list.toOwnedSlice();
    }
    return result;
}

/// Assemble a “line” (a block of 4 output rows) from a list of word arts,
/// inserting a space between words (using the space glyph).
fn assembleLine(wordArts: [][][4]u8, maxWidth: usize, allocator: *std.mem.Allocator) ![4][]u8 {
    // We start with 4 arraylists.
    var lineParts: [4]std.ArrayList(u8) = .{
        std.ArrayList(u8).init(allocator),
        std.ArrayList(u8).init(allocator),
        std.ArrayList(u8).init(allocator),
        std.ArrayList(u8).init(allocator),
    };

    // For the inter-word space we use the space glyph.
    const spaceGlyph = getGlyph(' ');

    for (wordArts,0..) |wordArt, wi| {
        if (wi != 0) {
            // Append space glyph between words.
            for (lineParts, 0..) |list, i| {
                try list.appendSlice(spaceGlyph.lines[i]);
            }
        }
        for (lineParts, 0..) |list, i| {
            try list.appendSlice(wordArt[i]);
        }
    }
    // Now pad each line to exactly maxWidth.
    var out: [4][]u8 = undefined;
    for (lineParts, 0..) |list, i| {
        // Compute current width in runes. For simplicity we assume each appended slice
        // is measured in bytes (works if all codepoints are one column wide).
        // In practice you might want to compute display width.
        const curWidth = list.items.len;
        if (curWidth < maxWidth) {
            const padCount = maxWidth - curWidth;
            for (padCount) |_| {
                try list.append(' ');
            }
        }
        out[i] = list.toOwnedSlice();
    }
    return out;
}

/// Main: parses arguments, reads input (stdin or file), normalizes it,
/// then assembles the ascii art and outputs it.
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer std.debug.assert(!gpa.deinit());
    const allocator = &gpa.allocator;

    const args = std.process.args();
    var stdinFlag = false;
    var outFilename: ?[]const u8 = null;
    var maxLineWidth: usize = 80;
    var fileInputs: []const u8 = "";
    // A simple argument parser:
    var i: usize = 1;
    while (i < args.len) : (i += 1) {
        const arg = args[i];
        if (std.mem.eql(u8, arg, "--stdin")) {
            stdinFlag = true;
        } else if (std.mem.eql(u8, arg, "--length")) {
            i += 1;
            if (i >= args.len) {
                writeError("Missing value for --length");
            }
            const lenStr = args[i];
            maxLineWidth = try std.fmt.parseInt(usize, lenStr, 10);
        } else if (std.mem.eql(u8, arg, "--out") or std.mem.eql(u8, arg, "-o")) {
            i += 1;
            if (i >= args.len) return writeError("Missing value for --out");
            outFilename = args[i];
        } else {
            // Assume any non-flag argument is a filename.
            fileInputs = arg;
        }
    }

    var inputText: []u8 = undefined;
    if (stdinFlag) {
        // Read from stdin.
        inputText = try std.io.getStdIn().readAllAlloc(allocator, 8192);
    } else if (fileInputs.len != 0) {
        // Read from file.
        const file = try std.fs.cwd().openFile(fileInputs, .{});
        inputText = try file.readToEndAlloc(allocator, 8192);
    } else {
        return writeError("No input specified. Use --stdin or provide a filename.");
    }

    // Detect a comment prefix from the whole input.
    const commentPrefix = detectCommentPrefix(inputText);
    // Now split input into lines (by newline) and process each.
    var lines = std.mem.split(u8, inputText, "\n");

    // We will collect the final output lines (each is 4 rows) in an array list.
    var outLines = std.ArrayList([][]u8).init(allocator); // each element will be one output row (string)
    // We must output 4 rows per block.
    // For each input line, we need to:
    //   - trim and collapse whitespace (using normalizeLine)
    //   - split into words (on space)
    //   - build ascii art for each word (buildWordArt)
    //   - assemble words into output lines ensuring not to exceed maxLineWidth.
    for (lines) |lineRaw| {
        // Normalize the line.
        const normLine = try normalizeLine(lineRaw, allocator);
        if (normLine.len == 0) continue; // skip blank lines

        // Split the normalized line into words.
        const words = std.mem.split(u8, normLine, " ");
        // Prepare a list of word arts.
        var wordArts = std.ArrayList([4][]u8).init(allocator);
        defer wordArts.deinit();
        for (words) |word| {
            // For each character in the word, map to a glyph.
            // (We ignore case by converting to uppercase in getGlyph.)
            const art = try buildWordArt(word, allocator);
            try wordArts.append(art);
        }

        // Now assemble words into output blocks.
        // We'll collect a list of word arts into a line until adding the next word would exceed maxLineWidth.
        var currentLineWordArts = std.ArrayList([4][]u8).init(allocator);
        defer currentLineWordArts.deinit();

        // Function to compute current assembled width.
        inline fn currentWidth(lineArts: *std.ArrayList([4][]u8)) usize {
            var sum: usize = 0;
            for (lineArts.items) |wordArt, idx| {
                if (idx != 0) {
                    // add space glyph width.
                    sum += getGlyph(' ').width;
                }
                // Assume the width of a word art is the length of its second row.
                sum += wordArt[1].len;
            }
            return sum;
        }

        for (wordArts.items) |wordArt| {
            const w = {
                // Use second row’s length (all rows should have the same width).
                wordArt[1].len,
            };
            // If current line is empty, simply add.
            if (currentLineWordArts.items.len == 0) {
                try currentLineWordArts.append(wordArt);
            } else {
                // Check if adding space + this word would exceed maxLineWidth.
                const extra = getGlyph(' ').width + w;
                if (currentWidth(&currentLineWordArts) + extra > maxLineWidth) {
                    // Assemble current line.
                    const artLines = try assembleLine(currentLineWordArts.items, maxLineWidth, allocator);
                    // Prepend each line with the comment prefix and a space.
                    for (artLines) |lineSlice| {
                        var buf = try allocator.alloc(u8, commentPrefix.len + 1 + lineSlice.len);
                        std.mem.copy(u8, buf, commentPrefix);
                        buf[commentPrefix.len] = ' ';
                        std.mem.copy(u8, buf[(commentPrefix.len + 1)..], lineSlice);
                        try outLines.append(buf);
                    }
                    currentLineWordArts.clear();
                    try currentLineWordArts.append(wordArt);
                } else {
                    try currentLineWordArts.append(wordArt);
                }
            }
        }
        // Assemble any remaining word arts.
        if (currentLineWordArts.items.len != 0) {
            const artLines = try assembleLine(currentLineWordArts.items, maxLineWidth, allocator);
            for (artLines) |lineSlice| {
                var buf = try allocator.alloc(u8, commentPrefix.len + 1 + lineSlice.len);
                std.mem.copy(u8, buf, commentPrefix);
                buf[commentPrefix.len] = ' ';
                std.mem.copy(u8, buf[(commentPrefix.len + 1)..], lineSlice);
                try outLines.append(buf);
            }
        }
    }

    // Write output to file if specified, else to stdout.
    if (outFilename) |fname| {
        const file = try std.fs.cwd().createFile(fname, .{ .truncate = true });
        var outStream = file.writer();
        for (outLines.items) |line| {
            try outStream.writeAll(line);
            try outStream.writeAll("\n");
        }
    } else {
        var stdout = std.io.getStdOut().writer();
        for (outLines.items) |line| {
            try stdout.writeAll(line);
            try stdout.writeAll("\n");
        }
    }
}

/// Helper to return an error.
fn writeError(msg: []const u8) !void {
    std.debug.print("Error: {}\n", .{msg});
    return error.GeneralFailure;
}

