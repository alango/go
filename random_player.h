#ifndef RANDOM_PLAYER_H
#define RANDOM_PLAYER_H

#include "player.h"

class RandomPlayer: public Player
{
private:
  list_of_points possible_moves;
public:
  RandomPlayer();
  ~RandomPlayer();
  Coordinate get_move(GameState game_state);
};

#endif