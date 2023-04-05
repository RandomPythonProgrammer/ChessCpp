#include "Board.h"
#include <stdint.h>

using namespace std;

int main() {
	Board* board = new Board();
	board->move(1ULL << 11, 1ULL << 19);
	board->move(1ULL << 51, 1ULL << 35);
	double alpha = numeric_limits<double>::max();
	double beta = numeric_limits<double>::min();
	reval(board, black, white, 0, &alpha, &beta);
}