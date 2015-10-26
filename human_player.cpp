#include <iostream>
#include "human_player.h"

Coordinate HumanPlayer::request_move()
{
	Coordinate coordinate;
	std::cout << "Enter x co-ordinate:" << std::endl;
	std::cin >> coordinate.x;
	std::cout << "Enter y co-ordinate:" << std::endl;
	std::cin >> coordinate.y;
	return coordinate;
}

Coordinate HumanPlayer::get_move(GameState game_state)
{
	Coordinate move;
	bool legal_move = false;
	is_legal_responses is_legal_response;
	while (!legal_move)
	{
		move = request_move();
		is_legal_response = game_state.is_legal(move);
		if (is_legal_response == LEGAL_MOVE)
		{
			legal_move = true;
		}
		else if (is_legal_response == POINT_NOT_ON_BOARD)
		{
			std::cout << "That is not a point on the board" << std::endl;
		}
		else if (is_legal_response == POINT_OCCUPIED)
		{
			std::cout << "There is already a stone on that point" << std::endl;
		}
		else if (is_legal_response == KO_POINT)
		{
			std::cout << "Ko rule: You must play elsewhere first" << std::endl;
		}
		else if (is_legal_response == SUICIDE)
		{
			std::cout << "That move is suicide" << std::endl;
		}
	}
	return move;
}