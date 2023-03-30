#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

uint8_t ilog2(uint64_t i);

Texture* load_texture(string path);

void rotate_right(uint64_t& mask);

void rotate_left(uint64_t& mask);