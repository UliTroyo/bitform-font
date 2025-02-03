#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Font data ------------------------------------------------

typedef struct {
  char ch;
  const char *lines[4];
  int width; // computed as the max length of the center two rows
} Glyph;

// (For brevity only a subset is shown; add the rest as needed)
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

#define FONT_DATA_COUNT (sizeof(fontData) / sizeof(Glyph))

Glyph *getGlyph(char c) {
  for (int i = 0; i < FONT_DATA_COUNT; i++) {
    if (fontData[i].ch == c)
      return &fontData[i];
  }
  return NULL;
}

// --- Helper: create a string of spaces of given length
char *make_padding(int width) {
  char *pad = malloc(width + 1);
  if (!pad) {
    perror("malloc");
    exit(1);
  }
  memset(pad, ' ', width);
  pad[width] = '\0';
  return pad;
}

// --- Main program ------------------------------------------------

int main(int argc, char **argv) {
  int maxWidth = 80; // default maximum width
  bool show_help = false;

  // Process command-line arguments.
  static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                         {"length", required_argument, 0, 'l'},
                                         {0, 0, 0, 0}};
  int opt;
  while ((opt = getopt_long(argc, argv, "hl:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      show_help = true;
      break;
    case 'l':
      maxWidth = atoi(optarg);
      break;
    default:
      show_help = true;
      break;
    }
  }
  if (show_help) {
    printf("Usage: %s [--length N] [--help]\n", argv[0]);
    printf("Reads ASCII text from stdin and outputs ASCII art using a custom "
           "font.\n");
    return 0;
  }

  // Read input from stdin.
  char input[1024];
  if (!fgets(input, sizeof(input), stdin)) {
    fprintf(stderr, "Error: no input provided.\n");
    return 1;
  }
  // Remove newline if present.
  input[strcspn(input, "\n")] = '\0';

  // Ensure all characters are ASCII.
  for (char *p = input; *p; p++) {
    if ((unsigned char)*p > 127) {
      fprintf(stderr, "Error: non-ASCII character encountered.\n");
      return 1;
    }
  }

  // Convert input to uppercase.
  for (char *p = input; *p; p++) {
    *p = toupper((unsigned char)*p);
  }

  // --- Comment mode detection ---
  bool commentMode = false;  // if any comment delimiter is detected
  bool blockComment = false; // for /* or <!-- style with closing delimiter
  const char *simplePrefix = NULL; // for simple comment mode
  char blockOpen[10] = {0}, blockClose[10] = {0};
  char *renderText = input;

  // Skip any leading whitespace.
  while (*renderText && isspace((unsigned char)*renderText))
    renderText++;

  // Check for comment delimiters. (Check longer ones first.)
  if (strncmp(renderText, "<!--", 4) == 0) {
    commentMode = true;
    // Look for closing delimiter "-->"
    char *closing = strstr(renderText, "-->");
    if (closing) {
      blockComment = true;
      strcpy(blockOpen, "<!--");
      strcpy(blockClose, "-->");
      renderText += 4;
      while (*renderText && isspace((unsigned char)*renderText))
        renderText++;
      // Only render the text between the delimiters.
      int textLen = closing - renderText;
      char *temp = malloc(textLen + 1);
      if (!temp) {
        perror("malloc");
        exit(1);
      }
      strncpy(temp, renderText, textLen);
      temp[textLen] = '\0';
      renderText = temp;
    } else {
      // No closing found: treat as simple comment.
      simplePrefix = "<!--";
      renderText += 4;
      while (*renderText && isspace((unsigned char)*renderText))
        renderText++;
    }
  } else if (strncmp(renderText, "/*", 2) == 0) {
    commentMode = true;
    char *closing = strstr(renderText, "*/");
    if (closing) {
      blockComment = true;
      strcpy(blockOpen, "/*");
      strcpy(blockClose, "*/");
      renderText += 2;
      while (*renderText && isspace((unsigned char)*renderText))
        renderText++;
      int textLen = closing - renderText;
      char *temp = malloc(textLen + 1);
      if (!temp) {
        perror("malloc");
        exit(1);
      }
      strncpy(temp, renderText, textLen);
      temp[textLen] = '\0';
      renderText = temp;
    } else {
      simplePrefix = "/*";
      renderText += 2;
      while (*renderText && isspace((unsigned char)*renderText))
        renderText++;
      commentMode = true;
    }
  } else if (strncmp(renderText, "//", 2) == 0) {
    commentMode = true;
    simplePrefix = "//";
    renderText += 2;
    while (*renderText && isspace((unsigned char)*renderText))
      renderText++;
  } else if (strncmp(renderText, "--", 2) == 0) {
    commentMode = true;
    simplePrefix = "--";
    renderText += 2;
    while (*renderText && isspace((unsigned char)*renderText))
      renderText++;
  } else if (*renderText == '#') {
    commentMode = true;
    simplePrefix = "#";
    renderText++;
    while (*renderText && isspace((unsigned char)*renderText))
      renderText++;
  } else if (*renderText == ';') {
    commentMode = true;
    simplePrefix = ";";
    renderText++;
    while (*renderText && isspace((unsigned char)*renderText))
      renderText++;
  }
  // If not comment mode, renderText remains the whole input.

  // --- Collapse internal whitespace ---
  char *processed = malloc(strlen(renderText) + 1);
  if (!processed) {
    perror("malloc");
    exit(1);
  }
  char *dst = processed;
  bool inWhitespace = false;
  for (char *p = renderText; *p; p++) {
    if (isspace((unsigned char)*p)) {
      if (!inWhitespace) {
        *dst++ = ' ';
        inWhitespace = true;
      }
    } else {
      *dst++ = *p;
      inWhitespace = false;
    }
  }
  *dst = '\0';
  // Trim possible leading/trailing space.
  while (*processed == ' ')
    memmove(processed, processed + 1, strlen(processed));
  int plen = strlen(processed);
  while (plen > 0 && processed[plen - 1] == ' ') {
    processed[plen - 1] = '\0';
    plen--;
  }

  // --- Look up glyphs for each character ---
  int numGlyphs = strlen(processed);
  Glyph **glyphs = malloc(numGlyphs * sizeof(Glyph *));
  if (!glyphs) {
    perror("malloc");
    exit(1);
  }
  int totalWidth = 0;
  for (int i = 0; i < numGlyphs; i++) {
    char ch = processed[i];
    Glyph *g = getGlyph(ch);
    if (!g) {
      fprintf(stderr, "Error: no glyph for character '%c'\n", ch);
      exit(1);
    }
    glyphs[i] = g;
    totalWidth += g->width;
  }
  // In simple comment mode, the comment prefix (plus a separating space)
  // counts toward the width.
  if (commentMode && !blockComment)
    totalWidth += (strlen(simplePrefix) + 1);

  // Check maximum allowed width (also check block delimiters in block mode).
  if (blockComment) {
    if (totalWidth > maxWidth || (int)strlen(blockOpen) > maxWidth ||
        (int)strlen(blockClose) > maxWidth) {
      fprintf(stderr, "Error: output width exceeds maximum length %d\n",
              maxWidth);
      exit(1);
    }
  } else {
    if (totalWidth > maxWidth) {
      fprintf(stderr, "Error: output width %d exceeds maximum length %d\n",
              totalWidth, maxWidth);
      exit(1);
    }
  }

  // --- Assemble each row (of 4 possible rows) ---
  // Even though every glyph has 4 rows, if all glyphs have an empty string
  // for a given row, we skip that row.
  bool active[4] = {false, false, false, false};
  for (int row = 0; row < 4; row++) {
    for (int i = 0; i < numGlyphs; i++) {
      if (strlen(glyphs[i]->lines[row]) > 0) {
        active[row] = true;
        break;
      }
    }
  }
  // Allocate a buffer for each active row.
  char *assembled[4] = {NULL, NULL, NULL, NULL};
  for (int row = 0; row < 4; row++) {
    if (!active[row])
      continue;
    assembled[row] = malloc(totalWidth + 1);
    if (!assembled[row]) {
      perror("malloc");
      exit(1);
    }
    assembled[row][0] = '\0';
    // For each glyph, append its row string or padded spaces.
    for (int i = 0; i < numGlyphs; i++) {
      Glyph *g = glyphs[i];
      const char *seg = g->lines[row];
      if (strlen(seg) == 0) {
        char *pad = make_padding(g->width);
        strcat(assembled[row], pad);
        free(pad);
      } else {
        // (Assumes the glyph row is already the correct width.)
        strncat(assembled[row], seg, g->width);
      }
    }
  }

  // In simple comment mode, prepend the comment delimiter and a space.
  if (commentMode && !blockComment) {
    for (int row = 0; row < 4; row++) {
      if (!assembled[row])
        continue;
      int newLen = strlen(simplePrefix) + 1 + strlen(assembled[row]);
      char *temp = malloc(newLen + 1);
      if (!temp) {
        perror("malloc");
        exit(1);
      }
      strcpy(temp, simplePrefix);
      strcat(temp, " ");
      strcat(temp, assembled[row]);
      free(assembled[row]);
      assembled[row] = temp;
    }
  }

  // --- Output ---
  // We build the output by concatenating the active rows with '\n'.
  // In block comment mode, output blockOpen and blockClose lines.
  if (blockComment)
    printf("%s\n", blockOpen);
  for (int row = 0; row < 4; row++) {
    if (assembled[row])
      printf("%s\n", assembled[row]);
  }
  if (blockComment)
    printf("%s\n", blockClose);

  // Free allocated memory.
  for (int row = 0; row < 4; row++) {
    free(assembled[row]);
  }
  free(glyphs);
  free(processed);
  // If we allocated renderText (in block comment mode), free it.
  if (blockComment)
    free(renderText);

  return 0;
}
