#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_MAX_WIDTH 80
#define MAX_INPUT_SIZE 1024
#define NUM_GLYPH_ROWS 4

typedef struct {
  char ch;
  const char *lines[NUM_GLYPH_ROWS];
  int width; // computed as the max length of the center two rows
} Glyph;

/* Example glyph data. Extend with all ascii characters as needed. */
Glyph fontData[] = {
    {' ', {"", "  ", "  ", ""}, 2},
    {'!', {"", "▌", "🬐", ""}, 1},
    {'"', {"", "🬄🬄", "  ", ""}, 2},
    {'#', {"", "🬵🬵🬏", "🬫🬫🬃", ""}, 3},
    {'$', {" 🬞  ", "🬤🬸🬰 ", "🬢🬷🬭🬄", " 🬁  "}, 4},
    {'%', {"", "🬃🬞🬃", "🬖🬀🬏", ""}, 3},
    {'&', {"", "🬤🬡🬃 ", "🬣🬗🬘🬀", ""}, 4},
    {'\'', {"", "🬞🬄", "  ", ""}, 2},
    {'(', {" 🬭 ", "🬔  ", "🬣  ", " 🬂 "}, 3},
    {')', {"🬞🬏 ", " 🬁🬓", " 🬞🬄", "🬁🬀 "}, 3},
    {'*', {"", "🬢🬣🬃", "🬀🬄🬀", ""}, 3},
    {'+', {"", " 🬓 ", "🬂🬕🬀", ""}, 3},
    {',', {"", "  ", " 🬓", "🬁 "}, 2},
    {'-', {"", "   ", "🬂🬂🬀", ""}, 3},
    {'.', {"", " ", "🬏", ""}, 1},
    {'/', {"", "🬞🬄", "🬔 ", ""}, 2},
    {'0', {"", "🬜🬊🬓", "🬪🬵🬄", ""}, 3},
    {'1', {"", "🬖▌", " ▌", ""}, 2},
    {'2', {"", "🬅🬂🬓", "🬳🬰🬏", ""}, 3},
    {'3', {"", "🬁🬰🬄", "🬢🬭🬄", ""}, 3},
    {'4', {"", "🬦🬧 ", "🬌🬫🬃", ""}, 3},
    {'5', {"", "🬴🬰🬀", "🬢🬭🬄", ""}, 3},
    {'6', {"", "🬖🬡🬀", "🬪🬮🬄", ""}, 3},
    {'7', {"", "🬂🬡🬄", "🬦🬀 ", ""}, 3},
    {'8', {"", "🬤🬰🬃", "🬣🬭🬄", ""}, 3},
    {'9', {"", "🬔🬂🬓", "🬠🬰🬄", ""}, 3},
    {',', {"", "🬃", "🬃", ""}, 1},
    {';', {"", " 🬏", " 🬓", "🬁 "}, 2},
    {'<', {" 🬞 ", "🬖🬀 ", "🬁🬢 ", ""}, 3},
    {'=', {"", "🬋🬋🬃", "🬋🬋🬃", ""}, 3},
    {'>', {"🬞  ", " 🬈🬏", "🬞🬅 ", ""}, 3},
    {'?', {"", "🬅🬡🬃", " 🬐 ", ""}, 3},
    {'@', {"", "🬜🬡🬒🬓", "▌🬣🬣🬄", ""}, 4},
    {'A', {"", "🬜🬊🬓", "▌🬂▌", ""}, 3},
    {'B', {"", "🬕🬂🬓", "🬲🬰🬃", ""}, 3},
    {'C', {"", "🬔🬂🬃", "🬣🬭🬃", ""}, 3},
    {'D', {"", "🬕🬈🬏", "🬲🬵🬄", ""}, 3},
    {'E', {"", "🬕🬂🬀", "🬴🬰🬏", ""}, 3},
    {'F', {"", "🬕🬂🬀", "🬕🬂 ", ""}, 3},
    {'G', {"", "🬜🬂🬀", "🬣🬯🬄", ""}, 3},
    {'H', {"", "▌ ▌", "🬕🬂▌", ""}, 3},
    {'I', {"", "🬂🬕🬀", "🬭🬲🬏", ""}, 3},
    {'J', {"", "🬁🬨🬀", "🬢🬘 ", ""}, 3},
    {'K', {"", "▌🬖🬀", "🬕🬋🬏", ""}, 3},
    {'L', {"", "▌  ", "🬲🬭🬏", ""}, 3},
    {'M', {"", "🬛🬏🬖▌", "▌🬁 ▌", ""}, 4},
    {'N', {"", "🬛🬏▌", "▌🬈▌", ""}, 3},
    {'O', {"", "🬔🬂🬓", "🬣🬭🬄", ""}, 3},
    {'P', {"", "🬕🬂🬓", "▌🬂 ", ""}, 3},
    {'Q', {"", "🬔🬂🬓", "🬣🬗🬐", ""}, 3},
    {'R', {"", "🬕🬂🬓", "▌🬂🬓", ""}, 3},
    {'S', {"", "🬳🬂🬃", "🬢🬰🬃", ""}, 3},
    {'T', {"", "🬂🬕🬀", " ▌ ", ""}, 3},
    {'U', {"", "▌ ▌", "🬣🬭🬄", ""}, 3},
    {'V', {"", "▌ ▌", "🬉🬘 ", ""}, 3},
    {'W', {"", "🬣🬞🬞🬄", "🬁🬔🬔 ", ""}, 4},
    {'X', {"", "🬣🬞🬄", "🬖🬈🬏", ""}, 3},
    {'Y', {"", "🬣🬞🬄", " ▌ ", ""}, 3},
    {'Z', {"", "🬂🬡🬄", "🬵🬮🬏", ""}, 3},
    {'[', {"🬭 ", "▌ ", "▌ ", "🬂 "}, 2},
    {'\\', {"", "🬣 ", "🬁🬓", ""}, 2},
    {']', {"🬞🬏", " ▌", " ▌", "🬁🬀"}, 2},
    {'^', {"", "🬖🬈🬏", "   ", ""}, 3},
    {'_', {"", "   ", "🬭🬭🬏", ""}, 3},
    {'`', {"", "🬁🬃", "  ", ""}, 2},
    {'\{', {" 🬭 ", "▐  ", "🬧  ", " 🬂 "}, 3},
    {'|', {"", "▌", "▌", ""}, 1},
    {'}', {"🬞🬏 ", " ▐ ", " 🬦🬀", "🬁🬀 "}, 3},
    {'~', {"", "🬞🬏🬏", "🬀🬂 ", ""}, 3},
};

const int numGlyphs = sizeof(fontData) / sizeof(Glyph);

/* Returns pointer to Glyph for a given character (assumes uppercase input) */
const Glyph *findGlyph(char ch) {
  for (int i = 0; i < numGlyphs; i++) {
    if (fontData[i].ch == ch)
      return &fontData[i];
  }
  return NULL;
}

/* Helper: pads an empty glyph line with spaces to match width */
const char *getGlyphLine(const Glyph *g, int row) {
  /* if the row is not empty, just return it */
  if (g->lines[row] && strlen(g->lines[row]) > 0)
    return g->lines[row];
  /* otherwise, return a string of spaces; allocate a static buffer */
  static char buf[16];
  if (g->width >= sizeof(buf))
    g->width = sizeof(buf) - 1;
  memset(buf, ' ', g->width);
  buf[g->width] = '\0';
  return buf;
}

/* Trims leading whitespace */
char *ltrim(char *s) {
  while (*s && isspace((unsigned char)*s))
    s++;
  return s;
}

/* Checks if s starts with prefix */
int startsWith(const char *s, const char *prefix) {
  return strncmp(s, prefix, strlen(prefix)) == 0;
}

/* Main entry */
int main(int argc, char *argv[]) {
  int maxWidth = DEFAULT_MAX_WIDTH;
  int helpFlag = 0;

  // Simple command-line flag handling
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      helpFlag = 1;
    } else if (strcmp(argv[i], "--length") == 0 && i + 1 < argc) {
      maxWidth = atoi(argv[++i]);
    }
  }
  if (helpFlag) {
    printf("Usage: asciiart [--length N]\n"
           "Reads an ascii string from stdin and outputs ascii art using a "
           "custom font.\n"
           "Only supports ASCII input; input is converted to uppercase.\n"
           "Special handling: if input starts with comment delimiters (#, //, "
           "--, /*, <!--, ;) then\n"
           "the output will be commented accordingly.\n");
    return 0;
  }

  // Read input from stdin (up to MAX_INPUT_SIZE characters)
  char input[MAX_INPUT_SIZE];
  if (!fgets(input, sizeof(input), stdin)) {
    fprintf(stderr, "Error: failed to read input.\n");
    return 1;
  }

  // Remove trailing newline if present.
  size_t len = strlen(input);
  if (len > 0 && input[len - 1] == '\n')
    input[len - 1] = '\0';

  // Make a copy for processing.
  char procInput[MAX_INPUT_SIZE];
  strcpy(procInput, input);
  char *p = ltrim(procInput);

  // Determine if we’re in a comment mode.
  // Supported comment prefixes: "#", "//", "--", "/*", "<!--", ";"
  enum { NONE, SINGLE, MULTI } commentMode = NONE;
  char commentPrefix[8] = ""; // store the verbatim prefix text
  char commentSuffix[8] = ""; // for multi-line comments

  if (startsWith(p, "#")) {
    commentMode = SINGLE;
    strcpy(commentPrefix, "#");
    p += 1; // skip prefix
  } else if (startsWith(p, "//")) {
    commentMode = SINGLE;
    strcpy(commentPrefix, "//");
    p += 2;
  } else if (startsWith(p, "--")) {
    commentMode = SINGLE;
    strcpy(commentPrefix, "--");
    p += 2;
  } else if (startsWith(p, ";")) {
    commentMode = SINGLE;
    strcpy(commentPrefix, ";");
    p += 1;
  } else if (startsWith(p, "/*")) {
    commentMode = MULTI;
    strcpy(commentPrefix, "/*");
    strcpy(commentSuffix, "*/");
    p += 2;
    // If a closing delimiter exists, only take text up to that point.
    char *end = strstr(p, "*/");
    if (end) {
      *end = '\0';
    }
  } else if (startsWith(p, "<!--")) {
    commentMode = MULTI;
    strcpy(commentPrefix, "<!--");
    strcpy(commentSuffix, "-->");
    p += 4;
    char *end = strstr(p, "-->");
    if (end) {
      *end = '\0';
    }
  }
  // Now collapse contiguous whitespace in the remaining p.
  // We build a new string that for any run of whitespace, outputs one space.
  char collapsed[MAX_INPUT_SIZE];
  int ci = 0;
  int inSpace = 0;
  for (char *c = p; *c; c++) {
    if (isspace((unsigned char)*c)) {
      if (!inSpace) {
        collapsed[ci++] = ' ';
        inSpace = 1;
      }
    } else {
      collapsed[ci++] = toupper((unsigned char)*c); // also uppercase
      inSpace = 0;
    }
  }
  collapsed[ci] = '\0';

  // Now build an array of pointers representing the glyphs to output.
  // We treat a space character as a glyph if it appears in the collapsed
  // string. (Any contiguous whitespace in the input already became a single
  // space.)
  int numChars = strlen(collapsed);
  // For each glyph, we’ll later need to know its width for spacing.
  // We also build the output lines row by row.
  char *rows[NUM_GLYPH_ROWS];
  // allocate buffers that can hold the max width (plus comment prefixes) – use
  // dynamic allocation.
  for (int i = 0; i < NUM_GLYPH_ROWS; i++) {
    rows[i] = calloc(maxWidth + 64, sizeof(char));
    if (!rows[i]) {
      fprintf(stderr, "Allocation error.\n");
      return 1;
    }
    rows[i][0] = '\0';
  }

  // For each character in the collapsed string, look up its glyph and append
  // its rows. For clarity, we insert a single space between glyphs (except
  // before the first glyph).
  int firstGlyph = 1;
  for (int idx = 0; idx < numChars; idx++) {
    char ch = collapsed[idx];
    const Glyph *g = findGlyph(ch);
    if (!g) {
      fprintf(stderr, "Error: No glyph for character '%c'.\n", ch);
      return 1;
    }
    // if not the first glyph, append a single space to each row
    if (!firstGlyph) {
      for (int r = 0; r < NUM_GLYPH_ROWS; r++) {
        strcat(rows[r], " ");
      }
    }
    firstGlyph = 0;
    // For each row, get the glyph line (pad if necessary) and append.
    for (int r = 0; r < NUM_GLYPH_ROWS; r++) {
      const char *line = getGlyphLine(g, r);
      strcat(rows[r], line);
    }
  }

  // Remove top row if it is all spaces.
  int startRow = 0, endRow = NUM_GLYPH_ROWS - 1;
  // Check first row:
  int allSpaces = 1;
  for (char *c = rows[0]; *c; c++) {
    if (*c != ' ') {
      allSpaces = 0;
      break;
    }
  }
  if (allSpaces)
    startRow = 1;
  // Check last row:
  allSpaces = 1;
  for (char *c = rows[NUM_GLYPH_ROWS - 1]; *c; c++) {
    if (*c != ' ') {
      allSpaces = 0;
      break;
    }
  }
  if (allSpaces)
    endRow = NUM_GLYPH_ROWS - 2;

  // Compute total width (assumes all rows have same length)
  int outWidth = strlen(rows[startRow]);
  if (outWidth > maxWidth) {
    fprintf(stderr, "Error: Output width (%d) exceeds maximum allowed (%d).\n",
            outWidth, maxWidth);
    return 1;
  }

  // Build final output. In comment mode, we need to add the comment prefix.
  // For MULTI comment mode, output 6 lines: first line is the opening
  // delimiter, then the art lines, then the closing delimiter.
  if (commentMode == MULTI) {
    printf("%s\n", commentPrefix);
    for (int r = startRow; r <= endRow; r++) {
      // Skip rows that are entirely spaces.
      int onlySpaces = 1;
      for (char *c = rows[r]; *c; c++) {
        if (*c != ' ') {
          onlySpaces = 0;
          break;
        }
      }
      if (!onlySpaces)
        printf("%s\n", rows[r]);
    }
    printf("%s\n", commentSuffix);
  } else if (commentMode == SINGLE) {
    for (int r = startRow; r <= endRow; r++) {
      // Prepend the comment delimiter and a space.
      printf("%s %s\n", commentPrefix, rows[r]);
    }
  } else {
    for (int r = startRow; r <= endRow; r++) {
      printf("%s\n", rows[r]);
    }
  }

  // free allocated memory
  for (int i = 0; i < NUM_GLYPH_ROWS; i++) {
    free(rows[i]);
  }
  return 0;
}
