#include "util.h"

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

Texture* load_texture(string path) {
	Texture* texture = new Texture();
	texture->loadFromFile("res\\" + path);
	return texture;
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
			mask |= ((uint64_t)val) << (7 - j) + i * 8;
		}
	}
	delete ranks;
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
	delete ranks;
}
