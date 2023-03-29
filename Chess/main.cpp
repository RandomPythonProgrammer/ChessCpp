#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;
using namespace sf;


typedef uint64_t* Board;

enum piece_t {pawns, bishops, knights, rooks, queens, kings};

enum color_t {white=0, black=6};

uint64_t knight_table[] = {
	132096,
	329728,
	659712,
	1319424,
	2638848,
	5277696,
	10489856,
	4202496,
	33816580,
	84410376,
	168886289,
	337772578,
	675545156,
	1351090312,
	2685403152,
	1075839008,
	8657044482,
	21609056261,
	43234889994,
	86469779988,
	172939559976,
	345879119952,
	687463207072,
	275414786112,
	2216203387392,
	5531918402816,
	11068131838464,
	22136263676928,
	44272527353856,
	88545054707712,
	175990581010432,
	70506185244672,
	567348067172352,
	1416171111120896,
	2833441750646784,
	5666883501293568,
	11333767002587136,
	22667534005174272,
	45053588738670592,
	18049583422636032,
	145241105196122112,
	362539804446949376,
	725361088165576704,
	1450722176331153408,
	2901444352662306816,
	5802888705324613632,
	11533718717099671552,
	4620693356194824192,
	288234782788157440,
	576469569871282176,
	1224997833292120064,
	2449995666584240128,
	4899991333168480256,
	9799982666336960512,
	1152939783987658752,
	2305878468463689728,
	1128098930098176,
	2257297371824128,
	4796069720358912,
	9592139440717824,
	19184278881435648,
	38368557762871296,
	4679521487814656,
	9077567998918656
};

uint64_t king_table[] = {
	770,
	1797,
	3594,
	7188,
	14376,
	28752,
	57504,
	49216,
	197123,
	460039,
	920078,
	1840156,
	3680312,
	7360624,
	14721248,
	12599488,
	50463488,
	117769984,
	235539968,
	471079936,
	942159872,
	1884319744,
	3768639488,
	3225468928,
	12918652928,
	30149115904,
	60298231808,
	120596463616,
	241192927232,
	482385854464,
	964771708928,
	825720045568,
	3307175149568,
	7718173671424,
	15436347342848,
	30872694685696,
	61745389371392,
	123490778742784,
	246981557485568,
	211384331665408,
	846636838289408,
	1975852459884544,
	3951704919769088,
	7903409839538176,
	15806819679076352,
	31613639358152704,
	63227278716305408,
	54114388906344448,
	216739030602088448,
	505818229730443264,
	1011636459460886528,
	2023272918921773056,
	4046545837843546112,
	8093091675687092224,
	16186183351374184448,
	13853283560024178688,
	144959613005987840,
	362258295026614272,
	724516590053228544,
	1449033180106457088,
	2898066360212914176,
	5796132720425828352,
	11592265440851656704,
	4665729213955833856
};

//White pawn moving forwards
uint64_t wpf_table[] = {
	256,
	512,
	1024,
	2048,
	4096,
	8192,
	16384,
	32768,
	16842752,
	33685504,
	67371008,
	134742016,
	269484032,
	538968064,
	1077936128,
	2155872256,
	16777216,
	33554432,
	67108864,
	134217728,
	268435456,
	536870912,
	1073741824,
	2147483648,
	4294967296,
	8589934592,
	17179869184,
	34359738368,
	68719476736,
	137438953472,
	274877906944,
	549755813888,
	1099511627776,
	2199023255552,
	4398046511104,
	8796093022208,
	17592186044416,
	35184372088832,
	70368744177664,
	140737488355328,
	281474976710656,
	562949953421312,
	1125899906842624,
	2251799813685248,
	4503599627370496,
	9007199254740992,
	18014398509481984,
	36028797018963968,
	72057594037927936,
	144115188075855872,
	288230376151711744,
	576460752303423488,
	1152921504606846976,
	2305843009213693952,
	4611686018427387904,
	9223372036854775808,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};
//White pawn captures
uint64_t wpc_table[] = {
	512,
	1280,
	2560,
	5120,
	10240,
	20480,
	40960,
	16384,
	131072,
	327680,
	655360,
	1310720,
	2621440,
	5242880,
	10485760,
	4194304,
	33554432,
	83886080,
	167772160,
	335544320,
	671088640,
	1342177280,
	2684354560,
	1073741824,
	8589934592,
	21474836480,
	42949672960,
	85899345920,
	171798691840,
	343597383680,
	687194767360,
	274877906944,
	2199023255552,
	5497558138880,
	10995116277760,
	21990232555520,
	43980465111040,
	87960930222080,
	175921860444160,
	70368744177664,
	562949953421312,
	1407374883553280,
	2814749767106560,
	5629499534213120,
	11258999068426240,
	22517998136852480,
	45035996273704960,
	18014398509481984,
	144115188075855872,
	360287970189639680,
	720575940379279360,
	1441151880758558720,
	2882303761517117440,
	5764607523034234880,
	11529215046068469760,
	4611686018427387904,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

//Black pawn moving forwards
uint64_t bpf_table[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	2,
	4,
	8,
	16,
	32,
	64,
	128,
	256,
	512,
	1024,
	2048,
	4096,
	8192,
	16384,
	32768,
	65536,
	131072,
	262144,
	524288,
	1048576,
	2097152,
	4194304,
	8388608,
	16777216,
	33554432,
	67108864,
	134217728,
	268435456,
	536870912,
	1073741824,
	2147483648,
	4294967296,
	8589934592,
	17179869184,
	34359738368,
	68719476736,
	137438953472,
	274877906944,
	549755813888,
	1103806595072,
	2207613190144,
	4415226380288,
	8830452760576,
	17660905521152,
	35321811042304,
	70643622084608,
	141287244169216,
	281474976710656,
	562949953421312,
	1125899906842624,
	2251799813685248,
	4503599627370496,
	9007199254740992,
	18014398509481984,
	36028797018963968
};
//Black pawn captures
uint64_t bpc_table[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	2,
	5,
	10,
	20,
	40,
	80,
	160,
	64,
	512,
	1280,
	2560,
	5120,
	10240,
	20480,
	40960,
	16384,
	131072,
	327680,
	655360,
	1310720,
	2621440,
	5242880,
	10485760,
	4194304,
	33554432,
	83886080,
	167772160,
	335544320,
	671088640,
	1342177280,
	2684354560,
	1073741824,
	8589934592,
	21474836480,
	42949672960,
	85899345920,
	171798691840,
	343597383680,
	687194767360,
	274877906944,
	2199023255552,
	5497558138880,
	10995116277760,
	21990232555520,
	43980465111040,
	87960930222080,
	175921860444160,
	70368744177664,
	562949953421312,
	1407374883553280,
	2814749767106560,
	5629499534213120,
	11258999068426240,
	22517998136852480,
	45035996273704960,
	18014398509481984
};

uint8_t ilog2(uint64_t i) {
	if (i >= 9223372036854775807) return 63;
	if (i >= 4611686018427387904) return 62;
	if (i >= 2305843009213693952) return 61;
	if (i >= 1152921504606846976) return 60;
	if (i >= 576460752303423488) return 59;
	if (i >= 288230376151711744) return 58;
	if (i >= 144115188075855872) return 57;
	if (i >= 72057594037927936) return 56;
	if (i >= 36028797018963968) return 55;
	if (i >= 18014398509481984) return 54;
	if (i >= 9007199254740992) return 53;
	if (i >= 4503599627370496) return 52;
	if (i >= 2251799813685248) return 51;
	if (i >= 1125899906842624) return 50;
	if (i >= 562949953421312) return 49;
	if (i >= 281474976710656) return 48;
	if (i >= 140737488355328) return 47;
	if (i >= 70368744177664) return 46;
	if (i >= 35184372088832) return 45;
	if (i >= 17592186044416) return 44;
	if (i >= 8796093022208) return 43;
	if (i >= 4398046511104) return 42;
	if (i >= 2199023255552) return 41;
	if (i >= 1099511627776) return 40;
	if (i >= 549755813888) return 39;
	if (i >= 274877906944) return 38;
	if (i >= 137438953472) return 37;
	if (i >= 68719476736) return 36;
	if (i >= 34359738368) return 35;
	if (i >= 17179869184) return 34;
	if (i >= 8589934592) return 33;
	if (i >= 4294967296) return 32;
	if (i >= 2147483648) return 31;
	if (i >= 1073741824) return 30;
	if (i >= 536870912) return 29;
	if (i >= 268435456) return 28;
	if (i >= 134217728) return 27;
	if (i >= 67108864) return 26;
	if (i >= 33554432) return 25;
	if (i >= 16777216) return 24;
	if (i >= 8388608) return 23;
	if (i >= 4194304) return 22;
	if (i >= 2097152) return 21;
	if (i >= 1048576) return 20;
	if (i >= 524288) return 19;
	if (i >= 262144) return 18;
	if (i >= 131072) return 17;
	if (i >= 65536) return 16;
	if (i >= 32768) return 15;
	if (i >= 16384) return 14;
	if (i >= 8192) return 13;
	if (i >= 4096) return 12;
	if (i >= 2048) return 11;
	if (i >= 1024) return 10;
	if (i >= 512) return 9;
	if (i >= 256) return 8;
	if (i >= 128) return 7;
	if (i >= 64) return 6;
	if (i >= 32) return 5;
	if (i >= 16) return 4;
	if (i >= 8) return 3;
	if (i >= 4) return 2;
	if (i >= 2) return 1;
	if (i >= 1) return 0;
}

Board create_board() {
	uint64_t pawns = 0b11111111ULL << 8;

	//testing
	//pawns = 0;

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

void knight_attack(const Board& board, const uint8_t& position, uint64_t& mask) {
	mask = 0;
	uint64_t attack = knight_table[position];
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(board, w);
	get_black(board, b);;
	uint64_t c = 1ULL << position & w ? w : b;

	mask |= (attack & ~c);
}

void king_attack(const Board& board, const uint8_t& position, uint64_t& mask) {
	mask = 0;
	uint64_t attack = king_table[position];
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(board, w);
	get_black(board, b);;
	uint64_t c = 1ULL << position & w ? w : b;

	mask |= (attack & ~c);
}

void pawn_attack(const Board& board, const uint8_t& position, uint64_t& mask) {
	//Major work in progress
	//Needs en passant and attack moves
	mask = 0;
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(board, w);
	get_black(board, b);;

	uint64_t attack = 0;
	uint64_t c;

	if (1ULL << position & w) {
		attack |= wpf_table[position] & ~(w | b);
		attack |= wpc_table[position] & b;
		c = w;
	} else {
		attack |= bpf_table[position] & ~(w | b);
		attack |= (bpc_table[position] & w);
		c = b;
	}

	mask |= attack;
}

void bishop_attack(const Board& board, const uint8_t& position, uint64_t& mask) {
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
	uint64_t _pos = 1ULL << position;
	uint64_t pos = _pos;
	bool hit = false;
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(board, w);
	get_black(board, b);
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

void rank_attack(const Board& board, const uint8_t& position, uint64_t& mask) {
	uint8_t line = 0b11111111;
	uint64_t pos = 1ULL << position;

	uint64_t w = 0;
	uint64_t b = 0;

	uint8_t y = position >> 3 << 3;
	uint8_t x = position % 8;
	uint8_t pos_line = pos >> y;

	get_white(board, w);
	get_black(board, b);
	uint64_t a = w | b;
	uint8_t o = (pos & w ? b : w) >> y;

	uint8_t rank = line ^ a >> y;
	uint8_t right = rank & (uint8_t) (line >> 8-x);
	uint8_t left = rank & (uint8_t) (line << x+1);
	uint8_t rfirst = countl_one((uint8_t) (right << 8-x));
	uint8_t lfirst = countr_one((uint8_t) (left >> x+1));

	left &= line >> (7 - x - lfirst);
	left |= (pos_line << lfirst + 1) & o;
	right &= line << (x - rfirst);
	right |= (pos_line >> rfirst + 1) & o;
	mask = (left | right);
	mask <<= y;
}

void file_attack(const Board& board, const uint8_t& position, uint64_t& mask) {
	uint64_t rpos = 1ULL << position;
	rotate_right(rpos);
	uint8_t pos = ilog2(rpos); // should figure out a way to optimize this one out
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

	left &= line >> (7 - x - lfirst);
	left |= (pos_line << lfirst + 1) & o;
	right &= line << (x - rfirst);
	right |= (pos_line >> rfirst + 1) & o;
	mask = (left | right);
	mask <<= y;
	rotate_left(mask);
}

void rook_attack(const Board& board, const uint8_t& position, uint64_t& mask) {
	uint64_t r = 0;
	uint64_t f = 0;
	rank_attack(board, position, r);
	file_attack(board, position, f);
	mask = r | f;
}

void get_moves(const Board& board, const uint8_t& position, uint64_t& mask) {
	uint64_t pos = 1ULL << position;
	for (int i = 0; i < 6; i++) {
		uint64_t wboard = board[i];
		uint64_t bboard = board[i + black];
		uint64_t r = 0;
		uint64_t b = 0;
		if (wboard & pos || bboard & pos) {
			switch (i) {
			case pawns:
				cout << "pawn move" << endl;
				pawn_attack(board, position, mask);
				break;
			case bishops:
				cout << "bishop move" << endl;
				bishop_attack(board, position, mask);
				return;
			case knights:
				cout << "knight move" << endl;
				knight_attack(board, position, mask);
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
				king_attack(board, position, mask);
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
					get_moves(board, selected , moves);
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

