#include <iostream>
#include "game.h"

Game::Game()
{
  black.set_colour(BLACK);
  white.set_colour(WHITE);
  while (!game_state.game_over)
  {
    turn();
  }
  game_state.print();
  game_state.score_game();
  game_state.print_game_record();
}

Game::~Game() {};

void Game::turn()
{
  Coordinate move;
  // game_state.print();
  if (game_state.to_play == BLACK)
  {
    move = black.get_move(game_state);
  }
  else if (game_state.to_play == WHITE)
  {
    move = white.get_move(game_state);
  }
  move.print();
  game_state.play_move(move);
}