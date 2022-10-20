#include "game_tools.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_private.h"

game game_load(char* filename) {
  if (!filename) {
    fprintf(stderr, "ERROR: Called game_load on an invalid param!\n");
    exit(EXIT_FAILURE);
  }
  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "ERROR: Failed to open the file!\n");
    exit(EXIT_FAILURE);
  }
  uint nb_rows = 0, nb_cols = 0, wrapping = 0;
  char actual = fgetc(f);
  uint cptval = 1;
  while (actual != '\n') {  // getting nb_rows, nb_cols and wrapping option
    if (actual == ' ') cptval++;
    switch (cptval) {
      case 1:
        nb_rows = actual - '0';
        actual = fgetc(f);
        break;
      case 2:
        nb_cols = actual - '0';
        actual = fgetc(f);
        break;
      case 3:
        wrapping = actual - '0';
        actual = fgetc(f);
        break;
      default:
        break;
    }
  }
  square* squares = malloc(sizeof(square) * nb_cols * nb_rows);
  int sq_tmp = 0;
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; !feof(f) && j < nb_cols; j++) {
      actual = fgetc(f);
      if (actual == '\n') actual = fgetc(f);
      if (actual == 'b') {
        sq_tmp = S_BLANK;
      } else if (actual == 'w') {
        sq_tmp = S_BLACKU;
      } else if (actual == '*') {
        sq_tmp = S_LIGHTBULB;
      } else if (actual == '-') {
        sq_tmp = S_MARK;
      } else if (actual == '.') {
        sq_tmp = S_BLANK;
      } else {
        sq_tmp = actual - '0' + S_BLACK;
      }
      squares[i * nb_cols + j] = sq_tmp;
    }
  }
  fclose(f);
  game g = game_new_ext(nb_rows, nb_cols, squares,
                        wrapping);  // new game with file indications
  game_update_flags(g);
  free(squares);
  return g;
}

void game_save(cgame g, char* filename) {
  if (filename == NULL || g == NULL) {
    fprintf(stderr, "ERROR: a parameter is empty !");
    exit(EXIT_FAILURE);
  }
  uint nbrows = game_nb_rows(g);
  uint nbcols = game_nb_cols(g);
  FILE* fic = fopen(filename, "w");
  fprintf(fic, "%u ", nbrows);
  fprintf(fic, "%u ", nbcols);
  fprintf(fic, "%u\n", game_is_wrapping(g));
  for (uint i = 0; i < nbrows; i++) {
    for (uint j = 0; j < nbcols; j++) {
      if (game_is_blank(g, i, j)) {
        fprintf(fic, "b");
      } else {
        char c = _square2str(SQUARE(g, i, j));
        fprintf(fic, "%c", c);
      }
    }
    fprintf(fic, "\n");
  }
  fclose(fic);
}

/* ************************************************************************** */

bool not_exist(game g, uint* nb_sol, game* tab) {
  int taille = *nb_sol;
  if (taille == 0) {
    tab[0] = game_copy(g);
    *nb_sol = taille + 1;
    return true;
  }
  for (int i = 0; i < taille; i++) {
    if (game_equal(g, tab[i])) return false;
  }
  tab[taille] = game_copy(g);
  *nb_sol = taille + 1;
  return true;
}

/* ************************************************************************** */

static bool game_is_error(game g) {
  for (uint i = 0; i < g->nb_cols; i++) {
    for (uint j = 0; j < g->nb_rows; j++) {
      if (game_has_error(g, i, j)) {
        return true;
      }
    }
  }
  return false;
}

/*bool is_already_solution(game solution, uint* nbSolutions,
                         square tab[][solution->nb_rows * solution->nb_cols]) {
  bool is_same;
  for (uint i = 0; i < *nbSolutions; i++) {
    is_same = true;
    for (uint j = 0; j < solution->nb_rows * solution->nb_cols; j++) {
      if (solution->squares[j] != tab[i][j]) {
        is_same = false;
      }
    }
    if (is_same) {
      return true;
    }
  }
  return false;
}

void print_all_solutions(uint pos, game solution,
                         square tab[][solution->nb_rows * solution->nb_cols],
                         uint* nbSolutions) {
  if (pos == (solution->nb_rows * solution->nb_cols)) {
    game_update_flags(solution);
    if (game_is_over(solution)) {
      if (*nbSolutions == 0 ||
          !is_already_solution(solution, nbSolutions, tab)) {
        (*nbSolutions)++;
        for (uint i = 0; i < solution->nb_rows * solution->nb_cols; i++) {
          tab[*nbSolutions][i] = solution->squares[i];
        }
      }
    }
    return;
  }
  if (!game_is_black(solution, pos / solution->nb_rows,
                     pos % solution->nb_cols)) {
    solution->squares[pos] = S_BLANK;
  }
  print_all_solutions(pos + 1, solution, tab, nbSolutions);
  if (!game_is_black(solution, pos / solution->nb_rows,
                     pos % solution->nb_cols)) {
    solution->squares[pos] = S_LIGHTBULB;
  }
  print_all_solutions(pos + 1, solution, tab, nbSolutions);
}
*/
static void game_solveur(uint position, game g, bool* solved) {
  if (position == (g->nb_rows * g->nb_cols)) {
    if (game_is_over(g)) {
      *solved = true;
      return;
    } else {
      return;
    }
  }
  if (!game_is_black(g, position / g->nb_cols, position % g->nb_cols) &&
      !game_is_lighted(g, position / g->nb_cols, position % g->nb_cols)) {
    g->squares[position] = S_LIGHTBULB;
    game_update_flags(g);
    if (game_is_error(g)) {
      g->squares[position] = S_BLANK;
      game_update_flags(g);
    }
    game_solveur(position + 1, g, solved);
  }

  if (*solved == true) {
    return;
  }
  if (!game_is_black(g, position / g->nb_cols, position % g->nb_cols)) {
    g->squares[position] = S_BLANK;
    game_update_flags(g);
  }
  game_solveur(position + 1, g, solved);
}

bool game_solve(game g) {
  bool solved = false;
  game_solveur(0, g, &solved);
  return solved;
}

static void game_nb_sol_aux(uint position, game g, uint* cpt) {
  if (position == (g->nb_rows * g->nb_cols)) {
    if (game_is_over(g)) {
      (*cpt)++;
      return;
    } else {
      return;
    }
  }
  if (!game_is_black(g, position / g->nb_cols, position % g->nb_cols) &&
      !game_is_lighted(g, position / g->nb_cols, position % g->nb_cols)) {
    g->squares[position] = S_LIGHTBULB;
    game_update_flags(g);
    if (!game_is_error(g)) {
      /*g->squares[position] = S_BLANK;
      game_update_flags(g);*/
      game_nb_sol_aux(position + 1, g, cpt);
    }
    // game_nb_sol_aux(position + 1, g, cpt, tab);
  }

  if (!game_is_black(g, position / g->nb_cols, position % g->nb_cols)) {
    g->squares[position] = S_BLANK;
    game_update_flags(g);
  }
  game_nb_sol_aux(position + 1, g, cpt);
}

uint game_nb_solutions(game g) {
  uint nb_solve = 0;
  game_nb_sol_aux(0, g, &nb_solve);
  return nb_solve;
}
