#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include <string>
#include "board.h"
#include "util.h"

using namespace std;
using namespace sf;

int main() {
	char c;
	cin >> c;
	color_t color = c == 'w'? white: black;
	int piece_size = 45;
	int board_size = piece_size * 8;
	RenderWindow window(VideoMode(board_size, board_size), "chess");
	Texture* textures[]{
		load_texture("wp.png"), load_texture("wb.png"), load_texture("wn.png"), load_texture("wr.png"), load_texture("wq.png"), load_texture("wk.png"),
		load_texture("bp.png"), load_texture("bb.png"), load_texture("bn.png"), load_texture("br.png"), load_texture("bq.png"), load_texture("bk.png")
	};
	Board* board = new Board();

	//bot goes first
	if (color == black) {
		board = board->get_best(color == white ? black : white).first;
		double w = board->evaluate(white, true);
		cout << "%%%%%%%%%%%%%%%%%%" << endl;
		double b = board->evaluate(black, true);
		printf("White value: %f, Black value: %f\n", w, b);
	}

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
				color == white ? board->get_white(mask) : board->get_black(mask);

				if (has_selection) {
					uint64_t moves = 0;
					board->get_moves(selected, moves);
					if (moves & click_pos) {
						Board* next = new Board(board);
						next->move(selection, click_pos); //selection
						if (!next->check(color)) {
							board = next;
							selected = pos;
							has_selection = false;
							//color = color == white ? black : white;
							board = board->get_best(color == white? black: white, Keyboard::isKeyPressed(Keyboard::LShift)).first;
							double w = board->evaluate(white, true);
							cout << "-------------------" << endl;
							double b = board->evaluate(black, true);
							printf("White value: %f, Black value: %f\n", w, b);
							if (board->checkmate(white)) {
								cout << "white checkmate" << endl;
							}
							if (board->checkmate(black)) {
								cout << "black checkmate" << endl;
							}
							cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
						} else {
							delete next;
						}
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
			uint64_t sub = board->board[i];
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

