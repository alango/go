#ifndef HUMAN_PLAYER_H
#define HUMAN_PLAYER_H

#include "player.h"

class HumanPlayer: public Player
{
public:
  Coordinate get_move(GameState game_state);
private:
  Coordinate request_move();
};

#endif