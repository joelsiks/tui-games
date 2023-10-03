
#include "game.h"
#include "roulette.h"

void Roulette::change_option(int arr[]) {
	for(int i = 0; i < sizeof(*arr); i++) {
		mvprintw(_gdata->mY-1, arr[i], " ");
	}

	mvprintw(_gdata->mY-1, arr[m_current_choice], "X");
	UI::clear_command_input(_gdata);
}

void Roulette::spin(int nspins, int sec) {
	for(int i = 0; i < nspins; i++) {
		UI::clear_command_input(_gdata);

		refresh();
		usleep(sec * 1000);

		// Shift the array values.
		int tmp = m_slotList[0];
		m_slotList.erase(m_slotList.begin());
		m_slotList.push_back(tmp);

		print();
	}
}

void Roulette::init() {
	m_slotList = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};

	m_row = (_gdata->mY/4) - 1;
	m_square_max_length = _gdata->mX/2;

	print();
	mvprintw(m_row+1, ((m_square_max_length - (m_slotList.size() * 3)) / 2) + (m_slotList.size() * 3) / 2 - 1, "^^");
}

void Roulette::print() {

	int xpos = (m_square_max_length - (m_slotList.size() * 3)) / 2;

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_RED);
	init_pair(3, COLOR_WHITE, COLOR_GREEN);

	for(int i = 0; i < m_slotList.size(); i++) {

		std::string number = "";
		int pair_number;

		if(m_slotList[i] == 0) pair_number = 3;
		else pair_number = (m_slotList[i] % 2) ? 1 : 2;

		if(m_slotList[i] < 10) number += "0";
		number += std::to_string(m_slotList[i]);

		attron(COLOR_PAIR(pair_number));
		mvprintw(m_row, xpos, number.c_str());
		attroff(COLOR_PAIR(pair_number));

		xpos += 3;
	}
}

void Roulette::start_game() {

	// Initializing rng.
	std::random_device rd;
	std::mt19937 rnd(rd());
	std::uniform_int_distribution<int> random_int(20, m_slotList.size());

	int amnt;
	char tmp_amnt[80];
	std::string options_str = "[ ] RED [ ] BLACK [ ] GREEN";
	m_current_choice = 1;

	// Clear the end status from roulette and slots.
	UI::clear_game_end_status(_gdata);

	UI::print_middle(_gdata, "BET: ");
	
	getnstr(tmp_amnt, 5);
	std::string bet_amount(tmp_amnt);

	try {
		amnt = std::stoi(bet_amount);
	} catch(std::invalid_argument) {
		UI::print_middle(_gdata, "INVALID INPUT");
		return;
	}

	if(!_pdata->hasAmount(amnt)) {
		UI::print_middle(_gdata, "NOT ENOUGH MONEY");
		return;
	}

	// Update current balance and output it to screen.
	setBalance(_pdata->getBalance() - amnt);
	UI::print_balance(_gdata, _pdata->getBalance());

	// Sets member variable for reference later.
	m_current_bet = amnt;

	// Prints out options that are available.
	UI::print_middle(_gdata, options_str);
	UI::clear_command_input(_gdata);

	int keyboard_value = 0; // The holder for the pressed key value.
	int POS1 = ((_gdata->mX/2) - (options_str.length()/2))+1; // Needs to be seperate from array to prevent error.
	int positions[3] = {POS1, _gdata->mX/2 - 4, _gdata->mX/2 + 6}; // Positions on screen for the 'X's.

	// Output the standard selected value.
	mvprintw(_gdata->mY-1, positions[m_current_choice], "X");
	UI::clear_command_input(_gdata);

	while(keyboard_value != Keys::ENTER) {
		keyboard_value = getch();

		switch(keyboard_value) {
			case Keys::LEFT:
				if(m_current_choice != 0) m_current_choice -= 1;
				Roulette::change_option(positions);
				break;

			case Keys::RIGHT:
				if(m_current_choice != 2) m_current_choice += 1;
				Roulette::change_option(positions);
				break;

			case Keys::ONE:
				m_current_choice = 0;
				Roulette::change_option(positions);
				break;

			case Keys::TWO:
				m_current_choice = 1;
				Roulette::change_option(positions);
				break;

			case Keys::THREE:
				m_current_choice = 2;
				Roulette::change_option(positions);
				break;

			default:
				UI::clear_command_input(_gdata);
				break;
		}
	}

	// Clear the middle screen of the selection menu.
	UI::clear_middle(_gdata);

	// random_int returns an integer between 20-(size of slots array), giving all possibilities equal chance of occurring.
	int spins = random_int(rnd);

	// Hardcoded fast spins.
	Roulette::spin(15, 90);
	Roulette::spin(7, 120);
	Roulette::spin(5, 170);

	for(int i = spins; i > 0; i--) {
		if(i > 13)      Roulette::spin(1, 200);
		else if(i > 10) Roulette::spin(1, 240);
		else if(i > 6)  Roulette::spin(1, 280);
		else if(i > 4)  Roulette::spin(1, 320);
		else            Roulette::spin(1, 640);
	}

	Roulette::end_game();
}

void Roulette::end_game() {

	UI::clear_command_input(_gdata);

	if(m_slotList[11] % 2 && m_current_choice == 1) { // BLACK
		setBalance(_pdata->getBalance() + (m_current_bet * 2));
		_pdata->incrementStreak();
		mvprintw((_gdata->mY/2)-3, (_gdata->mX/2) / 10, "LANDED ON: %d - YOU WON!", m_slotList[11]);
	} else if(!(m_slotList[11] % 2) && m_current_choice == 0) { // RED
		setBalance(_pdata->getBalance() + (m_current_bet * 2));
		_pdata->incrementStreak();
		mvprintw((_gdata->mY/2)-3, (_gdata->mX/2) / 10, "LANDED ON: %d - YOU WON!", m_slotList[11]);
	} else if(m_slotList[11] == 0 && m_current_choice == 2) { // GREEN
		setBalance(_pdata->getBalance() + (m_current_bet * 8));
		_pdata->incrementStreak();
		mvprintw((_gdata->mY/2)-3, (_gdata->mX/2) / 10, "LANDED ON: %d - YOU WON!", m_slotList[11]);
	} else {
		_pdata->discardStreak();
		mvprintw((_gdata->mY/2)-3, (_gdata->mX/2) / 10, "LANDED ON: %d - You lost.", m_slotList[11]);
	}

	updateStreakCounter();

	m_current_choice = 1;
	m_current_bet = 0;
}
