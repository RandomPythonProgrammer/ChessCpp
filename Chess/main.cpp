#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;
using namespace sf;


typedef uint64_t* Board;

enum piece_t {pawns, bishops, knights, rooks, queens, kings};

enum color_t {white=0, black=6};

Board create_board() {
	uint64_t pawns = 0b11111111 << 8;

	//testing
	pawns = 0;

	uint64_t bishops = 0b00100100;
	uint64_t knights = 0b01000010;
	uint64_t rooks = 0b10000001;
	uint64_t queens = 0b00010000;
	uint64_t kings = 0b00001000;


	return new uint64_t[]{
		pawns, bishops, knights, rooks, queens, kings,
		pawns << 40, bishops << 56, knights << 56, rooks << 56, queens << 56, kings << 56
	};
}

Texture* load_texture(string path) {
	Texture* texture = new Texture();
	texture->loadFromFile("res\\" + path);
	return texture;
}

void get_white(const Board &board, uint64_t &mask) {
	for (int i = 0; i < 6; i++) {
		mask |= board[i];
	}
}

void get_black(const Board &board, uint64_t &mask) {
	for (int i = 6; i < 12; i++) {
		mask |= board[i];
	}
}

void get_all(const Board &board, uint64_t &mask) {
	get_white(board, mask);
	get_black(board, mask);
}

void rotate_right(uint64_t& mask) {
	uint8_t copy = mask;
	uint8_t* ranks = new uint8_t[8];
	for (int i = 0; i < 8; i++) {
		ranks[i] = mask >> 8 * i;
	}
	mask = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			bool val = ranks[j] & (1 << i);
			mask |= ((uint64_t) val) << (7-j) + i*8;
		}
	}
}

void rotate_left(uint64_t& mask) {
	uint8_t copy = mask;
	uint8_t* ranks = new uint8_t[8];
	for (int i = 0; i < 8; i++) {
		ranks[i] = mask >> 8 * i;
	}
	mask = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			bool val = ranks[j] & (1 << 7 - i);
			mask |= ((uint64_t)val) << j + i * 8;
		}
	}
}

void t_r(uint64_t& pos) {
	pos <<= 7;
}

void t_l(uint64_t& pos) {
	pos <<= 9;
}

void b_r(uint64_t& pos) {
	pos >>= 9;
}

void b_l(uint64_t& pos) {
	pos >>= 7;
}

void bishop_attack(const Board& board, const uint64_t& position, uint64_t& mask) {
	//Need to fix the bug of the bishop exiting the board (Resolved)

	/*
	Strategy:
		Store the position in an x and y integer
		Check the integers for bounds

		(Should be Resolved Now)

	Todo: 
		Inline the functions as they serve no real purpose
	*/
	
	mask = 0;
	uint64_t pos = position;
	bool hit = false;
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(board, w);
	get_black(board, b);
	uint64_t a = w | b;
	uint8_t o = position & w ? b : w;

	uint8_t ipos = log2(position);
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
		t_r(pos);
		if (o & pos) {
			if (hit) {
				break;
			} else {
				hit = true;
			}
		} else if (a & pos) {
			break;
		}
		mask |= pos;
	}

	pos = position;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_++;
		x_++;
		if (y > 7 || x > 7) {
			break;
		}
		t_l (pos);
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

	pos = position;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_--;
		x_--;
		if (y < 0 || x < 0) {
			break;
		}
		b_r(pos);
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

	pos = position;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_--;
		x_++;
		if (y < 0 || x > 7) {
			break;
		}
		b_l(pos);
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

void rank_attack(const Board& board, const uint64_t& position, uint64_t& mask) {
	uint8_t pos = log2(position);
	uint8_t line = 0b11111111;

	uint64_t w = 0;
	uint64_t b = 0;

	uint8_t y = pos >> 3 << 3;
	uint8_t x = pos % 8;
	uint8_t pos_line = position >> y;

	get_white(board, w);
	get_black(board, b);
	uint64_t a = w | b;
	uint8_t o = (position & w ? b : w) >> y;

	uint8_t rank = line ^ a >> y;
	uint8_t right = rank & (uint8_t) (line >> 8-x);
	uint8_t left = rank & (uint8_t) (line << x+1);
	uint8_t rfirst = countl_one((uint8_t) (right << 8-x));
	uint8_t lfirst = countr_one((uint8_t) (left >> x+1));

	left &= line >> (6 - x - lfirst); //There is a bug on this line
	left |= (pos_line << lfirst + 1) & o;
	right &= line << (x - rfirst);
	right |= (pos_line >> rfirst + 1) & o;
	mask = (left | right);
	mask <<= y;
}

void file_attack(const Board& board, const uint64_t& position, uint64_t& mask) {
	uint64_t rpos = position;
	rotate_right(rpos);
	uint8_t pos = log(rpos) / log(2);
	uint8_t line = 0b11111111;

	uint64_t w = 0;
	uint64_t b = 0;

	uint8_t y = pos >> 3 << 3;
	uint8_t x = pos % 8;
	uint8_t pos_line = rpos >> y;

	get_white(board, w);
	get_black(board, b);
	rotate_right(w);
	rotate_right(b);
	uint64_t a = w | b;
	uint8_t o = (rpos & w ? b : w) >> y;

	uint8_t rank = line ^ a >> y;
	uint8_t right = rank & (uint8_t)(line >> 8 - x);
	uint8_t left = rank & (uint8_t)(line << x + 1);
	uint8_t rfirst = countl_one((uint8_t)(right << 8 - x));
	uint8_t lfirst = countr_one((uint8_t)(left >> x + 1));

	left &= line >> (6 - x - lfirst);
	left |= (pos_line << lfirst + 1) & o;
	right &= line << (x - rfirst);
	right |= (pos_line >> rfirst + 1) & o;
	mask = (left | right);
	mask <<= y;
	rotate_left(mask);
}

void rook_attack(const Board& board, const uint64_t& position, uint64_t& mask) {
	uint64_t r = 0;
	uint64_t f = 0;
	rank_attack(board, position, r);
	file_attack(board, position, f);
	mask = r | f;
}

void get_moves(const Board& board, const uint64_t& position, uint64_t& mask) {
	for (int i = 0; i < 6; i++) {
		uint64_t wboard = board[i];
		uint64_t bboard = board[i + black];
		uint64_t r = 0;
		uint64_t b = 0;
		if (wboard & position || bboard & position) {
			switch (i) {
			case pawns:
				cout << "pawn move" << endl;
				break;
			case bishops:
				cout << "bishop move" << endl;
				bishop_attack(board, position, mask);
				return;
			case knights:
				cout << "knight move" << endl;
				break;
			case rooks:
				cout << "rook move" << endl;
				rook_attack(board, position, mask);
				return;
			case queens:
				cout << "queen move" << endl;
				rook_attack(board, position, r);
				bishop_attack(board, position, b);
				mask = r | b;
				return;
			case kings:
				cout << "king move" << endl;
				break;
			default:
				cout << "invalid piece" << endl;
				break;
			}
		}
	}
}

void move(const Board& board, const uint64_t& start, const uint64_t& dest) {
	for (int i = 0; i < 12; i++) {
		uint64_t& sub = board[i];
		if (sub & start) {
			sub ^= start;
			sub |= dest;
		} else {
			sub &= ~dest;
		}
	}
}

int main() {
	color_t color = white;
	int piece_size = 45;
	int board_size = piece_size * 8;
	RenderWindow window(VideoMode(board_size, board_size), "chess");
	Texture* textures[]{
		load_texture("wp.png"), load_texture("wb.png"), load_texture("wn.png"), load_texture("wr.png"), load_texture("wq.png"), load_texture("wk.png"),
		load_texture("bp.png"), load_texture("bb.png"), load_texture("bn.png"), load_texture("br.png"), load_texture("bq.png"), load_texture("bk.png")
	};
	Board board = create_board();

	RectangleShape square(Vector2f(piece_size, piece_size));
	square.setFillColor(Color::Green);
	CircleShape selection_circle;
	selection_circle.setRadius(piece_size / 2);
	selection_circle.setFillColor(Color::Transparent);
	selection_circle.setOutlineColor(Color::Red);
	selection_circle.setOutlineThickness(2);

	bool has_selection = false;
	uint64_t selected = 0;


	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}

			if (event.type == Event::MouseButtonPressed) {
				Vector2i position = Mouse::getPosition(window);
				int x = (board_size - position.x) / piece_size;
				int y = (board_size - position.y) / piece_size;
				int pos = x + y * 8;
				uint64_t click_pos = 1ULL << pos;
				uint64_t selection = 1ULL << selected;
				uint64_t mask = 0ULL;
				color == white ? get_white(board, mask) : get_black(board, mask);

				if (has_selection) {
					uint64_t moves = 0;
					get_moves(board, selection, moves);
					if (moves & click_pos) {
						move(board, selection, click_pos); //selection
						selected = pos;
						break;
					}
				}
				
				if (mask & click_pos) {
					selected = pos;
					has_selection = mask & selection;
				}
			}
		}

		window.clear(Color::White);

		for (int i = 0; i < 64; i++) {
			int row = i / 8;
			int column = i % 8;
			if (row % 2 == column % 2) {
				square.setPosition(column * piece_size, board_size - (row+1) * piece_size);
				window.draw(square);
			}
		}

		for (int i = 0; i < 12; i++) {
			Texture texture = *textures[i];
			uint64_t sub = board[i];
			Sprite sprite(texture);
			for (int j = 0; j < 64; j++) {
				int row = j / 8;
				int column = j % 8;
				if (sub & 1ULL << j) {
					sprite.setPosition((7-column) * piece_size, board_size - (row+1) * piece_size);
					window.draw(sprite);
				}
			}
		}

		if (has_selection) {
			int row = selected / 8;
			int column = selected % 8;
			selection_circle.setPosition((7-column) * 45, board_size - (row+1) * 45);
			window.draw(selection_circle);
		}

		window.display();
	}
	return 0;
}

