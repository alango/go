#ifndef PLAYER_H
#define PLAYER_H

#include "game_state.h"
#include "neural_net.h"

class Player
{
public:
  virtual Coordinate get_move(GameState game_state) = 0;
};

class RandomPlayer: public Player
{
private:
  list_of_points all_points;
public:
  RandomPlayer();
  ~RandomPlayer();
  Coordinate get_move(GameState game_state);
};

class HumanPlayer: public Player
{
public:
  Coordinate get_move(GameState game_state);
private:
  Coordinate request_move();
};

class NeuralNetPlayer: public Player
{
private:
  NeuralNet net;
public:
  NeuralNetPlayer();
  ~NeuralNetPlayer();
  Coordinate get_move(GameState game_state);
};

#endif