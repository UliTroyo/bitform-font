#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_LEN 31
#define OUTPUT_BUFFER_SIZE 2048

typedef struct {
  char ch;
  const char *lines[4];
  int width; // width of center two rows (and used for padding)
} Glyph;

/* Sample font data. In a full implementation you’d fill in glyphs for all valid
   ASCII. For demonstration, only a few characters are defined.
*/
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
#define FONT_DATA_SIZE (sizeof(fontData) / sizeof(fontData[0]))

// Look up a glyph for a given character.
const Glyph *findGlyph(char c) {
  for (int i = 0; i < FONT_DATA_SIZE; i++) {
    if (fontData[i].ch == c)
      return &fontData[i];
  }
  return NULL;
}

// Returns nonzero if s is all spaces.
int is_all_spaces(const char *s) {
  for (; *s; s++)
    if (*s != ' ')
      return 0;
  return 1;
}

int main(int argc, char *argv[]) {
  // --help option.
  if (argc > 1 &&
      (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
    printf("ASCII Art Generator\n\nUsage: echo \"text\" | ascii-art "
           "[options]\n\nOptions:\n  --help, -h    Show this help "
           "message\n\nInput must be piped through stdin and must be ASCII "
           "text.\n");
    return 0;
  }

  // Input must be piped (not TTY).
  if (isatty(fileno(stdin))) {
    fprintf(stderr, "Error: Input must be piped through stdin.\n");
    return 1;
  }

  // Read input into a temporary buffer.
  char inputBuffer[128];
  if (!fgets(inputBuffer, sizeof(inputBuffer), stdin)) {
    fprintf(stderr, "Error: No input provided.\n");
    return 1;
  }
  // Remove trailing newline.
  size_t len = strlen(inputBuffer);
  if (len && inputBuffer[len - 1] == '\n')
    inputBuffer[len - 1] = '\0';

  // Collapse whitespace sequences to single spaces.
  char temp[128];
  int j = 0;
  int inSpace = 0;
  for (int i = 0; inputBuffer[i] != '\0'; i++) {
    char c = inputBuffer[i];
    if (c == ' ') {
      if (!inSpace) {
        temp[j++] = ' ';
        inSpace = 1;
      }
    } else {
      temp[j++] = c;
      inSpace = 0;
    }
  }
  temp[j] = '\0';
  // Trim any leading/trailing spaces.
  char *start = temp;
  while (*start == ' ')
    start++;
  char *end = start + strlen(start) - 1;
  while (end > start && *end == ' ') {
    *end = '\0';
    end--;
  }

  // Variables for comment handling.
  int isSingleLineComment = 0, isMultiLineComment = 0;
  const char *singleDelim = NULL;
  const char *multiOpenDelim = NULL, *multiCloseDelim = NULL;

  // Check for multi-line comment delimiters first.
  if (strncmp(start, "<!--", 4) == 0) {
    isMultiLineComment = 1;
    multiOpenDelim = "<!--";
    multiCloseDelim = "-->";
    start += 4; // skip opening delimiter
    char *closePos = strstr(start, multiCloseDelim);
    if (!closePos) {
      fprintf(stderr,
              "Error: Missing closing delimiter for multi-line comment.\n");
      return 1;
    }
    *closePos = '\0'; // terminate art content here
    if (strlen(start) > MAX_INPUT_LEN) {
      fprintf(stderr, "Error: Input exceeds maximum length of %d characters.\n",
              MAX_INPUT_LEN);
      return 1;
    }
    // Convert art content to uppercase.
    for (char *p = start; *p; p++)
      *p = toupper((unsigned char)*p);
  } else if (strncmp(start, "/*", 2) == 0) {
    isMultiLineComment = 1;
    multiOpenDelim = "/*";
    multiCloseDelim = "*/";
    start += 2;
    char *closePos = strstr(start, multiCloseDelim);
    if (!closePos) {
      fprintf(stderr,
              "Error: Missing closing delimiter for multi-line comment.\n");
      return 1;
    }
    *closePos = '\0';
    if (strlen(start) > MAX_INPUT_LEN) {
      fprintf(stderr, "Error: Input exceeds maximum length of %d characters.\n",
              MAX_INPUT_LEN);
      return 1;
    }
    for (char *p = start; *p; p++)
      *p = toupper((unsigned char)*p);
  }
  // Else check for single-line comment delimiters.
  else if (start[0] == '#' || (start[0] == '/' && start[1] == '/') ||
           (start[0] == '-' && start[1] == '-') || start[0] == ';') {
    isSingleLineComment = 1;
    if (start[0] == '#' || start[0] == ';') {
      singleDelim = (start[0] == '#') ? "#" : ";";
      start += 1;
    } else if (start[0] == '/' && start[1] == '/') {
      singleDelim = "//";
      start += 2;
    } else if (start[0] == '-' && start[1] == '-') {
      singleDelim = "--";
      start += 2;
    }
    // For single-line, delimiter counts toward max length.
    if (strlen(start) + strlen(singleDelim) > MAX_INPUT_LEN) {
      fprintf(stderr, "Error: Input exceeds maximum length of %d characters.\n",
              MAX_INPUT_LEN);
      return 1;
    }
    for (char *p = start; *p; p++)
      *p = toupper((unsigned char)*p);
  }
  // Otherwise, process input normally.
  else {
    if (strlen(start) > MAX_INPUT_LEN) {
      fprintf(stderr, "Error: Input exceeds maximum length of %d characters.\n",
              MAX_INPUT_LEN);
      return 1;
    }
    for (char *p = start; *p; p++)
      *p = toupper((unsigned char)*p);
  }

  // Validate allowed ASCII range (32-126).
  for (char *p = start; *p; p++) {
    if ((unsigned char)*p < 32 || (unsigned char)*p > 126) {
      fprintf(stderr, "Error: Invalid character detected.\n");
      return 1;
    }
  }

  // If there's no art content, show the help message.
  if (*start == '\0') {
    printf("ASCII Art Generator\n\nUsage: echo \"text\" | ascii-art "
           "[options]\n\nOptions:\n  --help, -h    Show this help "
           "message\n\nInput must be piped through stdin and must be ASCII "
           "text.\n");
    return 0;
  }

  /* Build the art row by row.
     We use an array of four row buffers.
     For each character, we look up its Glyph; if a glyph’s row is empty,
     we pad it with spaces (using the glyph’s width).
  */
  char artRows[4][256] = {{0}};
  for (int r = 0; r < 4; r++)
    artRows[r][0] = '\0';

  for (int i = 0; start[i] != '\0'; i++) {
    const Glyph *g = findGlyph(start[i]);
    if (!g) {
      fprintf(stderr, "Error: No glyph available for character '%c'.\n",
              start[i]);
      return 1;
    }
    for (int r = 0; r < 4; r++) {
      char buffer[64];
      if (g->lines[r] == NULL || g->lines[r][0] == '\0') {
        memset(buffer, ' ', g->width);
        buffer[g->width] = '\0';
      } else {
        int lenLine = strlen(g->lines[r]);
        strcpy(buffer, g->lines[r]);
        if (lenLine < g->width) {
          memset(buffer + lenLine, ' ', g->width - lenLine);
          buffer[g->width] = '\0';
        }
      }
      strcat(artRows[r], buffer);
    }
  }

  // Omit top (row 0) or bottom (row 3) rows if they contain only spaces.
  int startRow = is_all_spaces(artRows[0]) ? 1 : 0;
  int endRow = is_all_spaces(artRows[3]) ? 2 : 3;

  // Compute maximum art width for padding.
  int maxWidth = 0;
  for (int r = startRow; r <= endRow; r++) {
    int rowLen = strlen(artRows[r]);
    if (rowLen > maxWidth)
      maxWidth = rowLen;
  }

  // Build the final output buffer.
  char outputBuffer[OUTPUT_BUFFER_SIZE];
  outputBuffer[0] = '\0';

  // For multi-line comments, print the opening delimiter on its own line.
  if (isMultiLineComment) {
    strcat(outputBuffer, multiOpenDelim);
    strcat(outputBuffer, "\n");
  }

  // For each output art row, prepend comment prefix (for single-line) and pad
  // the row.
  for (int r = startRow; r <= endRow; r++) {
    char lineBuffer[512];
    if (isSingleLineComment) {
      strcpy(lineBuffer, singleDelim);
      strcat(lineBuffer, " ");
    } else {
      lineBuffer[0] = '\0';
    }
    char padded[512];
    int lenRow = strlen(artRows[r]);
    strcpy(padded, artRows[r]);
    for (int i = lenRow; i < maxWidth; i++)
      padded[i] = ' ';
    padded[maxWidth] = '\0';
    strcat(lineBuffer, padded);
    strcat(lineBuffer, "\n");
    strcat(outputBuffer, lineBuffer);
  }

  // For multi-line comments, add the closing delimiter on its own line.
  if (isMultiLineComment) {
    strcat(outputBuffer, multiCloseDelim);
    strcat(outputBuffer, "\n");
  }

  // Write the entire output in one go.
  fputs(outputBuffer, stdout);
  return 0;
}
