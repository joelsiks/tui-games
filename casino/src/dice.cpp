
#include "dice.h"

Dice::Dice(const Game &g) : Game(g) {}

// roll paramater given as an integer rangning between 1-6. Delay is given in seconds.
void Dice::print_dice(int roll[]) {

	int start_x = (_gdata->mX - (3 * m_dice_width) - (2 * m_dice_padding)) / 2 + 1;
	int start_y = _gdata->mY / 2 + (_gdata->mY/2 - m_dice_height) / 2;

	for(int i = 0; i < 3; i++) {
		mvprintw(start_y,   start_x, "+----+---+----+");
		mvprintw(start_y+1, start_x, "|             |");
		mvprintw(start_y+2, start_x, "|  %c   %c   %c  |", m_sides[roll[i]-1][0], m_sides[roll[i]-1][1], m_sides[roll[i]-1][2]);
		mvprintw(start_y+3, start_x, "|             |");
		mvprintw(start_y+4, start_x, "|             |");
		mvprintw(start_y+5, start_x, "|  %c   %c   %c  |", m_sides[roll[i]-1][3], m_sides[roll[i]-1][4], m_sides[roll[i]-1][5]);
		mvprintw(start_y+6, start_x, "|             |");
		mvprintw(start_y+7, start_x, "+----+---+----+");

		start_x += (15 + m_dice_padding);
		UI::clear_command_input(_gdata);

		refresh();
		usleep(200000);
	}
}

void Dice::roll_dice(int rolls[]) {

	clear_board();

	std::random_device rd;
	std::mt19937 rnd(rd());
	std::uniform_int_distribution<int> random_int(1, 6);

	int start_x = (_gdata->mX - (3 * m_dice_width) - (2 * m_dice_padding)) / 2 + 1;
	int start_y = _gdata->mY / 2 + (_gdata->mY/2 - m_dice_height) / 2;

	// Print out some random rolls to make it look like the dice has "rolled" on the "table" a few times.
	for(int i = 0; i < 8; i++) {
		int random_roll[3] = { random_int(rnd), random_int(rnd), random_int(rnd) };
		print_dice(random_roll);
	}

	// Print out the final result.
	print_dice(rolls);
}

void Dice::clear_board() {
	// TODO: Inefficient?
	for(int i = _gdata->mY/2+1; i < _gdata->mY-2; i++) {
		for(int j = 0; j < _gdata->mX; j++) {
			mvprintw(i, j, " ");
		}
	}
}

void Dice::print_selection_menu(int selected) {

	int ty = _gdata->mY / 2 + 6;
	for(int i = 0; i < length(_alternatives); i++) {
		mvprintw(ty, 5, _alternatives[i].c_str());
		ty += 2;
	}

	mvprintw(_gdata->mY / 2 + 6 + (selected) * 2, 6, "X");
}

void Dice::start_game() {

	// Clear the board.
	clear_board();

	// Initializing rng.
	std::random_device rd;
	std::mt19937 rnd(rd());
	std::uniform_int_distribution<int> random_int(1, 6);

	int bet;
	int amnt;
	char tmp_amnt[5];

	// Clear the end status from roulette and slots.
	UI::clear_game_end_status(_gdata);

	UI::print_middle(_gdata, "ENTER BET: ");

	getnstr(tmp_amnt, 5);
	UI::clear_middle(_gdata);

	std::string bet_amount(tmp_amnt);

	try {
		amnt = std::stoi(bet_amount);
	} catch(std::invalid_argument) {
		UI::print_middle(_gdata, "INVALID INPUT");
		return;
	}

	if(!_pdata->hasAmount(amnt)) {
		UI::print_middle(_gdata, "NOT ENOUGH BALANCE");
		return;
	}

	setBalance(_pdata->getBalance() - amnt);
	UI::print_balance(_gdata, _pdata->getBalance());
	bet = amnt;

	// PRINT OUT BET ALTERNATIVES.
	int selected_alternative = 0;
	print_selection_menu(selected_alternative);
	UI::clear_command_input(_gdata);

	char key_press;
	bool islooping = true;

	while(islooping) {

		key_press = getch();

		switch(key_press) {
			case Keys::UP:
				selected_alternative-- > 0 ? selected_alternative : selected_alternative++;
				break;
			case Keys::DOWN:
				selected_alternative++ < length(_alternatives)-1 ? selected_alternative : selected_alternative--;
				break;
			case Keys::ENTER:
				islooping = false;
				break;
		}

		print_selection_menu(selected_alternative);
		UI::clear_command_input(_gdata);

		// Debug.
		//UI::print_middle(_gdata, std::to_string(selected_alternative));
	}

	int rolls[3] = {
		random_int(rnd),
		random_int(rnd),
		random_int(rnd),
	};

	roll_dice(rolls);

	int total = rolls[0] + rolls[1] + rolls[2];

	std::string status_message = "You lost.";
	int win_multiplier = 0;

	switch(selected_alternative) {
		case 0: // Even numbers.
			if(total % 2 == 0) {
				status_message = "You won! " + std::to_string(total) + " is even.";
				win_multiplier = 2;
			}
			break;
		case 1: // Odd numbers.
			if(total % 2 == 1) {
				status_message = "You won! " + std::to_string(total) + " is odd.";
				win_multiplier = 2;
			}
			break;
		case 2: // Three of a kind
			if(rolls[0] == rolls[1] && rolls[1] == rolls[2]) {
				status_message = "You scored three of a kind. Lets go!";
				win_multiplier = 6;
			}
			break;
		case 3: // Above 12
			if(total > 12) {
				status_message = "Total is above 12. CONGRATULATIONS!";
				win_multiplier = 2;
			}
			break;
		case 4: // Below 10
			if(total < 10) {
				status_message = "Total is below 10. Eureka!";
				win_multiplier = 2;
			}
			break;
		case 5: // Specific number.
			break;
	};

	if(win_multiplier != 0) {
		_pdata->setBalance(_pdata->getBalance() + (bet * win_multiplier));
		_pdata->incrementStreak();
	} else {
		_pdata->discardStreak();
	}

	// Output the status message.
	mvprintw(_gdata->mY-4, (_gdata->mX/2) - (status_message.size()/2), status_message.c_str());

	updateStreakCounter();
}

