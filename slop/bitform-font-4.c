#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_MAX_LENGTH 80
#define MAX_INPUT 1024
#define MAX_LINES 4

// CLAUDE v.1

typedef struct {
  char ch;
  const char *lines[4];
  int width;
} Glyph;

// Font data as provided
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

typedef struct {
  char prefix[6];
  char suffix[4];
  bool multiline;
} CommentStyle;

const CommentStyle COMMENT_STYLES[] = {
    {"#", "", false}, {"//", "", false},  {"--", "", false},
    {";", "", false}, {"/*", "*/", true}, {"<!--", "-->", true}};
const int NUM_COMMENT_STYLES = sizeof(COMMENT_STYLES) / sizeof(CommentStyle);

const Glyph *find_glyph(char c) {
  for (int i = 0; i < sizeof(fontData) / sizeof(Glyph); i++) {
    if (fontData[i].ch == c)
      return &fontData[i];
  }
  return NULL;
}

void print_help() {
  printf("ASCII Art Generator\n\n");
  printf("Usage: ascii-art [--length=N] [--help|-h]\n\n");
  printf("Options:\n");
  printf("  --length=N    Set maximum output width (default: 80)\n");
  printf("  --help, -h    Show this help message\n\n");
  printf("Input is read from stdin and must be ASCII text.\n");
  printf("Text will be converted to uppercase before rendering.\n");
  printf("Whitespace between glyphs will be replaced with '.'.\n");
  printf("Comment markers (#, //, --, ;, /*, <!--) are preserved.\n");
}

bool starts_with(const char *str, const char *prefix) {
  while (*str == ' ' || *str == '\t')
    str++; // Skip whitespace
  return strncmp(str, prefix, strlen(prefix)) == 0;
}

const CommentStyle *detect_comment_style(const char *input) {
  for (int i = 0; i < NUM_COMMENT_STYLES; i++) {
    if (starts_with(input, COMMENT_STYLES[i].prefix)) {
      return &COMMENT_STYLES[i];
    }
  }
  return NULL;
}

int calculate_total_width(const char *input, const CommentStyle *comment) {
  int width = 0;
  const char *p = input;

  // Skip whitespace and comment prefix
  if (comment) {
    while (*p == ' ' || *p == '\t')
      p++;
    p += strlen(comment->prefix);
    width += strlen(comment->prefix) + 1; // +1 for the space after prefix
  }

  // Calculate width for each character
  while (*p && *p != '\n') {
    if (comment && comment->multiline &&
        strncmp(p, comment->suffix, strlen(comment->suffix)) == 0) {
      break;
    }
    char c = toupper(*p);
    const Glyph *g = find_glyph(c);
    if (g)
      width += g->width;
    p++;
  }

  return width;
}

void render_ascii_art(const char *input, int max_length) {
  char output[MAX_LINES][MAX_INPUT] = {0};
  const CommentStyle *comment = detect_comment_style(input);
  const char *p = input;
  int total_width;

  // Calculate total width and check against max_length
  total_width = calculate_total_width(input, comment);
  if (total_width > max_length) {
    fprintf(stderr, "Error: Output width %d exceeds maximum length %d\n",
            total_width, max_length);
    return;
  }

  // Skip leading whitespace
  while (*p == ' ' || *p == '\t')
    p++;

  // Handle comment prefix
  if (comment) {
    if (comment->multiline) {
      printf("%s\n", comment->prefix);
    } else {
      for (int i = 0; i < MAX_LINES; i++) {
        strcat(output[i], comment->prefix);
        strcat(output[i], " ");
      }
    }
    p += strlen(comment->prefix);
  }

  // Process each character
  while (*p && *p != '\n') {
    if (comment && comment->multiline &&
        strncmp(p, comment->suffix, strlen(comment->suffix)) == 0) {
      break;
    }

    char c = toupper(*p);
    const Glyph *g = find_glyph(c);
    if (g) {
      for (int i = 0; i < MAX_LINES; i++) {
        strcat(output[i], g->lines[i]);
        // Pad with spaces if line is shorter than glyph width
        int pad = g->width - strlen(g->lines[i]);
        for (int j = 0; j < pad; j++) {
          strcat(output[i], " ");
        }
      }
    }
    p++;
  }

  // Output the lines, skipping empty ones
  bool has_content[MAX_LINES] = {false};
  for (int i = 0; i < MAX_LINES; i++) {
    for (int j = 0; output[i][j]; j++) {
      if (output[i][j] != ' ') {
        has_content[i] = true;
        break;
      }
    }
  }

  for (int i = 0; i < MAX_LINES; i++) {
    if (has_content[i]) {
      printf("%s\n", output[i]);
    }
  }

  // Handle multiline comment suffix
  if (comment && comment->multiline) {
    printf("%s\n", comment->suffix);
  }
}

int main(int argc, char *argv[]) {
  int max_length = DEFAULT_MAX_LENGTH;
  char input[MAX_INPUT];

  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      print_help();
      return 0;
    }
    if (strncmp(argv[i], "--length=", 9) == 0) {
      max_length = atoi(argv[i] + 9);
      if (max_length <= 0) {
        fprintf(stderr, "Error: Invalid length value\n");
        return 1;
      }
    }
  }

  // Read input
  if (!fgets(input, sizeof(input), stdin)) {
    fprintf(stderr, "Error: Failed to read input\n");
    return 1;
  }

  // Remove trailing newline if present
  size_t len = strlen(input);
  if (len > 0 && input[len - 1] == '\n') {
    input[len - 1] = '\0';
  }

  render_ascii_art(input, max_length);
  return 0;
}
