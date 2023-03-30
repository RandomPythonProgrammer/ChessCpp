#include "board.h"
#include "util.h"
#include <iostream>

using namespace std;

Board::Board() {
	uint64_t pawns = 0b11111111ULL << 8;

	uint64_t bishops = 0b00100100;
	uint64_t knights = 0b01000010;
	uint64_t rooks = 0b10000001;
	uint64_t queens = 0b00010000;
	uint64_t kings = 0b00001000;


	board = new uint64_t[]{
		pawns, bishops, knights, rooks, queens, kings,
		pawns << 40, bishops << 56, knights << 56, rooks << 56, queens << 56, kings << 56
	};
}

void Board::get_white(uint64_t& mask) {
	for (int i = 0; i < 6; i++) {
		mask |= board[i];
	}
}

void Board::get_black(uint64_t& mask) {
	for (int i = 6; i < 12; i++) {
		mask |= board[i];
	}
}

void Board::get_all(uint64_t& mask) {
	get_white(mask);
	get_black(mask);
}

void Board::knight_attack(const uint8_t& position, uint64_t& mask) {
	mask = 0;
	uint64_t attack = knight_table[position];
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(w);
	get_black(b);
	uint64_t c = 1ULL << position & w ? w : b;

	mask |= (attack & ~c);
}

void Board::king_attack(const uint8_t& position, uint64_t& mask) {
	//castling needs to be added
	mask = 0;
	uint64_t attack = king_table[position];
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(w);
	get_black(b);
	uint64_t c = 1ULL << position & w ? w : b;

	mask |= (attack & ~c);
}

void Board::pawn_attack(const uint8_t& position, uint64_t& mask) {
	//Needs en passant
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(w);
	get_black(b);

	uint64_t a = w | b;
	uint64_t m = 1ULL << position;

	mask = 0;

	if (m & w) {
		if ((m <<= 8) & ~a) {
			mask |= m;
			if (m & 16711680 && (m <<= 8) & ~a) {
				mask |= m;
			}
		}
		mask |= wpc_table[position] & b;
	}
	else {
		if ((m >>= 8) & ~a) {
			mask |= m;
			if (m & 280375465082880 && (m >>= 8) & ~a) {
				mask |= m;
			}
		}
		mask |= bpc_table[position] & w;
	}
}

void Board::bishop_attack(const uint8_t& position, uint64_t& mask) {
	mask = 0;
	uint64_t _pos = 1ULL << position;
	uint64_t pos = _pos;
	bool hit = false;
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(w);
	get_black(b);
	uint64_t a = w | b;
	uint64_t o = pos & w ? b : w;

	uint8_t ipos = position;
	uint8_t y = ipos >> 3;
	uint8_t x = ipos % 8;

	uint8_t y_ = y;
	uint8_t x_ = x;

	for (int i = 0; i < 7; i++) {
		y_++;
		x_--;
		if (y > 7 || x < 0) {
			break;
		}
		pos <<= 7;
		if (o & pos) {
			if (hit) {
				break;
			}
			else {
				hit = true;
			}
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
	}

	pos = _pos;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_++;
		x_++;
		if (y > 7 || x > 7) {
			break;
		}
		pos <<= 9;
		if (o & pos) {
			if (hit) {
				break;
			}
			else {
				hit = true;
			}
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
	}

	pos = _pos;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_--;
		x_--;
		if (y < 0 || x < 0) {
			break;
		}
		pos >>= 9;
		if (o & pos) {
			if (hit) {
				break;
			}
			else {
				hit = true;
			}
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
	}

	pos = _pos;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_--;
		x_++;
		if (y < 0 || x > 7) {
			break;
		}
		pos >>= 7;
		if (o & pos) {
			if (hit) {
				break;
			}
			else {
				hit = true;
			}
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
	}

	//There must be an easier way to do this
}

void Board::rank_attack(const uint8_t& position, uint64_t& mask) {
	uint8_t line = 0b11111111;
	uint64_t pos = 1ULL << position;

	uint64_t w = 0;
	uint64_t b = 0;

	uint8_t y = position >> 3 << 3;
	uint8_t x = position % 8;
	uint8_t pos_line = pos >> y;

	get_white(w);
	get_black(b);
	uint64_t a = w | b;
	uint8_t o = (pos & w ? b : w) >> y;

	uint8_t rank = line ^ a >> y;
	uint8_t right = rank & (uint8_t)(line >> 8 - x);
	uint8_t left = rank & (uint8_t)(line << x + 1);
	uint8_t rfirst = countl_one((uint8_t)(right << 8 - x));
	uint8_t lfirst = countr_one((uint8_t)(left >> x + 1));

	left &= line >> (7 - x - lfirst);
	left |= (pos_line << lfirst + 1) & o;
	right &= line << (x - rfirst);
	right |= (pos_line >> rfirst + 1) & o;
	mask = (left | right);
	mask <<= y;
}

void Board::file_attack(const uint8_t& position, uint64_t& mask) {
	uint64_t rpos = 1ULL << position;
	rotate_right(rpos); // Todo: rewrite without rotation (its probably faster)
	uint8_t pos = ilog2(rpos); // should figure out a way to optimize this one out
	uint8_t line = 0b11111111;

	uint64_t w = 0;
	uint64_t b = 0;

	uint8_t y = pos >> 3 << 3;
	uint8_t x = pos % 8;
	uint8_t pos_line = rpos >> y;

	get_white(w);
	get_black(b);
	rotate_right(w);
	rotate_right(b);
	uint64_t a = w | b;
	uint8_t o = (rpos & w ? b : w) >> y;

	uint8_t rank = line ^ a >> y;
	uint8_t right = rank & (uint8_t)(line >> 8 - x);
	uint8_t left = rank & (uint8_t)(line << x + 1);
	uint8_t rfirst = countl_one((uint8_t)(right << 8 - x));
	uint8_t lfirst = countr_one((uint8_t)(left >> x + 1));

	left &= line >> (7 - x - lfirst);
	left |= (pos_line << lfirst + 1) & o;
	right &= line << (x - rfirst);
	right |= (pos_line >> rfirst + 1) & o;
	mask = (left | right);
	mask <<= y;
	rotate_left(mask);
}

void Board::rook_attack(const uint8_t& position, uint64_t& mask) {
	uint64_t r = 0;
	uint64_t f = 0;
	rank_attack(position, r);
	file_attack(position, f);
	mask = r | f;
}

void Board::get_moves(const uint8_t& position, uint64_t& mask) {
	uint64_t pos = 1ULL << position;
	for (int i = 0; i < 6; i++) {
		uint64_t wboard = board[i];
		uint64_t bboard = board[i + black];
		uint64_t r = 0;
		uint64_t b = 0;
		if (wboard & pos || bboard & pos) {
			switch (i) {
			case pawns:
				pawn_attack(position, mask);
				break;
			case bishops:
				bishop_attack(position, mask);
				return;
			case knights:
				knight_attack(position, mask);
				break;
			case rooks:
				rook_attack(position, mask);
				return;
			case queens:
				rook_attack(position, r);
				bishop_attack(position, b);
				mask = r | b;
				return;
			case kings:
				king_attack(position, mask);
				break;
			default:
				cout << "invalid piece" << endl;
				break;
			}
		}
	}
}

void Board::get_observers(const uint8_t& position, uint64_t& mask) {
	mask = 0;
	for (int i = 0; i < 12; i++) {
		uint64_t selector = 1ULL;
		for (int j = 0; j < 64; j++, selector <<= 1) {
			uint64_t moves = 0;
			get_moves(j, moves);
			if (moves & selector) {
				mask |= 1ULL << j;
			}
		}
	}
}

bool Board::check(const uint8_t& position, const color_t& color) {
	uint8_t king_pos = countr_zero(board[kings + color == white ? 0 : black]);
	uint64_t observers = 0;
	get_observers(king_pos, observers);
	return observers;
}

void Board::move(const uint64_t& start, uint64_t& dest) {
	for (int i = 11; i >= 0; i--) {
		uint64_t& sub = board[i];
		if (sub & start) {
			sub ^= start;
			if (i == pawns) {
				if (dest & 18374686479671623680) {
					board[queens] |= dest;
					continue;
				}
			}
			else if (i == black + pawns) {
				if (dest & 255) {
					board[black + queens] |= dest;
					continue;
				}
			}
			sub |= dest;
		}
		else {
			sub &= ~dest;
		}
	}
}


