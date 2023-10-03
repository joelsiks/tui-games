
#include "slots.h"

Slots::Slots(const Game &g) : Game(g), m_slots(9) {

	int tempIndex = 0;

	m_slots[0] = "ZZZ";
	m_slots[1] = "YYY";
	m_slots[2] = "ZZZ";
	m_slots[3] = "XXX";
	m_slots[4] = "XXX";
	m_slots[5] = "YYY";
	m_slots[6] = "YYY";
	m_slots[7] = "ZZZ";
	m_slots[8] = "XXX";

}

void Slots::init() {
	print();
}

void Slots::print() {

	// The offset of which the arrow ">" indicators will be placed away from the slot machine.
	int offset = 6;

	// calculates the distace of the beginning x & y positions that the slot machine will start from. i.e x = 0; y = 0;
	int holder_x = (_gdata->mX/2+1) + ((_gdata->mX/2-27)/2);
	int holder_y = (((_gdata->mY/2) - (27)) / 2) + 6;

	// The holders of the distance values of which the slot "tiles" will be distanced from eachother.
	int tempIncrement_x = 0;
	int tempIncrement_y = 0;

	// Temporary index for the slots inside the loop.
	int tempIndex = 0;

	// Colors for the slot icons.
	init_pair(2, COLOR_WHITE, COLOR_RED);
	init_pair(3, COLOR_WHITE, COLOR_GREEN);
	init_pair(4, COLOR_WHITE, COLOR_BLUE);

	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {

			int pair_code;

			if(m_slots[tempIndex] == "XXX") {
				pair_code = 4;
			} else if(m_slots[tempIndex] == "YYY") {
				pair_code = 2;
			} else if(m_slots[tempIndex] == "ZZZ") {
				pair_code = 3;
			}

			attron(COLOR_PAIR(pair_code));
			mvprintw(holder_y + tempIncrement_y, holder_x + tempIncrement_x, m_slots[tempIndex].c_str());
			mvprintw(holder_y+1 + tempIncrement_y, holder_x + tempIncrement_x, m_slots[tempIndex].c_str());
			attroff(COLOR_PAIR(pair_code));

			tempIncrement_x += 11;
			tempIndex += 1;
		}

		if (!m_printedNecessaryContent) {
			mvprintw(holder_y + tempIncrement_y, holder_x - offset, ">");
			mvprintw(holder_y+1 + tempIncrement_y, holder_x - offset, ">");
		}

		tempIncrement_x = 0;
		tempIncrement_y += 5;
	}

	if(!m_printedNecessaryContent) m_printedNecessaryContent = true;
}

void Slots::shuffle_one(int column) {
	if(column < 3) {
		int i = column;

		std::string temp = m_slots[i];
		m_slots[i] = m_slots[i+6];
		m_slots[i+6] = m_slots[i+3];
		m_slots[i+3] = temp;
	}
}

void Slots::spin() {

	std::random_device rd;
	std::mt19937 rnd(rd());

	std::uniform_int_distribution<int> random_high_int(55, 85);
	std::uniform_int_distribution<int> random_low_int(1, 8);

	int randomBase = random_high_int(rnd);
	int randomSmallBaseOne = random_low_int(rnd);
	int randomSmallBaseTwo = random_low_int(rnd);

	for(int i = 0; i < randomBase; i++) {
		usleep(100 * 1000);

		if(i < randomBase - 10) {
			shuffle_one(0);
		}

		if(i < randomBase - randomSmallBaseOne - 2) {
			shuffle_one(1);
		}

		if(i < randomBase - randomSmallBaseTwo) {
			shuffle_one(2);
		}

		print(); // Update the newly moved/shifted slots in the array to the screen.
		UI::clear_command_input(_gdata);
		refresh(); // Refresh the actual screen to push the content.
	}

}

void Slots::start_game() {

	int amnt;
	char tmp_amnt[80];

	// Clear the end status from roulette and slots.
	UI::clear_game_end_status(_gdata);

	UI::print_middle(_gdata, "BET: ");
	
	getnstr(tmp_amnt, 5);
	std::string bet_amount(tmp_amnt);

	try {
		amnt = std::stoi(bet_amount);
	} catch(std::invalid_argument) {
		// Print error message.
		UI::print_middle(_gdata, "INVALID INPUT");
		return;
	}

	if(!_pdata->hasAmount(amnt)) {
		// Print error message.
		UI::print_middle(_gdata, "NOT ENOUGH MONEY");
		return;
	}

	// Set the bet amount for future reference.
	m_current_bet = amnt;

	// Update the balance.
    setBalance(_pdata->getBalance()-amnt);
	UI::print_balance(_gdata, _pdata->getBalance());

	UI::clear_command_input(_gdata);
	UI::clear_middle(_gdata);

	spin();

	end_game();
}

void Slots::end_game() {

	UI::clear_command_input(_gdata);

	bool won = false;

	if(m_slots[0] == m_slots[1] && m_slots[1] == m_slots[2]) {
		won = true;
	} else if(m_slots[3] == m_slots[4] && m_slots[4] == m_slots[5]) {
		won = true;
	} else if(m_slots[6] == m_slots[7] && m_slots[7] == m_slots[8]) {
		won = true;
	} else if(m_slots[0] == m_slots[4] && m_slots[4] == m_slots[8]) {
		won = true;
	} else if(m_slots[2] == m_slots[4] && m_slots[4] == m_slots[6]) {
		won = true;
	} else {
		_pdata->discardStreak();
		mvprintw((_gdata->mY/2)-3, (_gdata->mX/2) + (_gdata->mX/2) / 10, "No win. Sorry.");
	}

	if(won) {
		setBalance(_pdata->getBalance() + (m_current_bet * WIN_MULTIPLIER));
		_pdata->incrementStreak();
		mvprintw((_gdata->mY/2)-3, (_gdata->mX/2) + (_gdata->mX/2) / 10, "Threee in a row. YOU WON!");
	}

	// TODO: Add a respin/play again option with ENTER key.
	UI::clear_middle(_gdata);
	updateStreakCounter();

	// Reset bet holder.
	m_current_bet = 0;
}

