#include "model.h"

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_tools.h"
#include "queue.h"
#define APP_NAME "SDL2 Demo"
#define FONT "arial.ttf"
#define FONTSIZE 36

#define BACKGROUND "background.png"
#define WARNING "Warning.png"

struct Env_t {
  SDL_Texture *background;
  SDL_Texture *warning;
  SDL_Texture *ampoule;
  SDL_Texture *mark;
  SDL_Texture *grille;
  SDL_Texture *wall;
  SDL_Surface *surfaceMessage0;
  SDL_Surface *surfaceMessage1;
  SDL_Surface *surfaceMessage2;
  SDL_Surface *surfaceMessage3;
  SDL_Surface *surfaceMessage4;
  SDL_Surface *surfaceMessageErreur;
  game g;
  float grille_x, grille_y;
};

typedef struct Env_t Env;

Env *init(SDL_Window *fenetre, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));

  int w, h;
  SDL_GetWindowSize(fenetre, &w, &h);

  /* récupération du jeu */
  if (argc == 2) {
    (env->g) = game_load(argv[1]);
  } else {
    env->g = game_default();
  }
  // Chargement de toutes les textures de la structure.
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background) ERROR("IMG_LoadTexture: %s\n", BACKGROUND);

  env->warning = IMG_LoadTexture(ren, WARNING);
  if (!env->warning) ERROR("IMG_LoadTexture: %s\n", WARNING);

  char str[10];

  sprintf(str, "%d", 0);
  TTF_Font *Sans = TTF_OpenFont(FONT, 40);
  SDL_Color Red = {255, 0, 0};
  SDL_Color White = {255, 255, 255};
  env->surfaceMessage0 = TTF_RenderText_Blended(Sans, str, White);

  sprintf(str, "%d", 1);
  env->surfaceMessage1 = TTF_RenderText_Blended(Sans, str, White);

  sprintf(str, "%d", 2);
  env->surfaceMessage2 = TTF_RenderText_Blended(Sans, str, White);

  sprintf(str, "%d", 3);
  env->surfaceMessage3 = TTF_RenderText_Blended(Sans, str, White);

  sprintf(str, "%d", 4);
  env->surfaceMessage4 = TTF_RenderText_Blended(Sans, str, White);

  env->surfaceMessageErreur = TTF_RenderText_Blended(Sans, "Warning !!", Red);

  return env;
}

void TraceGrille(SDL_Renderer *ren, Env *env, int w, int h) {
  SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE); /* red */
  float marge = 0.15 * w;
  float cote = fmin(w, h) - marge;
  uint colonne = game_nb_cols(env->g);
  uint ligne = game_nb_rows(env->g);
  float w_case = (fmin(w, h) - (2 * marge)) / colonne;
  float h_case = (fmin(w, h) - (2 * marge)) / ligne;
  // CADRE
  SDL_RenderDrawLine(ren, marge, marge, marge, cote);
  SDL_RenderDrawLine(ren, marge, marge, cote, marge);
  SDL_RenderDrawLine(ren, marge, cote, cote, cote);
  SDL_RenderDrawLine(ren, cote, marge, cote, cote);

  /* TRACER LIGNES VERTICALES*/
  for (uint i = 0; i < colonne; i++) {
    SDL_RenderDrawLine(ren, marge + (w_case * i), marge, marge + (w_case * i),
                       cote);
  }
  /* TRACER LIGNES HORIZONTALES*/
  for (uint i = 0; i < ligne; i++) {
    SDL_RenderDrawLine(ren, marge, marge + (h_case * i), cote,
                       marge + (h_case * i));
  }
}

void PlacerCase(SDL_Renderer *ren, Env *env, int w, int h) {
  SDL_Rect rect;
  float marge = 0.15 * w;
  float cote = fmin(w, h) - marge;
  uint colonne = game_nb_cols(env->g);
  uint ligne = game_nb_rows(env->g);
  int number;
  float x_grille = cote - marge;
  float y_grille = cote - marge;
  uint case_x = x_grille / colonne;
  uint case_y = y_grille / ligne;
  int tab_x[colonne + 1];
  int tab_y[ligne + 1];
  // INITIALISATION D'UN TABLEAU DE COORDONNÉES EN X ET Y
  for (uint i = 0; i < colonne + 1; i++) {
    tab_x[i] = marge + (i * case_x);
  }
  for (uint i = 0; i < ligne + 1; i++) {
    tab_y[i] = marge + (i * case_y);
  }
  // BOUCLE SUR TOUTE LA GRILLE
  for (uint i = 0; i < colonne; i++) {
    for (uint j = 0; j < ligne; j++) {
      rect.y = tab_x[i];
      rect.x = tab_y[j];
      rect.w = tab_x[i + 1] - tab_x[i];
      rect.h = tab_y[j + 1] - tab_y[j];

      // POSE DES CASE NOIR
      if (game_is_black(env->g, i, j)) {
        number = game_get_black_number(env->g, i, j);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &rect);
        // create a rect content the black number
        SDL_Rect Message_rect;
        Message_rect.y = tab_x[i] + (0.2 * rect.w);
        Message_rect.x = tab_y[j] + (0.1 * rect.h);
        Message_rect.w = rect.w * 0.8;
        Message_rect.h = rect.h * 0.8;
        // Pose de la surface correspondant au bon chiffre dans les cases
        // blacks.
        if (number == 0) {
          SDL_Texture *Message =
              SDL_CreateTextureFromSurface(ren, env->surfaceMessage0);
          SDL_RenderCopy(ren, Message, NULL, &Message_rect);
          SDL_DestroyTexture(Message);
        } else if (number == 1) {
          SDL_Texture *Message =
              SDL_CreateTextureFromSurface(ren, env->surfaceMessage1);
          SDL_RenderCopy(ren, Message, NULL, &Message_rect);
          SDL_DestroyTexture(Message);
        } else if (number == 2) {
          SDL_Texture *Message =
              SDL_CreateTextureFromSurface(ren, env->surfaceMessage2);
          SDL_RenderCopy(ren, Message, NULL, &Message_rect);
          SDL_DestroyTexture(Message);
        } else if (number == 3) {
          SDL_Texture *Message =
              SDL_CreateTextureFromSurface(ren, env->surfaceMessage3);
          SDL_RenderCopy(ren, Message, NULL, &Message_rect);
          SDL_DestroyTexture(Message);
        } else if (number == 4) {
          SDL_Texture *Message =
              SDL_CreateTextureFromSurface(ren, env->surfaceMessage4);
          SDL_RenderCopy(ren, Message, NULL, &Message_rect);
          SDL_DestroyTexture(Message);
        }
      }
      // POSE DES BLANK
      if (game_is_blank(env->g, i, j)) {
        SDL_SetRenderDrawColor(ren, 169, 169, 169, 255);
        SDL_RenderFillRect(ren, &rect);
      }
      // POSE DES LIGHT
      if (game_is_lighted(env->g, i, j)) {
        SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
        SDL_RenderFillRect(ren, &rect);
      }
      // POSE DES LIGHTBULB
      if (game_is_lightbulb(env->g, i, j)) {
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderFillRect(ren, &rect);
      }
      // POSE DES LIGHTBULB
      if (game_has_error(env->g, i, j)) {
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_MOD);
        SDL_RenderFillRect(ren, &rect);
        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
      }
    }
  }
}

void PlacerLight(SDL_Window *win, SDL_Renderer *ren, Env *env, int mouse_x,
                 int mouse_y) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  float marge = 0.15 * w;
  float cote = fmin(w, h) - marge;
  uint colonne = game_nb_cols(env->g);
  uint ligne = game_nb_rows(env->g);
  float x_grille = cote - marge;
  float y_grille = cote - marge;
  uint case_x = x_grille / colonne;
  uint case_y = y_grille / ligne;
  int tab_x[colonne + 1];
  int tab_y[ligne + 1];
  // INITIALISATION D'UN TABLEAU DE COORDONNÉES EN X ET Y
  for (uint i = 0; i < colonne + 1; i++) {
    tab_x[i] = marge + (i * case_x);
  }
  for (uint i = 0; i < ligne + 1; i++) {
    tab_y[i] = marge + (i * case_y);
  }

  // Placer Light sur la case cliqué si elle correspond à une case
  if (tab_x[0] < mouse_x && tab_y[0] < mouse_y) {
    for (uint i = 0; i < colonne; i++) {
      for (uint j = 0; j < ligne; j++) {
        if (tab_x[i + 1] > mouse_x && mouse_x >= tab_x[i] &&
            tab_y[j + 1] > mouse_y && mouse_y >= tab_y[j]) {
          if (game_get_state(env->g, j, i) == S_LIGHTBULB) {
            game_play_move(env->g, j, i, S_BLANK);
          } else if (game_is_black(env->g, j, i) == true) {
            printf("warning\n");
          } else {
            game_play_move(env->g, j, i, S_LIGHTBULB);
          }
        }
      }
    }
  }
}

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_RenderCopy(ren, env->background, NULL, NULL); /* put the background */
  PlacerCase(ren, env, w, h);
  TraceGrille(ren, env, w, h);
}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  if (e->type == SDL_QUIT) {
    return true;
    // Intéraction à la souris.
  } else if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    PlacerLight(win, ren, env, mouse.x, mouse.y);
    if (game_is_over(env->g)) {
      printf("WIN !!\n");
    }
    // Option complémentaires...
  } else if (e->type == SDL_KEYDOWN) {
    switch (e->key.keysym.sym) {
      // Restart
      case SDLK_r:
        game_restart(env->g);
        break;
      // Undo
      case SDLK_z:
        if (!game_is_over(env->g)) {
          game_undo(env->g);
        }
        break;
      // Redo
      case SDLK_y:
        if (!game_is_over(env->g)) {
          game_redo(env->g);
        }
        break;
      // Solve
      case SDLK_s:
        if (!game_is_over(env->g)) {
          game_solve(env->g);
        }
        break;
      // Quit
      case SDLK_q:
        return true;
        break;
    }
    return false;
  }
  return false;
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  /* PUT YOUR CODE HERE TO CLEAN MEMORY */

  free(env);
}

/* **************************************************************** */
