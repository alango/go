#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

#define BOARD_SIZE 9

typedef struct
{
  int x, y;
} Coordinate;

bool operator==(Coordinate point1, Coordinate point2);

typedef std::vector<Coordinate> list_of_points;

enum player{ EMPTY, BLACK, WHITE };

enum is_legal_responses{ LEGAL_MOVE, POINT_NOT_ON_BOARD, POINT_OCCUPIED, KO_POINT, SUICIDE };

class GameState
{
private:
  player board_state[BOARD_SIZE][BOARD_SIZE];
  player to_play;
  player other_player;
  Coordinate ko;

public:
  GameState();
  ~GameState();
  // Request moves until a legal one is submitted, and then plays it.
  void human_move();
  // Place stone and update the board.
  void play_move(Coordinate move);
  void print();

private:
  // Asks the user to input a move.
  Coordinate request_move();
  // Checks if the coordinate is actually on the board.
  bool point_on_board(Coordinate coordinate);
  // Checks if the point is on the board and is empty.
  bool point_unoccupied(Coordinate coordinate);
  // Returns a list of the adjacent points that are on the board.
  list_of_points get_adjacent_points(Coordinate coordinate);
  // Returns the number liberties of the group that the point belongs to.
  int liberties_on_group(Coordinate coordinate);
  // Removes all stones from the board that are connected to the specified point.
  int remove_captures(Coordinate point);
  // Checks if a move is illegal due to being suicidal.
  bool is_suicide(Coordinate move);
  // Returns 0 if a move is legal, and the appropriate response code otherwise.
  is_legal_responses is_legal(Coordinate move);
};

#endif