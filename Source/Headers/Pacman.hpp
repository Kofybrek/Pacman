#pragma once

class Pacman
{
	//This is used for the death animation.
	bool animation_over;
	//Am I dead?
	bool dead;

	unsigned char direction;

	//More timers!
	unsigned short animation_timer;
	unsigned short energizer_timer;

	//Current location of this creature, commonly known as Pacman.
	Position position;
public:
	Pacman();

	bool get_animation_over();
	bool get_dead();

	unsigned char get_direction();

	unsigned short get_energizer_timer();

	void draw(bool i_victory, sf::RenderWindow& i_window);
	void reset();
	void set_animation_timer(unsigned short i_animation_timer);
	void set_dead(bool i_dead);
	void set_position(short i_x, short i_y);
	void update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map);

	Position get_position();
};