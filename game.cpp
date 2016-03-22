#include <iostream>
#include "game.h"

Game::Game()
{
  // game_state.place_handicap_stones(3);
  while (!game_state.game_over)
  {
    turn();
  }
  game_state.print();
  std::cout << "Final score: " << game_state.score_game() << std::endl;
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
  game_state.print_game_record();
}
