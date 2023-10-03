#pragma once

#include <string>
#include <stdexcept>
#include <random>

#include <unistd.h>

#include "game.h"

class Dice : public Game {
private:
	void end_game();

	void print_selection_menu(int selected);
	void clear_board();

	void print_dice(int roll[]);
	void roll_dice(int rolls[]);

	const int m_dice_padding = 8;
	const int m_dice_height  = 8;
	const int m_dice_width   = 15;

	const char m_sides[6][6] = { {'X', ' ', ' ', ' ', ' ', ' '}, {'X', 'X', ' ', ' ', ' ', ' '}, {'X', 'X', 'X', ' ', ' ', ' '}, {'X', 'X', 'X', 'X', ' ', ' '}, {'X', 'X', 'X', 'X', 'X', ' '}, {'X', 'X', 'X', 'X', 'X', 'X'} };
	const std::string _alternatives[6] = {
		"[ ] Even numbers",
		"[ ] Odd numbers",
		"[ ] Three of a kind",
		"[ ] Above 12",
		"[ ] Below 10",
		"[ ] Specific number",
	};

public:
	Dice(const Game &g);
	void start_game();
};

