#include <iostream>
#include "game.h"

Game::Game()
{
  pass.set(-1,-1);
  game_over = false;
  first_pass = false;
  while (!game_over)
  {
    turn();
  }
  game_state.score_game();
  for (list_of_points::iterator move = game_record.begin(); move != game_record.end(); move++)
  {
    move->print();
  }
}

Game::~Game() {};

void Game::turn()
{
  Coordinate move;
  game_state.print();
  if (game_state.to_play == BLACK)
  {
    move = black.get_move(game_state);
  }
  else if (game_state.to_play == WHITE)
  {
    move = white.get_move(game_state);
  }
  move.print();
  if (move == pass)
  {
    // This needs refactoring.
    if (game_state.to_play == BLACK)
    {
      game_state.to_play = WHITE;
      game_state.other_player = BLACK;
    }
    else
    {
      game_state.to_play = BLACK;
      game_state.other_player = WHITE;
    }

    if (first_pass == true)
    {
      game_over = true;
    }
    first_pass = true;
  }
  else
  { 
    game_state.play_move(move);
    first_pass = false;
  }
  game_record.push_back(move);
}