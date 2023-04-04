#include "board.h"
#include "util.h"
#include <iostream>
#include <unordered_map>
#include <future>

using namespace std;

const int EVAL_DEPTH = 5;

Board::Board() {
	previous = nullptr;

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

	bcasle = false;
	wcasle = false;

	bkmove = false;
	wkmove = false;

	brrmove = false;
	wrrmove = false;

	blrmove = false;
	wlrmove = false;
}

Board::Board(Board* previous) {
	this->previous = previous;

	this->board = new uint64_t[12];
	memcpy(board, previous->board, 96);

	bcasle = previous->bcasle;
	wcasle = previous->wcasle;

	bkmove = previous->bkmove;
	wkmove = previous->wkmove;

	brrmove = previous->brrmove;
	wrrmove = previous->wrrmove;

	blrmove = previous->blrmove;
	wlrmove = previous->wlrmove;
}

Board::~Board() {
	delete board;
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
	uint64_t c;
	uint64_t attacks;
	uint64_t a = w | b;

	if (1ULL << position & w) {
		c = w;
		attacked_squares(black, attacks);
		if (!wkmove && !check(white)) {
			if (!wlrmove && !(0b01110000 & (a | attacks))) {
				mask |= 0b10000000;
			}
			if (!wrrmove && !(0b00000110 & (a | attacks))) {
				mask |= 0b00000001;
			}
		}
	}
	else {
		c = b;
		attacked_squares(white, attacks);
		if (!bkmove && !check(black)) {
			if (!blrmove && !(8070450532247928832 & (a | attacks))) {
				mask |= 9223372036854775808;
			}
			if (!brrmove && !(432345564227567616 & (a | attacks))) {
				mask |= 72057594037927936;
			}
		}
	}

	mask |= (attack & ~c);
}

void Board::pawn_attack(const uint8_t& position, uint64_t& mask) {
	//Needs en passant
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(w);
	get_black(b);

	uint64_t a = w | b;
	uint64_t pos = 1ULL << position;
	uint64_t m = pos;

	uint8_t y = position >> 3 << 3;
	uint64_t na = ~a;

	mask = 0;

	if (m & w) {
		//en passant white
		if (m & 1095216660480 && previous) {
			uint8_t epawns = board[black + pawns] >> y;
			epawns &= (uint8_t) ((previous->board[black + pawns] >> y) >> 16);
			mask |= (((uint64_t) epawns) << y) << 8;
		}

		mask |= m = m << 8 & na;
		mask |= m = (m & 16711680) << 8 & na;

		mask |= wpc_table[position] & b;
	}
	else {
		//en passant black
		if (m & 4278190080 && previous) {
			uint8_t epawns = board[pawns] >> y;
			epawns &= (uint8_t)((previous->board[pawns] << 16) >> y);
			mask |= (((uint64_t)epawns) << y) >> 8;
		}

		mask |= m = m >> 8 & na;
		mask |= m = (m & 280375465082880) >> 8 & na;

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

	uint8_t y = position >> 3;
	uint8_t x = position % 8;

	int y_ = y;
	int x_ = x;

	for (int i = 0; i < 7; i++) {
		y_++;
		x_--;
		if (y_ > 7 || x_ < 0) {
			break;
		}
		pos <<= 7;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}

	pos = _pos;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_++;
		x_++;
		if (y_ > 7 || x_ > 7) {
			break;
		}
		pos <<= 9;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}

	pos = _pos;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_--;
		x_--;
		if (y_ < 0 || x_ < 0) {
			break;
		}
		pos >>= 9;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}

	pos = _pos;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_--;
		x_++;
		if (y_ < 0 || x_ > 7) {
			break;
		}
		pos >>= 7;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
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

	uint8_t y = position >> 3;

	int y_ = y;

	//up
	for (int i = 0; i < 7; i++) {
		y_++;
		if (y_ > 7) {
			break;
		}
		pos <<= 8;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}

	pos = _pos;
	hit = false;
	y_ = y;

	//down
	for (int i = 0; i < 7; i++) {
		y_--;
		if (y_ < 0) {
			break;
		}
		pos >>= 8;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}
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

void Board::get_attackers(const uint8_t& position, uint64_t& mask) {
	mask = 0;
	uint64_t b = 0;
	uint64_t w = 0;
	get_black(b);
	get_white(w);
	uint64_t pos = 1ULL << position;
	uint64_t o = pos & w ? b: w;

	int leading;
	while (o) {
		leading = ilog2(o);
		uint64_t moves = 0;
		get_attacks(leading, moves);
		if (moves & pos) {
			mask |= 1ULL << leading;
		}
		o -= ipow2[leading];
	}
}

void Board::get_attacks(const uint8_t& position, uint64_t& mask) {
	uint64_t pos = 1ULL << position;
	if (pos & board[kings] || pos & board[black + kings]) {
		mask |= king_table[position];
	}
	else {
		get_moves(position, mask);
	}
}

void Board::attacked_squares(const color_t color, uint64_t& mask) {
	uint64_t a;
	color == white ? get_white(a) : get_black(a);
	uint64_t selector = 0;
	int leading;
	while (a) {
		leading = ilog2(a);
		uint64_t moves = 0;
		get_attacks(leading, moves);
		mask |= moves;
		a -= 1ULL << leading;
	}
}

bool Board::check(const color_t& color) {
	uint8_t king_pos = ilog2(board[kings + (color == white ? 0 : black)]);
	uint64_t attackers = 0;
	get_attackers(king_pos, attackers);
	return attackers;
}

bool Board::checkmate(const color_t& color) {
	if (check(color)) {
		uint64_t pieces = 0;
		color == white ? get_white(pieces) : get_black(pieces);
		int p_leading;
		while (pieces) {
			p_leading = ilog2(pieces);
			uint64_t moves = 0;
			get_moves(p_leading, moves);
			int m_leading;
			while (moves) {
				m_leading = ilog2(moves);
				Board* next = new Board(this);
				next->move(1ULL << p_leading, 1ULL << m_leading);
				bool check = next->check(color);
				delete next;
				if (!check) {
					return false;
				}
				moves -= 1ULL << m_leading;
			}
			pieces -= 1ULL << p_leading;
		}
		return true;
	}
	return false;
}

void Board::move(const uint64_t& start, const uint64_t& dest) {
	uint64_t w = 0;
	uint64_t b = 0;
	get_white(w);
	get_black(b);

	for (int i = 11; i >= 0; i--) {
		uint64_t& sub = board[i];
		if (sub & start) {
			sub ^= start;
			if (i == pawns) {
				if (dest & 18374686479671623680) {
					board[queens] |= dest;
					continue;
				}
				else if (start & 1095216660480 && !(dest & b) && !(start << 8 & dest)) {
					board[black + pawns] &= ~(dest >> 8);
				}
			}
			else if (i == black + pawns) {
				if (dest & 255) {
					board[black + queens] |= dest;
					continue;
				}
				else if (start & 4278190080 && !(dest & w) && !(start >> 8 & dest)) {
					board[pawns] &= ~(dest << 8);
				}
			} else if (i == rooks) {
				if (start & 0b10000000) {
					wlrmove = true;
				} else {
					wrrmove = true;
				}
			}
			else if (i == black + rooks) {
				if (start & 9223372036854775808) {
					blrmove = true;
				} else {
					brrmove = true;
				}
			}
			else if (i == kings) {
				wkmove = true;
				if (dest & board[rooks]) {
					if (dest & 0b10000000) {
						board[kings] |= 0b00100000;
						board[rooks] &= ~dest;
						board[rooks] |= 0b00010000;
					}
					else {
						board[kings] |= 0b00000010;
						board[rooks] &= ~dest;
						board[rooks] |= 0b00000100;
					}
					wcasle = true;
					return;
				}
			}
			else if (i == black + kings) {
				bkmove = true;
				if (dest & board[black + rooks]) {
					if (dest & 9223372036854775808) {
						board[black + kings] |= 2305843009213693952;
						board[black + rooks] &= ~dest;
						board[black + rooks] |= 1152921504606846976;
					}
					else {
						board[black + kings] |= 144115188075855872;
						board[black + rooks] &= ~dest;
						board[black + rooks] |= 288230376151711744;
					}
					bcasle = true;
					return;
				}
			}

			sub |= dest;
		}
		else {
			sub &= ~dest;
		}
	}
}

double Board::evaluate(color_t color, bool debug) {
	int min;
	int max;
	int value = 0;
	Board* d_board = new Board();
	uint64_t center_pawns = 103481868288;
	uint64_t vision = 0;
	bool castled;
	bool cc;
	attacked_squares(color, vision);

	uint64_t d_pieces = 0;
	uint64_t pieces = 0;
	if (color == white) {
		get_white(pieces);
		d_board->get_white(d_pieces);
		center_pawns &= board[pawns];
		min = 0;
		max = 6;
		castled = wcasle;
		cc = !wkmove && (!wrrmove || !wlrmove);
	}
	else {
		get_black(pieces);
		d_board->get_black(d_pieces);
		center_pawns &= board[black + pawns];
		min = 6;
		max = 12;
		castled = bcasle;
		cc = !bkmove && (!brrmove || !blrmove);
	}

	double development = 0;
	for (int i = min; i < max; i++) {
		uint64_t sub = board[i];
		uint8_t val = value_table[i];
		uint16_t d = 0;
		uint64_t developed = (d_board->board[i] ^ sub) & ~d_board->board[i];
		d += popcount(developed & 35604928818740736) * 0.75;
		d += popcount(developed & 66229406269440);
		development += d / abs(6 * (val - 3.25));
		value += val * popcount(sub);
	}
	delete d_board;

	if (debug)
		printf("Value: %d, Development: %f, Center Pawns: %f, Vision: %f, Castled: %d\n", value, development, popcount(center_pawns) * 0.5, popcount(vision) * 0.05, (cc ? 0 : (castled ? 1 : -2)));

	return value + development + popcount(center_pawns) * 0.5 + popcount(vision) * 0.05 + (cc ? 0 : (castled ? 1 : -2));
}

pair<uint8_t, uint8_t> Board::get_best(color_t color) {
	unordered_map<pair<uint8_t, uint8_t>*, future<double>> map;
	uint64_t pieces = 0;
	color == white ? get_white(pieces) : get_black(pieces);
	double alpha = numeric_limits<double>::min();
	double beta = numeric_limits<double>::max();
	int p_leading;
	while (pieces) {
		p_leading = ilog2(pieces);
		uint64_t moves = 0;
		get_moves(p_leading, moves);
		int m_leading;
		while (moves) {
			m_leading = ilog2(moves);
			Board* next = new Board(this);
			next->move(1ULL << p_leading, 1ULL << m_leading);
			if (!next->check(color)) {
				map.emplace(new pair<uint8_t, uint8_t>(p_leading, m_leading) , async(reval, next, color, color == white ? black : white, 0, &alpha, &beta));
			}
			else {
				delete next;
			}
			moves -= 1ULL << m_leading;
		}
		pieces -= 1ULL << p_leading;
	}
	double best = numeric_limits<double>::min();
	pair<uint8_t, uint8_t>* best_move = nullptr;

	for (pair<pair<uint8_t, uint8_t>* const, future<double>> &p : map) {
		double val = p.second.get();
		int sx, sy, ex, ey;
		sx = 8 - p.first->first % 8;
		sy = 1 + p.first->first / 8;
		ex = 8 - p.first->second % 8;
		ey = 1 + p.first->second / 8;

		printf("(%d, %d) -> (%d, %d): %f\n", sx, sy, ex, ey, val);

		if (!best_move || val >= best) {
			best = val;
			best_move = p.first;
		}
		else {
			delete p.first;
		}
	}
	cout << "------------" << endl;
	int sx, sy, ex, ey;
	sx = 8 - best_move->first % 8;
	sy = 1 + best_move->first / 8;
	ex = 8 - best_move->second % 8;
	ey = 1 + best_move->second / 8;
	printf("(%d, %d) -> (%d, %d): %f\n", sx, sy, ex, ey, best);
	cout << "******************" << endl;
	pair<uint8_t, uint8_t> ret = *best_move;
	delete best_move;
	return ret;
}

double reval(Board* board, color_t og_color, color_t curr_color, int depth, double* alpha, double* beta) {
	color_t op_color = curr_color == white ? black : white;
	if (board->checkmate(og_color)) {
		return numeric_limits<double>::min();
	}
	if (board->checkmate(op_color)) {
		return numeric_limits<double>::max();
	}
	if (depth >= EVAL_DEPTH) {
		return board->evaluate(og_color) / board->evaluate(og_color == white? black: white);
	}
	uint64_t pieces = 0;
	bool is_color = curr_color == og_color;
	double value = is_color ? numeric_limits<double>::min() : numeric_limits<double>::max();
	curr_color == white ? board->get_white(pieces) : board->get_black(pieces);
	int p_leading;
	while (pieces) {
		p_leading = ilog2(pieces);
		uint64_t moves = 0;
		board->get_moves(p_leading, moves);
		int m_leading;
		while (moves) {
			m_leading = ilog2(moves);
			Board* next = new Board(board);
			next->move(1ULL << p_leading, 1ULL << m_leading);
			if (!next->check(curr_color)) {
				double val = reval(next, og_color, op_color, depth + 1, alpha, beta);
				delete next;
				if (is_color) {
					value = max(val, value);
					if (value > *beta) {
						return value;
					}
					*alpha = max(*alpha, value);
				}
				else {
					value = min(val, value);
					if (value < *alpha) {
						return value;
					}
					*beta = min(*beta, value);
				}
			}
			else {
				delete next;
			}
			moves -= 1ULL << m_leading;
		}
		pieces -= 1ULL << p_leading;
	}
	return value;
}
