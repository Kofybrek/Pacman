#include <array>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "Headers/Global.hpp"
#include "Headers/Pacman.hpp"
#include "Headers/Ghost.hpp"
#include "Headers/MapCollision.hpp"

Ghost::Ghost(unsigned char i_id) :
	id(i_id)
{
	//I keep writing "gohst" instead of "gohst" (THERE! I did it again!).
	//So in this file I'll write only "gohst".
	//Enjoy!
}

bool Ghost::pacman_collision(const Position& i_pacman_position)
{
	//I used the ADVANCED collision checking algorithm.
	//Only experts like me can understand this.
	if (position.x > i_pacman_position.x - CELL_SIZE && position.x < CELL_SIZE + i_pacman_position.x)
	{
		if (position.y > i_pacman_position.y - CELL_SIZE && position.y < CELL_SIZE + i_pacman_position.y)
		{
			return 1;
		}
	}

	return 0;
}

float Ghost::get_target_distance(unsigned char i_direction)
{
	short x = position.x;
	short y = position.y;

	//We'll imaginarily move the gohst in a given direction and calculate the distance to the target.
	switch (i_direction)
	{
		case 0:
		{
			x += GHOST_SPEED;

			break;
		}
		case 1:
		{
			y -= GHOST_SPEED;

			break;
		}
		case 2:
		{
			x -= GHOST_SPEED;

			break;
		}
		case 3:
		{
			y += GHOST_SPEED;
		}
	}

	//I used the Pythagoras' theorem.
	//Because I know math.
	//Are you impressed?
	return static_cast<float>(sqrt(pow(x - target.x, 2) + pow(y - target.y, 2)));
}

void Ghost::draw(bool i_flash, sf::RenderWindow& i_window)
{
	//Current frame of the animation.
	unsigned char body_frame = static_cast<unsigned char>(floor(animation_timer / static_cast<float>(GHOST_ANIMATION_SPEED)));

	sf::Sprite body;
	sf::Sprite face;

	sf::Texture texture;
	texture.loadFromFile("Resources/Images/Ghost" + std::to_string(CELL_SIZE) + ".png");

	body.setTexture(texture);
	body.setPosition(position.x, position.y);
	//Animation is basically a quick display of similar images.
	//So that's what we're doing here.
	body.setTextureRect(sf::IntRect(CELL_SIZE * body_frame, 0, CELL_SIZE, CELL_SIZE));

	face.setTexture(texture);
	face.setPosition(position.x, position.y);

	//The "I'm not frightened" look.
	if (0 == frightened_mode)
	{
		switch (id)
		{
			case 0:
			{
				//Red color
				body.setColor(sf::Color(255, 0, 0));

				break;
			}
			case 1:
			{
				//Pink color
				body.setColor(sf::Color(255, 182, 255));

				break;
			}
			case 2:
			{
				//Cyan color (I still don't understand why they called it blue)
				body.setColor(sf::Color(0, 255, 255));

				break;
			}
			case 3:
			{
				//Orange color
				body.setColor(sf::Color(255, 182, 85));
			}
		}

		face.setTextureRect(sf::IntRect(CELL_SIZE * direction, CELL_SIZE, CELL_SIZE, CELL_SIZE));

		i_window.draw(body);
	}
	//The "Maybe I am frightened" look.
	else if (1 == frightened_mode)
	{
		body.setColor(sf::Color(36, 36, 255));
		//The face remains the same regardless of where the gohst is going right now.
		face.setTextureRect(sf::IntRect(4 * CELL_SIZE, CELL_SIZE, CELL_SIZE, CELL_SIZE));

		if (1 == i_flash && 0 == body_frame % 2)
		{
			body.setColor(sf::Color(255, 255, 255));
			face.setColor(sf::Color(255, 0, 0));
		}
		else
		{
			body.setColor(sf::Color(36, 36, 255));
			face.setColor(sf::Color(255, 255, 255));
		}

		i_window.draw(body);
	}
	//The "AAAAH!!!! I'M OUTTA HERE!!!!" look.
	else
	{
		//We only draw the face because Pacman stole the body.
		face.setTextureRect(sf::IntRect(CELL_SIZE * direction, 2 * CELL_SIZE, CELL_SIZE, CELL_SIZE));
	}

	i_window.draw(face);

	//--------------------------------------<        This is to prevent overflowing.         >-
	animation_timer = (1 + animation_timer) % (GHOST_ANIMATION_FRAMES * GHOST_ANIMATION_SPEED);
}

void Ghost::reset(const Position& i_home, const Position& i_home_exit)
{
	movement_mode = 0;
	//Everyone can use the door except the red gohst.
	//Because I hate the red gohst.
	use_door = 0 < id;

	direction = 0;
	frightened_mode = 0;
	frightened_speed_timer = 0;

	animation_timer = 0;

	home = i_home;
	home_exit = i_home_exit;
	target = i_home_exit;
}

void Ghost::set_position(short i_x, short i_y)
{
	position = {i_x, i_y};
}

void Ghost::switch_mode()
{
	movement_mode = 1 - movement_mode;
}

void Ghost::update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, Ghost& i_ghost_0, Pacman& i_pacman)
{
	//Can the gohst move?
	bool move = 0;

	//If this is greater than 1, that means that the gohst has reached the intersection.
	//We don't consider the way back as an available way.
	unsigned char available_ways = 0;
	unsigned char speed = GHOST_SPEED;

	std::array<bool, 4> walls{};

	//Here the gohst starts and stops being frightened.
	if (0 == frightened_mode && i_pacman.get_energizer_timer() == ENERGIZER_DURATION / pow(2, i_level))
	{
		frightened_speed_timer = GHOST_FRIGHTENED_SPEED;

		frightened_mode = 1;
	}
	else if (0 == i_pacman.get_energizer_timer() && 1 == frightened_mode)
	{
		frightened_mode = 0;
	}

	//I used the modulo operator in case the gohst goes outside the grid.
	if (2 == frightened_mode && 0 == position.x % GHOST_ESCAPE_SPEED && 0 == position.y % GHOST_ESCAPE_SPEED)
	{
		speed = GHOST_ESCAPE_SPEED;
	}

	update_target(i_pacman.get_direction(), i_ghost_0.get_position(), i_pacman.get_position());

	//This is so clean! I could spend hours staring at it.
	walls[0] = map_collision(0, use_door, speed + position.x, position.y, i_map);
	walls[1] = map_collision(0, use_door, position.x, position.y - speed, i_map);
	walls[2] = map_collision(0, use_door, position.x - speed, position.y, i_map);
	walls[3] = map_collision(0, use_door, position.x, speed + position.y, i_map);

	if (1 != frightened_mode)
	{
		//I used 4 because using a number between 0 and 3 will make the gohst move in a direction it can't move.
		unsigned char optimal_direction = 4;

		//The gohst can move.
		move = 1;

		for (unsigned char a = 0; a < 4; a++)
		{
			//Gohsts can't turn back! (Unless they really have to)
			if (a == (2 + direction) % 4)
			{
				continue;
			}
			else if (0 == walls[a])
			{
				if (4 == optimal_direction)
				{
					optimal_direction = a;
				}

				available_ways++;

				if (get_target_distance(a) < get_target_distance(optimal_direction))
				{
					//The optimal direction is the direction that's closest to the target.
					optimal_direction = a;
				}
			}
		}

		if (1 < available_ways)
		{
			//When the gohst is at the intersection, it chooses the optimal direction.
			direction = optimal_direction;
		}
		else
		{
			if (4 == optimal_direction)
			{
				//"Unless they have to" part.
				direction = (2 + direction) % 4;
			}
			else
			{
				direction = optimal_direction;
			}
		}
	}
	else
	{
		//I used rand() because I figured that we're only using randomness here, and there's no need to use a whole library for it.
		unsigned char random_direction = rand() % 4;

		if (0 == frightened_speed_timer)
		{
			//The gohst can move after a certain number of frames.
			move = 1;

			frightened_speed_timer = GHOST_FRIGHTENED_SPEED;

			for (unsigned char a = 0; a < 4; a++)
			{
				//They can't turn back even if they're frightened.
				if (a == (2 + direction) % 4)
				{
					continue;
				}
				else if (0 == walls[a])
				{
					available_ways++;
				}
			}

			if (0 < available_ways)
			{
				while (1 == walls[random_direction] || random_direction == (2 + direction) % 4)
				{
					//We keep picking a random direction until we can use it.
					random_direction = rand() % 4;
				}

				direction = random_direction;
			}
			else
			{
				//If there's no other way, it turns back.
				direction = (2 + direction) % 4;
			}
		}
		else
		{
			frightened_speed_timer--;
		}
	}

	//If the gohst can move, we move it.
	if (1 == move)
	{
		switch (direction)
		{
			case 0:
			{
				position.x += speed;

				break;
			}
			case 1:
			{
				position.y -= speed;

				break;
			}
			case 2:
			{
				position.x -= speed;

				break;
			}
			case 3:
			{
				position.y += speed;
			}
		}

		//Warping.
		//When the gohst leaves the map, we move it to the other side.
		if (-CELL_SIZE >= position.x)
		{
			position.x = CELL_SIZE * MAP_WIDTH - speed;
		}
		else if (position.x >= CELL_SIZE * MAP_WIDTH)
		{
			position.x = speed - CELL_SIZE;
		}
	}

	if (1 == pacman_collision(i_pacman.get_position()))
	{
		if (0 == frightened_mode) //When the gohst is not frightened and collides with Pacman, we kill Pacman.
		{
			i_pacman.set_dead(1);
		}
		else //Otherwise, the gohst starts running towards the house.
		{
			use_door = 1;

			frightened_mode = 2;

			target = home;
		}
	}
}

void Ghost::update_target(unsigned char i_pacman_direction, const Position& i_ghost_0_position, const Position& i_pacman_position)
{
	if (1 == use_door) //If the gohst can use the door.
	{
		if (position == target)
		{
			if (home_exit == target) //If the gohst has reached the exit.
			{
				use_door = 0; //It can no longer use the door.
			}
			else if (home == target) //If the gohst has reached its home.
			{
				frightened_mode = 0; //It stops being frightened.

				target = home_exit; //And starts leaving the house.
			}
		}
	}
	else
	{
		if (0 == movement_mode) //The scatter mode
		{
			//Each gohst goes to the corner it's assigned to.
			switch (id)
			{
				case 0:
				{
					target = {CELL_SIZE * (MAP_WIDTH - 1), 0};

					break;
				}
				case 1:
				{
					target = {0, 0};

					break;
				}
				case 2:
				{
					target = {CELL_SIZE * (MAP_WIDTH - 1), CELL_SIZE * (MAP_HEIGHT - 1)};

					break;
				}
				case 3:
				{
					target = {0, CELL_SIZE * (MAP_HEIGHT - 1)};
				}
			}
		}
		else //The chase mode
		{
			switch (id)
			{
				case 0: //The red gohst will chase Pacman.
				{
					target = i_pacman_position;

					break;
				}
				case 1: //The pink gohst will chase the 4th cell in front of Pacman.
				{
					target = i_pacman_position;

					switch (i_pacman_direction)
					{
						case 0:
						{
							target.x += CELL_SIZE * GHOST_1_CHASE;

							break;
						}
						case 1:
						{
							target.y -= CELL_SIZE * GHOST_1_CHASE;

							break;
						}
						case 2:
						{
							target.x -= CELL_SIZE * GHOST_1_CHASE;

							break;
						}
						case 3:
						{
							target.y += CELL_SIZE * GHOST_1_CHASE;
						}
					}

					break;
				}
				case 2: //The blue gohst.
				{
					target = i_pacman_position;

					//Getting the second cell in front of Pacman.
					switch (i_pacman_direction)
					{
						case 0:
						{
							target.x += CELL_SIZE * GHOST_2_CHASE;

							break;
						}
						case 1:
						{
							target.y -= CELL_SIZE * GHOST_2_CHASE;

							break;
						}
						case 2:
						{
							target.x -= CELL_SIZE * GHOST_2_CHASE;

							break;
						}
						case 3:
						{
							target.y += CELL_SIZE * GHOST_2_CHASE;
						}
					}

					//We're sending a vector from the red gohst to the second cell in front of Pacman.
					//Then we're doubling it.
					target.x += target.x - i_ghost_0_position.x;
					target.y += target.y - i_ghost_0_position.y;

					break;
				}
				case 3: //The orange gohst will chase Pacman until it gets close to him. Then it'll switch to the scatter mode.
				{
					//Using the Pythagoras' theorem again.
					if (CELL_SIZE * GHOST_3_CHASE <= sqrt(pow(position.x - i_pacman_position.x, 2) + pow(position.y - i_pacman_position.y, 2)))
					{
						target = i_pacman_position;
					}
					else
					{
						target = {0, CELL_SIZE * (MAP_HEIGHT - 1)};
					}
				}
			}
		}
	}
}

Position Ghost::get_position()
{
	return position;
}
