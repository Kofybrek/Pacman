#pragma once

class Ghost
{
	//It can be the scatter mode or the chase mode.
	bool movement_mode;
	//"Can I use the door, pwease?"
	bool use_door;

	//Current direction.
	unsigned char direction;
	//0 - I'm not frightened
	//1 - Okay, maybe I am
	//2 - AAAAAAAH!!! I'M GOING TO MY HOUSE!
	unsigned char frightened_mode;
	//To make the ghost move more slowly, we'll move it after a certain number of frames. So we need a timer.
	unsigned char frightened_speed_timer;
	//0 - Red
	//1 - Pink
	//2 - Blue (It's actually cyan, but the website said it's blue. And I didn't wanna argue.)
	//3 - Orange
	unsigned char id;

	unsigned short animation_timer;

	//The ghost will go here when escaping.
	Position home;
	//You can't stay in your house forever (sadly).
	Position home_exit;
	//Current position.
	Position position;
	//Current target.
	Position target;
public:
	Ghost(unsigned char i_id);

	bool pacman_collision(const Position& i_pacman_position);

	float get_target_distance(unsigned char i_direction);

	void draw(bool i_flash, sf::RenderWindow& i_window);
	void reset(const Position& i_home, const Position& i_home_exit);
	void set_position(short i_x, short i_y);
	void switch_mode();
	void update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, Ghost& i_ghost_0, Pacman& i_pacman);
	void update_target(unsigned char i_pacman_direction, const Position& i_ghost_0_position, const Position& i_pacman_position);

	Position get_position();
};