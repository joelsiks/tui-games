
#include "dice.h"
#include "roulette.h"
#include "slots.h"
#include "game.h"

int main(int argc, char** argv) {

	bool running = true;
	char tmp_command[80];

	GameData gdata;
	PlayerData pdata(6000);

	Game game(&pdata, &gdata);

	Slots sl(game);
	Roulette rl(game);
	Dice dice(game);

	initscr();
	start_color();
	use_default_colors();
	getmaxyx(stdscr, gdata.mY, gdata.mX);
	raw();

	// The lowest playable width is when gdata.mX > 135.
	if(gdata.mX > 135) {

		UI::print_layout(&gdata, &pdata);
		UI::print_balance(&gdata, pdata.getBalance());

		rl.init();
		sl.init();

		UI::notification(&gdata, "WIN STREAK: " + std::to_string(pdata.getStreak()));
		UI::clear_command_input(&gdata);

		while (running) {

			getnstr(tmp_command, gdata.input_length);
			std::string command(tmp_command);

			if(command == "q" || command == "quit" || command == "exit") running = false;
			else if(command == "r" || command == "roulette") rl.start_game();
			else if(command == "s" || command == "slots") sl.start_game();
			else if(command == "d" || command == "dice") dice.start_game();
			else if(command == "bal") game.setBalance();
			else if(command == "clear") {
				UI::clear_middle(&gdata); // Clear the middle input.
				UI::clear_game_end_status(&gdata);
			}

			UI::clear_command_input(&gdata);
		}

	} else {
		std::string message = "Your terminal window is too small. Please zoom out (CTRL + (-)) or resize your terminal.";
		std::string message2 = "Press any key to exit.";
		mvprintw((gdata.mY-1) / 2, (gdata.mX - message.length()) / 2, message.c_str());
		mvprintw(((gdata.mY-1) / 2) + 2, (gdata.mX - message2.length()) / 2, message2.c_str());
		move(0, 0);
		getch();
	}

	endwin();

	return 0;
}
