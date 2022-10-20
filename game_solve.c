#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 4) {
    fprintf(stderr, "parametre manquant ou en trop\n");
    return EXIT_FAILURE;
  }
  game g = game_load(argv[2]);
  if (strcmp(argv[1], "-s") == 0) {
    if (game_solve(g)) {
      if (argc == 3) {
        game_print(g);
        return EXIT_SUCCESS;
      } else {
        game_save(g, argv[3]);
        return EXIT_SUCCESS;
      }
    } else {
      fprintf(stderr, "Aucune solution trouvé\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "-c") == 0) {
    uint cpt = game_nb_solutions(g);
    if (argc == 3) {
      printf("%u\n", cpt);
      return EXIT_SUCCESS;
    } else {
      FILE *fichiersol = fopen(argv[3], "w");
      fprintf(fichiersol, "%u\n", cpt);
      fclose(fichiersol);
      return EXIT_SUCCESS;
    }
  } else {
    fprintf(stderr, "-s ou bien -c pour executer game_solve\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}