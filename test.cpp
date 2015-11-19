#include <assert.h>
#include <iostream>
#include <ctime>
#include "test.h"
#include "game_state.h"
#include "mcts_node.h"

void ko_test()
{
  GameState game;
  Coordinate move;

  move.set(4,4);
  game.play_move(move);

  move.set(3,4);
  game.play_move(move);

  move.set(3,3);
  game.play_move(move);

  move.set(2,3);
  game.play_move(move);

  move.set(3,5);
  game.play_move(move);

  move.set(2,5);
  game.play_move(move);

  move.set(8,8);
  game.play_move(move);

  move.set(1,4);
  game.play_move(move);

  move.set(2,4);
  game.play_move(move);

  move.set(3,4);
  // Shouldn't be able to take the ko.
  assert(game.is_legal(move) == KO_POINT);

  move.set(8,7);
  game.play_move(move);

  move.set(7,8);
  game.play_move(move);
  
  move.set(3,4);
  // Should now be able to take the ko.
  assert(game.is_legal(move) == LEGAL_MOVE);

  // Test for case where single stone is captured but larger
  // group can be recaptured (snapback).
  GameState game1;
  move.set(1,0);
  game1.play_move(move);

  move.set(2,0);
  game1.play_move(move);

  move.set(1,1);
  game1.play_move(move);

  move.set(2,1);
  game1.play_move(move);

  move.set(1,2);
  game1.play_move(move);

  move.set(2,2);
  game1.play_move(move);

  move.set(0,2);
  game1.play_move(move);

  move.set(1,3);
  game1.play_move(move);

  move.set(8,8);
  game1.play_move(move);

  move.set(0,3);
  game1.play_move(move);

  move.set(7,8);
  game1.play_move(move);

  move.set(0,0);
  game1.play_move(move);

  move.set(0,1);
  game1.play_move(move);

  move.set(0,0);
  // This shouldn't be a ko.
  assert(game1.is_legal(move) != KO_POINT);

  std::cout << "Ko test passed" << std::endl;
}

void eye_test()
{
  GameState game;
  Coordinate move;

  move.set(4,4);
  game.play_move(move);

  move.set(1,0);
  game.play_move(move);

  move.set(4,6);
  game.play_move(move);

  move.set(0,1);
  game.play_move(move);
  // False eye in the corner.
  assert(game.is_eye(move, EMPTY) == false);

  move.set(5,5);
  game.play_move(move);

  move.set(1,1);
  game.play_move(move);
  
  // Real eye in the corner.
  move.set(0,0);
  assert(game.is_eye(move, EMPTY) == true);

  move.set(3,5);
  game.play_move(move);

  move.set(2,1);
  game.play_move(move);

  move.set(3,4);
  game.play_move(move);

  move.set(3,0);
  game.play_move(move);
  
  // False eye on the side.
  move.set(2,0);
  assert(game.is_eye(move, EMPTY) == false);

  move.set(5,6);
  game.play_move(move);
  
  // False eye in the middle.
  move.set(4,5);
  assert(game.is_eye(move, EMPTY) == false);

  move.set(3,1);
  game.play_move(move);
  
  // Real eye on the side
  move.set(2,0);
  assert(game.is_eye(move, EMPTY) == true);

  move.set(5,4);
  game.play_move(move);
  
  // Real eye in the middle.
  move.set(4,5);
  assert(game.is_eye(move, EMPTY) == true);

  std::cout << "Eye test passed" << std::endl;
}

void MCTSNode_test()
{
  GameState game_state;
  MCTSNode mcts_node(game_state);
  std::clock_t start;
  start = std::clock();
  for (int i = 0; i < 10000; i++)
  {
    mcts_node.simulate_and_update();
  }
  double duration = (std::clock()-start) / (double) CLOCKS_PER_SEC;
  std::cout << "Time for 10000 simulations: " << duration << std::endl;

  start = std::clock();
  mcts_node.expand();
  duration = (std::clock()-start) / (double) CLOCKS_PER_SEC;
  std::cout << "Time to expand node: " << duration << std::endl;
  // mcts_node.print_visit_map();
  // mcts_node.print_uct_map();
}