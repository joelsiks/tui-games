#pragma once

#include <vector>
#include <stdexcept>
#include <random>
#include <unistd.h>

#include "game.h"

class Slots : public Game {
private:
	std::string m_slotIcons[3] = {"ZZZ", "XXX", "YYY"};
	std::vector<std::string> m_slots;
	bool m_printedNecessaryContent = false;
	int m_current_bet;
	void end_game();
	void spin();
	void print();
	void shuffle_one(int);

	static const int WIN_MULTIPLIER = 3;

public:
	Slots(const Game &g);
	void start_game();
	void init();
};
