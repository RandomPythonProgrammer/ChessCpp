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

void get_moves(const Board& board, uint64_t& mask) {
	
}

void rank_attack(const Board& board, uint64_t& position, uint64_t& mask) {
	uint8_t pos = log(position) / log(2);
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

	left &= line >> (6 - x - lfirst);
	left |= (pos_line << lfirst + 1) & o;
	right &= line << (x - rfirst);
	right |= (pos_line >> rfirst + 1) & o;
	mask = left | right;
}

int main() {
	Board board = create_board();
	board[bishops] = 0;
	board[knights] = 0;
	board[knights + black] = 0b10001000;
	board[queens] = 0;
	board[kings] = 0;
	board[rooks] = 0b00100000;
	uint64_t pos = 1ULL << 5;
	uint64_t attacks = 0;
	rank_attack(board, pos, attacks);
}


int main1() {
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
				if (!has_selection) {
					Vector2i position = Mouse::getPosition(window);
					int x = position.x / piece_size;
					int y = (board_size - position.y) / piece_size;
					selected = x + y * 8;
					uint64_t selection = 1ULL << selected;
					uint64_t mask = 0ULL;
					color == white ? get_white(board, mask) : get_black(board, mask);
					has_selection = mask & selection;
				} else {
					
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
					sprite.setPosition(column * piece_size, board_size - (row+1) * piece_size);
					window.draw(sprite);
				}
			}
		}

		if (has_selection) {
			int row = selected / 8;
			int column = selected % 8;
			selection_circle.setPosition(column * 45, board_size - (row+1) * 45);
			window.draw(selection_circle);
		}

		window.display();
	}
	return 0;
}
