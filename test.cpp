#include <assert.h>
#include <iostream>
#include "test.h"
#include "game_state.h"

void ko_test()
{
  GameState game;
  Coordinate move;

  move.x = 4;
  move.y = 4;
  game.play_move(move);

  move.x = 3;
  move.y = 4;
  game.play_move(move);

  move.x = 3;
  move.y = 3;
  game.play_move(move);

  move.x = 2;
  move.y = 3;
  game.play_move(move);

  move.x = 3;
  move.y = 5;
  game.play_move(move);

  move.x = 2;
  move.y = 5;
  game.play_move(move);

  move.x = 8;
  move.y = 8;
  game.play_move(move);

  move.x = 1;
  move.y = 4;
  game.play_move(move);

  move.x = 2;
  move.y = 4;
  game.play_move(move);

  move.x = 3;
  move.y = 4;
  // Shouldn't be able to take the ko.
  assert(game.is_legal(move) == KO_POINT);

  move.x = 8;
  move.y = 7;
  game.play_move(move);

  move.x = 8;
  move.y = 0;
  game.play_move(move);
  
  move.x = 3;
  move.y = 4;
  // Should now be able to take the ko.
  assert(game.is_legal(move) == LEGAL_MOVE);

  std::cout << "Ko test passed" << std::endl;
}

void eye_test()
{
  GameState game;
  Coordinate move;

  move.x = 4;
  move.y = 4;
  game.play_move(move);

  move.x = 1;
  move.y = 0;
  game.play_move(move);

  move.x = 4;
  move.y = 6;
  game.play_move(move);

  move.x = 0;
  move.y = 1;
  game.play_move(move);
  // False eye in the corner.
  assert(game.is_eye(move) == false);

  move.x = 5;
  move.y = 5;
  game.play_move(move);

  move.x = 1;
  move.y = 1;
  game.play_move(move);
  
  // Real eye in the corner.
  move.x = 0;
  move.y = 0;
  assert(game.is_eye(move) == true);

  move.x = 3;
  move.y = 5;
  game.play_move(move);

  move.x = 2;
  move.y = 1;
  game.play_move(move);

  move.x = 3;
  move.y = 4;
  game.play_move(move);

  move.x = 3;
  move.y = 0;
  game.play_move(move);
  
  // False eye on the side.
  move.x = 2;
  move.y = 0;
  assert(game.is_eye(move) == false);

  move.x = 5;
  move.y = 6;
  game.play_move(move);
  
  // False eye in the middle.
  move.x = 4;
  move.y = 5;
  assert(game.is_eye(move) == false);

  move.x = 3;
  move.y = 1;
  game.play_move(move);
  
  // Real eye on the side
  move.x = 2;
  move.y = 0;
  assert(game.is_eye(move) == true);

  move.x = 5;
  move.y = 4;
  game.play_move(move);
  
  // Real eye in the middle.
  move.x = 4;
  move.y = 5;
  assert(game.is_eye(move) == true);

  std::cout << "Eye test passed" << std::endl;
}