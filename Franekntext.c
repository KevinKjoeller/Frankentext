#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_WORD_COUNT 15000
#define MAX_SUCCESSOR_COUNT (MAX_WORD_COUNT / 2)

// Sample Frankenstein text - in production, load from pg84.txt file
char book[] = 
    "I am by birth a Genevese, and my family is one of the most distinguished of that republic. "
    "My ancestors had been for many years counsellors and syndics, and my father had filled several public situations "
    "with honour and reputation. He was respected by all who knew him for his integrity and indefatigable attention "
    "to public business. He passed his younger days perpetually occupied by the affairs of his country; a variety "
    "of circumstances had prevented his marrying early, nor was it until the decline of life that he became a husband "
    "and the father of a family. The interval was spent in travelling. Nothing is so painful to the human mind as a great and sudden change! "
    "I was possessed by a maddening rage when I thought of him. The miserable monster whom I had created. "
    "Why did I not die? More miserable than man ever was before, why did I not sink into forgetfulness and rest?";

/// Array of tokens registered so far.
/// No duplicates are allowed.
char *tokens[MAX_WORD_COUNT];
/// `tokens`'s current size
size_t tokens_size = 0;

/// Array of successor tokens
/// One token can have many successor tokens. `succs[x]` corresponds to
/// `token[x]`'s successors.
/// We store directly tokens instead of token_ids, because we will directly
/// print them. If we wanted to delete the book, then it would make more sense
/// to store `token_id`s
char *succs[MAX_WORD_COUNT][MAX_SUCCESSOR_COUNT];
/// `succs`'s current size
size_t succs_sizes[MAX_WORD_COUNT];

/// Overwrites non-printable characters in `book` with a space.
/// Non-printable characters may lead to duplicates like
/// `"\xefthe" and "the"` even both print `the`.
void replace_non_printable_chars_with_space() {
  for (size_t i = 0; book[i] != '\0'; ++i) {
    if (!isprint(book[i])) {
      book[i] = ' ';
    }
  }
}

/// Returns the id (index) of the token, creating it if necessary.
///
/// Returns token id if token exists in \c tokens, otherwise creates a new entry
/// in \c tokens and returns its token id.
///
/// \param token token to look up (or insert)
/// \return Index of `token` in \c tokens array
size_t token_id(char *token) {
  size_t id;
  for (id = 0; id < tokens_size; ++id) {
    if (strcmp(tokens[id], token) == 0) {
      return id;
    }
  }
  tokens[id] = token;
  ++tokens_size;
  return id;
}

/// Appends the token \c succ to the successors list of \c token.
void append_to_succs(char *token, char *succ) {
  size_t *next_empty_index_ptr = &succs_sizes[token_id(token)];

  if (*next_empty_index_ptr >= MAX_SUCCESSOR_COUNT) {
    printf("Successor array full.");
    exit(EXIT_FAILURE);
  }

  succs[token_id(token)][(*next_empty_index_ptr)++] = succ;
}

/// Creates tokens on \c book and fills \c tokens and \c succs using
/// the functions \c token_id and \c append_to_succs.
void tokenize_and_fill_succs(char *delimiters, char *str) {
  char *token = strtok(str, delimiters);
  if (token == NULL) return;
  
  char *prev_token = token;
  token_id(prev_token); // Register first token
  
  while ((token = strtok(NULL, delimiters)) != NULL) {
    append_to_succs(prev_token, token);
    prev_token = token;
  }
}

/// Returns last character of a string
char last_char(char *str) {
  size_t len = strlen(str);
  if (len == 0) return '\0';
  return str[len - 1];
}

/// Returns whether the token ends with `!`, `?` or `.`.
bool token_ends_a_sentence(char *token) {
  char last = last_char(token);
  return last == '.' || last == '!' || last == '?';
}

/// Returns a random `token_id` that corresponds to a `token` that starts with a
/// capital letter.
/// Uses \c tokens and \c tokens_size.
size_t random_token_id_that_starts_a_sentence() {
  size_t capital_tokens[MAX_WORD_COUNT];
  size_t capital_count = 0;
  
  for (size_t i = 0; i < tokens_size; ++i) {
    if (isupper(tokens[i][0])) {
      capital_tokens[capital_count++] = i;
    }
  }
  
  if (capital_count == 0) {
    return 0; // Fallback to first token
  }
  
  return capital_tokens[rand() % capital_count];
}

/// Generates a random sentence using \c tokens, \c succs, and \c succs_sizes.
/// The sentence array will be filled up to \c sentence_size-1 characters using
/// random tokens until:
/// - a token is found where \c token_ends_a_sentence
/// - or more tokens cannot be concatenated to the \c sentence anymore.
/// Returns the filled sentence array.
///
/// @param sentence array what will be used for the sentence.
//
//                  Will be overwritten. Does not have to be initialized.
/// @param sentence_size
/// @return input sentence pointer
char *generate_sentence(char *sentence, size_t sentence_size) {
  size_t current_token_id = random_token_id_that_starts_a_sentence();
  char *token = tokens[current_token_id];

  sentence[0] = '\0';
  strcat(sentence, token);
  if (token_ends_a_sentence(token))
    return sentence;

  // Calculated sentence length for the next iteration.
  // Used to stop the loop if the length exceeds sentence size
  size_t sentence_len_next;
  // Concatenates random successors to the sentence as long as
  // `sentence` can hold them.
  do {
    size_t succ_count = succs_sizes[current_token_id];
    if (succ_count == 0) break; // No successors
    
    // Pick random successor
    size_t random_succ_idx = rand() % succ_count;
    char *succ_token = succs[current_token_id][random_succ_idx];
    
    // Calculate next length
    sentence_len_next = strlen(sentence) + 1 + strlen(succ_token);
    
    if (sentence_len_next >= sentence_size - 1) break;
    
    strcat(sentence, " ");
    strcat(sentence, succ_token);
    
    // Update current token
    current_token_id = token_id(succ_token);
    
    if (token_ends_a_sentence(succ_token)) break;
    
  } while (sentence_len_next < sentence_size - 1);
  return sentence;
}

int main() {
  replace_non_printable_chars_with_space();

  char *delimiters = " \n\r";
  tokenize_and_fill_succs(delimiters, book);

  char sentence[1000];
  srand(time(NULL)); // Be random each time we run the program

  // Generate sentences until we find a question sentence.
  do {
    generate_sentence(sentence, sizeof sentence);
  } while (last_char(sentence) != '?');
  puts(sentence);
  puts("");

  // Initialize `sentence` and then generate sentences until we find a sentence
  // ending with an exclamation mark.
  do {
    generate_sentence(sentence, sizeof sentence);
  } while (last_char(sentence) != '!');
  puts(sentence);
}
