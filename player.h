#ifndef PLAYER_H
#define PLAYER_H

#include "game_state.h"

class Player
{
  player colour;
public:
  void set_colour(player colour)
  {
    this->colour = colour;
  }
  virtual Coordinate get_move(GameState game_state) = 0;
};

#endif