#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <iostream>
#include <vector>

#define BOARD_SIZE 9

typedef struct
{
  int x, y;
  void print()
  {
    std::cout << "(" << x << ", " << y << ")" << std::endl;
  }
} Coordinate;

bool operator==(Coordinate point1, Coordinate point2);

typedef std::vector<Coordinate> list_of_points;

enum player{ EMPTY, BLACK, WHITE };

enum is_legal_responses{ LEGAL_MOVE, POINT_NOT_ON_BOARD, POINT_OCCUPIED, KO_POINT, SUICIDE };

class GameState
{
public:
  player board_state[BOARD_SIZE][BOARD_SIZE];
  player to_play;
  player other_player;
  Coordinate ko;

public:
  GameState();
  ~GameState();
  // Place stone and update the board.
  void play_move(Coordinate move);
  void print();
  // Checks if the coordinate is actually on the board.
  bool point_on_board(Coordinate coordinate);
  // Checks if the point is on the board and is empty.
  bool point_unoccupied(Coordinate coordinate);
  // Returns a list of the adjacent points that are on the board.
  list_of_points get_adjacent_points(Coordinate coordinate);
  // Returns a list of the diagonal points that are on the board.
  list_of_points get_diagonal_points(Coordinate coordinate);
  // Returns the number liberties of the group that the point belongs to.
  int liberties_on_group(Coordinate coordinate);
  // Removes all stones from the board that are connected to the specified point.
  int remove_captures(Coordinate point);
  // Checks if a move is illegal due to being suicidal.
  bool is_suicide(Coordinate move);
  // Returns 0 if a move is legal, and the appropriate response code otherwise.
  is_legal_responses is_legal(Coordinate move);
  // Checks if a point is a true eye or not.
  bool is_eye(Coordinate point);
  // Returns a random legal move that won't fill a true eye.
  // Coordinate random_move();
  // Checks if a game is finished. A game ends when the only empty points remaining
  // on the board are true eyes.
  bool game_finished();
  // Returns the difference between black and white's scores. Only guaranteed to
  // work if game_finished returns true.
  int score_game();
};

#endif