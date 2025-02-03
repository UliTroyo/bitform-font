#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_BUFFER_SIZE 1024

/* --- Embedded Glyph Data --- */
typedef struct {
  char ch;
  const char *lines[4];
  int width; // computed as the max length of the center two rows
} Glyph;

/* (For brevity, only a few glyphs are defined.
   In your actual app you’d include all glyphs from your JSON.) */
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
const int fontDataCount = sizeof(fontData) / sizeof(fontData[0]);

/* Return the glyph for the given character.
   Letters are mapped case‑insensitively.
   Unknown characters default to the space glyph. */
const Glyph *get_glyph(char ch) {
  char uc = ch;
  if (islower(ch))
    uc = toupper(ch);
  for (int i = 0; i < fontDataCount; i++) {
    if (fontData[i].ch == uc)
      return &fontData[i];
  }
  for (int i = 0; i < fontDataCount; i++) {
    if (fontData[i].ch == ' ')
      return &fontData[i];
  }
  return NULL; // should not occur
}

/* --- Helpers for String Processing --- */

/* trim() returns a newly allocated string with leading/trailing whitespace
 * removed. */
char *trim(const char *str) {
  while (*str && isspace((unsigned char)*str))
    str++;
  const char *end = str + strlen(str);
  while (end > str && isspace((unsigned char)*(end - 1)))
    end--;
  size_t len = end - str;
  char *result = malloc(len + 1);
  if (!result)
    exit(1);
  memcpy(result, str, len);
  result[len] = '\0';
  return result;
}

/* collapse_whitespace() converts any internal contiguous whitespace into a
   single space. (For simplicity we assume that the input line does not contain
   newlines.) */
char *collapse_whitespace(const char *str) {
  size_t len = strlen(str);
  char *result = malloc(len + 1);
  if (!result)
    exit(1);
  int inSpace = 0;
  size_t j = 0;
  for (size_t i = 0; i < len; i++) {
    if (isspace((unsigned char)str[i])) {
      if (!inSpace) {
        result[j++] = ' ';
        inSpace = 1;
      }
    } else {
      result[j++] = str[i];
      inSpace = 0;
    }
  }
  result[j] = '\0';
  return result;
}

/* detect_comment_prefix() examines the beginning of the input (after trimming)
   and returns one of the supported comment markers ("--", "#", ";" or "\\").
   The returned string is newly allocated. */
char *detect_comment_prefix(const char *input) {
  while (*input && isspace((unsigned char)*input))
    input++;
  if (strncmp(input, "--", 2) == 0)
    return strdup("--");
  if (*input == '#' || *input == ';' || *input == '\\') {
    char *prefix = malloc(2);
    prefix[0] = *input;
    prefix[1] = '\0';
    return prefix;
  }
  return strdup("");
}

/* append_str() appends the string src to the dynamic buffer *dest.
 *len is the current length and *cap is the capacity; both may be updated.
 */
void append_str(char **dest, size_t *len, size_t *cap, const char *src) {
  size_t src_len = strlen(src);
  while (*len + src_len >= *cap) {
    *cap *= 2;
    *dest = realloc(*dest, *cap);
    if (!*dest)
      exit(1);
  }
  memcpy(*dest + *len, src, src_len);
  *len += src_len;
  (*dest)[*len] = '\0';
}

/* --- Building ASCII Art from Glyphs --- */

/* For a given word, build a 4-row ascii art block.
   Returns an array of 4 newly allocated strings.
   (Remember to free each row and the array when done.) */
char **build_word_art(const char *word) {
  char **rows = malloc(4 * sizeof(char *));
  if (!rows)
    exit(1);
  for (int i = 0; i < 4; i++) {
    size_t cap = INITIAL_BUFFER_SIZE;
    rows[i] = malloc(cap);
    if (!rows[i])
      exit(1);
    rows[i][0] = '\0';
    size_t len = 0;
    for (size_t j = 0; j < strlen(word); j++) {
      const Glyph *g = get_glyph(word[j]);
      append_str(&rows[i], &len, &cap, g->lines[i]);
    }
  }
  return rows;
}

/* Assemble a line from multiple word arts, inserting a space glyph between
   words. Each output row is padded with spaces to exactly maxWidth columns.
   Returns an array of 4 newly allocated strings. */
char **assemble_line(char ***wordArts, int count, int maxWidth) {
  char **rows = malloc(4 * sizeof(char *));
  if (!rows)
    exit(1);
  for (int i = 0; i < 4; i++) {
    size_t cap = INITIAL_BUFFER_SIZE;
    rows[i] = malloc(cap);
    if (!rows[i])
      exit(1);
    rows[i][0] = '\0';
    size_t len = 0;
    for (int w = 0; w < count; w++) {
      if (w != 0) {
        const Glyph *spaceGlyph = get_glyph(' ');
        append_str(&rows[i], &len, &cap, spaceGlyph->lines[i]);
      }
      append_str(&rows[i], &len, &cap, wordArts[w][i]);
    }
    // Pad with spaces if needed.
    while (strlen(rows[i]) < (size_t)maxWidth)
      append_str(&rows[i], &len, &cap, " ");
  }
  return rows;
}

/* Free a 4-row word art block. */
void free_word_art(char **wordArt) {
  for (int i = 0; i < 4; i++)
    free(wordArt[i]);
  free(wordArt);
}

/* --- A Simple Dynamic Array of Word Arts --- */
typedef struct {
  char ***arts; // each element is a 4-row word art
  int count;
  int capacity;
} WordArtArray;

void init_wordart_array(WordArtArray *arr) {
  arr->capacity = 8;
  arr->count = 0;
  arr->arts = malloc(arr->capacity * sizeof(char **));
  if (!arr->arts)
    exit(1);
}

void push_wordart_array(WordArtArray *arr, char **wordArt) {
  if (arr->count >= arr->capacity) {
    arr->capacity *= 2;
    arr->arts = realloc(arr->arts, arr->capacity * sizeof(char **));
    if (!arr->arts)
      exit(1);
  }
  arr->arts[arr->count++] = wordArt;
}

void free_wordart_array(WordArtArray *arr) { free(arr->arts); }

/* Return the “width” of a word art (using row 1 as representative). */
int word_art_width(char **wordArt) { return (int)strlen(wordArt[1]); }

/* Compute the total width of a line built from word arts, adding a space glyph
 * between words. */
int total_line_width(WordArtArray *arr) {
  int width = 0;
  for (int i = 0; i < arr->count; i++) {
    if (i > 0)
      width += get_glyph(' ')->width;
    width += word_art_width(arr->arts[i]);
  }
  return width;
}

/* --- Main --- */
int main(int argc, char **argv) {
  int use_stdin = 0;
  char *out_filename = NULL;
  int max_line_width = 80;
  char *input_filename = NULL;

  /* Simple command-line argument parsing */
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--stdin") == 0) {
      use_stdin = 1;
    } else if (strcmp(argv[i], "--length") == 0) {
      if (++i >= argc) {
        fprintf(stderr, "Missing value for --length\n");
        return 1;
      }
      max_line_width = atoi(argv[i]);
    } else if (strcmp(argv[i], "--out") == 0 || strcmp(argv[i], "-o") == 0) {
      if (++i >= argc) {
        fprintf(stderr, "Missing value for --out\n");
        return 1;
      }
      out_filename = argv[i];
    } else {
      /* Non-flag argument: input filename */
      input_filename = argv[i];
    }
  }

  /* Read input from either stdin or file */
  FILE *infile = NULL;
  if (use_stdin)
    infile = stdin;
  else if (input_filename) {
    infile = fopen(input_filename, "r");
    if (!infile) {
      perror("Error opening input file");
      return 1;
    }
  } else {
    fprintf(stderr, "No input specified. Use --stdin or provide a filename.\n");
    return 1;
  }

  fseek(infile, 0, SEEK_END);
  long fsize = ftell(infile);
  fseek(infile, 0, SEEK_SET);
  char *input_text = malloc(fsize + 1);
  if (!input_text)
    exit(1);
  fread(input_text, 1, fsize, infile);
  input_text[fsize] = '\0';
  if (infile != stdin)
    fclose(infile);

  /* Detect comment prefix from the entire input */
  char *comment_prefix = detect_comment_prefix(input_text);

  /* Open output file if specified */
  FILE *outfile = stdout;
  if (out_filename) {
    outfile = fopen(out_filename, "w");
    if (!outfile) {
      perror("Error opening output file");
      return 1;
    }
  }

  /* Process input line by line.
     (Each newline in the input represents a text break.) */
  char *line = strtok(input_text, "\n");
  while (line) {
    char *trimmed = trim(line);
    if (strlen(trimmed) == 0) {
      free(trimmed);
      line = strtok(NULL, "\n");
      continue;
    }
    char *collapsed = collapse_whitespace(trimmed);
    free(trimmed);

    /* Split collapsed line into words (tokens separated by space) */
    WordArtArray wordArts;
    init_wordart_array(&wordArts);
    char *word = strtok(collapsed, " ");
    while (word) {
      char **art = build_word_art(word);
      push_wordart_array(&wordArts, art);
      word = strtok(NULL, " ");
    }
    free(collapsed);

    /* Assemble words into output lines with wrapping.
       We build a dynamic array (currentLine) of word arts until adding the next
       word would exceed max_line_width. */
    WordArtArray currentLine;
    init_wordart_array(&currentLine);
    for (int i = 0; i < wordArts.count; i++) {
      int wordWidth = word_art_width(wordArts.arts[i]);
      if (currentLine.count == 0) {
        push_wordart_array(&currentLine, wordArts.arts[i]);
      } else {
        int extra = get_glyph(' ')->width + wordWidth;
        if (total_line_width(&currentLine) + extra > max_line_width) {
          char **assembled = assemble_line(currentLine.arts, currentLine.count,
                                           max_line_width);
          for (int r = 0; r < 4; r++) {
            fprintf(outfile, "%s %s\n", comment_prefix, assembled[r]);
            free(assembled[r]);
          }
          free(assembled);
          currentLine.count = 0;
          push_wordart_array(&currentLine, wordArts.arts[i]);
        } else {
          push_wordart_array(&currentLine, wordArts.arts[i]);
        }
      }
    }
    if (currentLine.count > 0) {
      char **assembled =
          assemble_line(currentLine.arts, currentLine.count, max_line_width);
      for (int r = 0; r < 4; r++) {
        fprintf(outfile, "%s %s\n", comment_prefix, assembled[r]);
        free(assembled[r]);
      }
      free(assembled);
    }
    free_wordart_array(&currentLine);

    /* Free word art blocks (they were stored in wordArts) */
    for (int i = 0; i < wordArts.count; i++) {
      free_word_art(wordArts.arts[i]);
    }
    free_wordart_array(&wordArts);

    line = strtok(NULL, "\n");
  }

  free(comment_prefix);
  free(input_text);
  if (outfile != stdout)
    fclose(outfile);
  return 0;
}
