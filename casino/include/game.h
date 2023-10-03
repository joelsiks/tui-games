#pragma once

#include <string>
#include <stdexcept>
#include <random>
#include <curses.h>

// Function used to get the size of a static array.
template<class T, size_t N>
constexpr size_t length(T (&)[N]) { return N; }

namespace Keys {
	static short const ENTER = 10;

	static short const UP    = 65;
	static short const DOWN  = 66;
	static short const RIGHT = 67;
	static short const LEFT  = 68;

	static short const ONE   = 49;
	static short const TWO   = 50;
	static short const THREE = 51;
}

typedef struct GameData {
	std::string balance_str;
	std::string command_str;
	std::string quit_str;
	static const int input_length = 10;
	int mX;
	int mY;
} GameData;

class PlayerData {
private:
	int _balance;
	int _streak = 0;
	int initial_bet;
	int win;

public:
	PlayerData(int balance) : _balance(balance) {}

	inline bool hasAmount(int amount) {
		return _balance >= amount;
	}

	inline void incrementStreak() {
		_streak++;
	}

	inline void discardStreak() {
		_streak = 0;
	}

	inline int getStreak() {
		return _streak;
	}

	inline void setBalance(int bal) {
		_balance = bal;
	}

	inline int getBalance() {
		return _balance;
	}
};

class Game {
public:
	Game(PlayerData *p, GameData *g);

	inline void setBalance(int val) const {
		_pdata->setBalance(val);
	}

	void setBalance();

	void updateStreakCounter();

protected:
	PlayerData *_pdata;
	GameData *_gdata;
};

namespace UI {
	void print_layout(GameData *d, PlayerData *p);
	void print_balance(GameData *d, int bal);
	void print_middle(GameData *d, std::string str);
	void clear_command_input(GameData *d);
	void clear_middle(GameData *d);
	void clear_notification(GameData *d);
	void clear_game_end_status(GameData *d);
	void clear_dice_board(GameData *d);
	void notification(GameData *d, std::string str);
}
