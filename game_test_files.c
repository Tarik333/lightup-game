#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_examples.h"
#include "game_ext.h"
#include "game_test.h"
#include "game_tools.h"

static int value[255] = {
    ['b'] = S_BLANK,  ['0'] = S_BLACK0,    ['1'] = S_BLACK1,
    ['2'] = S_BLACK2, ['3'] = S_BLACK3,    ['4'] = S_BLACK4,
    ['w'] = S_BLACKU, ['*'] = S_LIGHTBULB, ['-'] = S_MARK};
int str2square(char c) {
  unsigned char uc = c;
  return value[uc];
}

int test_game_load() {
  game g1 = game_default();
  game_play_move(g1, 0, 0, S_LIGHTBULB);
  game_play_move(g1, 0, 1, S_MARK);
  game_save(g1, "test1.txt");
  game g2 = game_load("test1.txt");
  if (game_equal(g1, g2)) {
    game_delete(g1);
    game_delete(g2);
    return EXIT_SUCCESS;
  } else {
    game_delete(g1);
    game_delete(g2);
    return EXIT_FAILURE;
  }
}

int test_game_save() {
  game g1 = game_default();
  game_play_move(g1, 0, 0, S_LIGHTBULB);
  game_play_move(g1, 0, 1, S_MARK);
  game_save(g1, "test1.txt");
  FILE *fic = fopen("test1.txt", "r");
  assert(fic);
  uint nbcols = game_nb_cols(g1);
  uint nbrows = game_nb_rows(g1);
  uint rows, cols, wrap;
  if (fscanf(fic, "%u %u %u", &rows, &cols, &wrap) != 3) {
    game_delete(g1);
    fprintf(stderr, "%d", __LINE__);
    return EXIT_FAILURE;
  }
  if (rows != nbrows || cols != nbcols || 0 != wrap) {
    game_delete(g1);
    fprintf(stderr, "%d", __LINE__);
    return EXIT_FAILURE;
  }
  char test;
  fgetc(fic);
  for (uint i = 0; i < rows; i++) {
    for (uint j = 0; j < cols; j++) {
      if (fscanf(fic, "%c", &test) != 1) {
        game_delete(g1);
        fprintf(stderr, "%d", __LINE__);
        return EXIT_FAILURE;
      } else {
        square value = str2square(test);
        if (value != game_get_state(g1, i, j)) {
          game_delete(g1);
          fprintf(stderr, "%d %u, %u", __LINE__, i, j);
          return EXIT_FAILURE;
        }
      }
    }
    fgetc(fic);
  }
  game_delete(g1);
  fclose(fic);
  return EXIT_SUCCESS;
}
