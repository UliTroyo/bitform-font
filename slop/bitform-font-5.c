#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_LEN 31
#define MAX_OUTPUT_LEN 2048
#define MAX_GLYPH_WIDTH 8
#define GLYPH_HEIGHT 4

// Claude v.2

typedef struct {
  char ch;
  const char *lines[GLYPH_HEIGHT];
  int width;
} Glyph;

// Sample glyphs - would be expanded to full ASCII 32-126 range
static const Glyph GLYPHS[] = {
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

typedef enum {
  COMMENT_NONE,
  COMMENT_SINGLE_HASH,    // #
  COMMENT_SINGLE_SLASHES, // //
  COMMENT_SINGLE_DASHES,  // --
  COMMENT_SINGLE_SEMI,    // ;
  COMMENT_MULTI_C,        // /* */
  COMMENT_MULTI_HTML      // <!-- -->
} CommentType;

typedef struct {
  char buffer[MAX_INPUT_LEN + 1];
  size_t length;
  CommentType comment_type;
} InputBuffer;

typedef struct {
  char buffer[MAX_OUTPUT_LEN];
  size_t length;
  int max_width;
  int used_height;
} OutputBuffer;

static void print_help(void) {
  printf("ASCII Art Generator\n"
         "Usage: echo \"text\" | ascii-art [options]\n"
         "Options:\n"
         "  --help, -h    Show this help message\n"
         "Input must be piped through stdin and must be ASCII text.\n");
}

static CommentType detect_comment_type(const char *input) {
  // Strip leading whitespace
  while (*input && isspace(*input))
    input++;

  if (strncmp(input, "<!--", 4) == 0)
    return COMMENT_MULTI_HTML;
  if (strncmp(input, "/*", 2) == 0)
    return COMMENT_MULTI_C;
  if (strncmp(input, "//", 2) == 0)
    return COMMENT_SINGLE_SLASHES;
  if (strncmp(input, "--", 2) == 0)
    return COMMENT_SINGLE_DASHES;
  if (input[0] == '#')
    return COMMENT_SINGLE_HASH;
  if (input[0] == ';')
    return COMMENT_SINGLE_SEMI;

  return COMMENT_NONE;
}

static void process_input(InputBuffer *input) {
  // Convert to uppercase and collapse whitespace
  char temp[MAX_INPUT_LEN + 1];
  int j = 0;
  int space_pending = 0;

  for (size_t i = 0; i < input->length; i++) {
    if (isspace(input->buffer[i])) {
      space_pending = 1;
      continue;
    }
    if (space_pending && j > 0) {
      temp[j++] = ' ';
      space_pending = 0;
    }
    temp[j++] = toupper(input->buffer[i]);
  }
  temp[j] = '\0';

  strcpy(input->buffer, temp);
  input->length = j;
}

static const Glyph *find_glyph(char ch) {
  for (size_t i = 0; i < sizeof(GLYPHS) / sizeof(GLYPHS[0]); i++) {
    if (GLYPHS[i].ch == ch)
      return &GLYPHS[i];
  }
  return NULL;
}

static void append_comment_delimiter(OutputBuffer *output, CommentType type,
                                     int is_opening) {
  const char *delimiter = NULL;

  switch (type) {
  case COMMENT_MULTI_HTML:
    delimiter = is_opening ? "<!--\n" : "-->\n";
    break;
  case COMMENT_MULTI_C:
    delimiter = is_opening ? "/*\n" : "*/\n";
    break;
  case COMMENT_SINGLE_HASH:
    delimiter = "# ";
    break;
  case COMMENT_SINGLE_SLASHES:
    delimiter = "// ";
    break;
  case COMMENT_SINGLE_DASHES:
    delimiter = "-- ";
    break;
  case COMMENT_SINGLE_SEMI:
    delimiter = "; ";
    break;
  default:
    return;
  }

  size_t len = strlen(delimiter);
  memcpy(output->buffer + output->length, delimiter, len);
  output->length += len;
}

static void generate_ascii_art(const InputBuffer *input, OutputBuffer *output) {
  output->length = 0;
  output->max_width = 0;
  output->used_height = 0;

  // Add opening comment if needed
  if (input->comment_type >= COMMENT_MULTI_C) {
    append_comment_delimiter(output, input->comment_type, 1);
  }

  // Calculate max width and prepare glyph array
  const Glyph *glyphs[MAX_INPUT_LEN];
  int total_width = 0;

  for (size_t i = 0; i < input->length; i++) {
    glyphs[i] = find_glyph(input->buffer[i]);
    if (glyphs[i]) {
      total_width += glyphs[i]->width;
    }
  }

  // Generate output line by line
  for (int line = 0; line < GLYPH_HEIGHT; line++) {
    // Add comment delimiter for single-line comments
    if (input->comment_type > COMMENT_NONE &&
        input->comment_type < COMMENT_MULTI_C) {
      append_comment_delimiter(output, input->comment_type, 1);
    }

    // Add glyph lines
    for (size_t i = 0; i < input->length; i++) {
      if (glyphs[i]) {
        const char *glyph_line = glyphs[i]->lines[line];
        size_t len = strlen(glyph_line);
        memcpy(output->buffer + output->length, glyph_line, len);
        output->length += len;
      }
    }

    output->buffer[output->length++] = '\n';
  }

  // Add closing comment if needed
  if (input->comment_type >= COMMENT_MULTI_C) {
    append_comment_delimiter(output, input->comment_type, 0);
  }

  output->buffer[output->length] = '\0';
}

int main(int argc, char *argv[]) {
  // Check for help flag
  if (argc > 1 &&
      (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
    print_help();
    return 0;
  }

  // Verify input is piped
  if (isatty(STDIN_FILENO)) {
    fprintf(stderr, "Error: Input must be piped through stdin\n");
    return 1;
  }

  // Read input
  InputBuffer input = {0};
  int c;
  while ((c = getchar()) != EOF && input.length < MAX_INPUT_LEN) {
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Invalid character in input\n");
      return 1;
    }
    input.buffer[input.length++] = c;
  }

  if (input.length == 0) {
    print_help();
    return 0;
  }

  if (c != EOF) {
    fprintf(stderr, "Error: Input exceeds maximum length of %d characters\n",
            MAX_INPUT_LEN);
    return 1;
  }

  input.buffer[input.length] = '\0';

  // Detect comment type
  input.comment_type = detect_comment_type(input.buffer);

  // Process input
  process_input(&input);

  // Generate and output ASCII art
  OutputBuffer output = {0};
  generate_ascii_art(&input, &output);
  printf("%s", output.buffer);

  return 0;
}
