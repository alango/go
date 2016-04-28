#include <iostream>
#include <vector>

#include "game.h"
#include "game_state.h"
#include "player.h"
#include "mcts_player.h"
#include "test.h"

int main()
{
  ko_test();
  eye_test();
  srand(time(NULL));
  // MCTSNode_test();
  int black_wins = 0;
  for (int i=0; i != 1; i++)
  {
    Game game;
    black_wins += game.get_result();
    std::cout << "played: " << i << "; black wins: " << black_wins << std::endl;
  }
  return 0;
}