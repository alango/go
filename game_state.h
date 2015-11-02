#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <iostream>
#include <vector>

#define BOARD_SIZE 9

class Coordinate
{
public:
  int x;
  int y;

  Coordinate();
  Coordinate(int x, int y);
  ~Coordinate();
  void set(int x, int y);
  void print();
};

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
  // Prints the current state of the game.
  void print();
  // Returns the state of the point on the board.
  player get_point(Coordinate point);
  // Checks if the coordinate is actually on the board.
  bool point_on_board(Coordinate coordinate);
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
  // Checks if a point is a true eye or not for the player of the specified colour.
  // if the colour is EMPTY, then it checks if the point is an eye for either player.
  bool is_eye(Coordinate point, player eye_colour);
  // Checks if a game is finished. A game ends when the only empty points remaining
  // on the board are true eyes.
  bool game_finished();
  // Returns the difference between black and white's scores. Only guaranteed to
  // work if game_finished returns true.
  int score_game();
};

#endif