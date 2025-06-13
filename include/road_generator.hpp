#pragma once

#include <array>
#include <random.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <settings.hpp>



enum ROADTYPE {
	STRAIGHT = 0,
	CORNER,
	TJUNCTION,
	CROSSROADS,
	PATH,
	NONE
};

struct RoadTile {
	glm::mat4 model;
	glm::vec3 position;
	float angle;
	ROADTYPE type = ROADTYPE::NONE;

	RoadTile() = default;
	RoadTile(glm::vec3 _position, float _angle, ROADTYPE _type = STRAIGHT) {
		position = _position;
		type = _type;
		angle = _angle;
		model = glm::translate(glm::mat4(1.0f), position);  // First, translate to the correct position
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));  // Then, rotate around the translated position

	};

};




struct RoadGenerator {


	static void generate(std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& grid) {
		int roadsLeft = Settings::GRID_HEIGHT / 2 * 1.5;
		for (std::array<int, Settings::GRID_HEIGHT>& row : grid) {
			row.fill(0);
		}


		generateRoad(5, 0, 2, roadsLeft, grid);

		while (roadsLeft > 0) {
			int x = 0, y = 0;
			int direction = -1;


			int edge = Random::getRangeInt(0, 3);

			switch (edge) {
			case 0:
				x = Random::getRangeInt(1, Settings::GRID_WIDTH - 2);
				y = 0;
				direction = 2;
				break;
			case 1:
				x = Settings::GRID_WIDTH - 1;
				y = Random::getRangeInt(1, Settings::GRID_HEIGHT - 2);
				direction = 1;
				break;
			case 2:
				x = Random::getRangeInt(1, Settings::GRID_WIDTH - 2);
				y = Settings::GRID_HEIGHT - 1;
				direction = 0;
				break;
			case 3:
				x = 0;
				y = Random::getRangeInt(1, Settings::GRID_HEIGHT - 2);
				direction = 3;
				break;
			}


			int maxTries = 10;
			int tries = 0;

			while (tries < maxTries && isParallelNextTo(grid, x, y, direction)) {
				tries++;

				edge = Random::getRangeInt(0, 3);
				switch (edge) {
				case 0:
					x = Random::getRangeInt(1, Settings::GRID_WIDTH - 2);
					y = 0;
					direction = 2;
					break;
				case 1:
					x = Settings::GRID_WIDTH - 1;
					y = Random::getRangeInt(1, Settings::GRID_HEIGHT - 2);
					direction = 1;
					break;
				case 2:
					x = Random::getRangeInt(1, Settings::GRID_WIDTH - 2);
					y = Settings::GRID_HEIGHT - 1;
					direction = 0;
					break;
				case 3:
					x = 0;
					y = Random::getRangeInt(1, Settings::GRID_HEIGHT - 2);
					direction = 3;
					break;
				}
			}

			if (!isParallelNextTo(grid, x, y, direction)) {
				generateRoad(x, y, direction, roadsLeft, grid);
			}
			else {
				roadsLeft--;
			}

		}

		for (int y = 0; y < Settings::GRID_HEIGHT; y++) {
			for (int x = 0; x < Settings::GRID_WIDTH; x++) {
				if (grid[y][x] == 0) continue;
				if (nextToCount(grid, x, y) == 4){
					grid[x][y] = 3;
				}

			}

		}

		removeParallelStrips(grid);

		/*
		for (int y = 0; y < Settings::GRID_HEIGHT; y++) {
			for (int x = 0; x < Settings::GRID_WIDTH; x++) {
				if (grid[y][x] == 0) continue;
				grid[y][x] = nextToCount(grid, x, y) - 1;

			}

		}*/


		for (std::array<int, Settings::GRID_HEIGHT> &row : grid) {
			for (int i : row) {
				std::cout << i << " ";
			}
			std::cout << "\n";
		}

	}

	static void generateTiles(std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& Igrid, std::array<std::array<RoadTile, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& grid) {

		for (int y = 0; y < Settings::GRID_HEIGHT; y++) {
			for (int x = 0; x < Settings::GRID_WIDTH; x++) {
				if (Igrid[y][x] == 0) continue;
				bool u = y == 0 || Igrid[y - 1][x] > 0;
				bool d = y == Settings::GRID_HEIGHT-1 || Igrid[y + 1][x] > 0;
				bool r = x == Settings::GRID_WIDTH-1 || Igrid[y][x + 1] > 0;
				bool l = x == 0 || Igrid[y][x - 1] > 0;

				glm::vec3 position(x - ((float)(Settings::GRID_WIDTH-1.f) / 2.f), 0.0f, y - ((float)(Settings::GRID_HEIGHT-1) / 2.f)); // Adjust position to center the grid
				ROADTYPE type = ROADTYPE::NONE;
				float angle = 0.0f;

				// Determine the road type and rotation
				if (u && d && r && l) {
					// Crossroads (all directions have roads)
					type = CROSSROADS;
				}
				else if ((u && d && r) || (u && d && l) || (r && l && u) || (r && l && d)) {
					// T-junctions (three roads)
					type = TJUNCTION;

					if (l && d && r) {
						angle = 0.f;
					}
					else if (l && u && r) {
						angle = 180.f;
					}
					else if (l && u && d) {
						angle = 270.f;
					}
					else if (r && u && d) {
						angle = 90.f;
					}

				}
				else if ((u && d) || (r && l)) {
					// Straight road (only two directions)
					type = STRAIGHT;
					angle = (r && l) ? 0.0f : 90.0f; // Horizontal or vertical
				}
				else if ((u && r) || (r && d) || (d && l) || (l && u)) {
					// Corner roads (two adjacent directions)
					type = CORNER;

					if (u && r) angle = 180.0f; // Up and right corner
					else if (r && d) angle = 90.0f; // Right and down corner
					else if (d && l) angle = 0.0f; // Down and left corner
					else if (l && u) angle = 270.0f; // Left and up corner
				}

				// Assign the calculated road type and angle
				grid[y][x] = RoadTile(position, angle, type);
				

			}

		}

	}

private:
	static void generateRoad(int x, int y, int direction, int &roadsLeft, std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& grid) {
		roadsLeft--;
		int xIncrement = 0, yIncrement = 0;
		switch (direction)
		{
		case 0:
			yIncrement = -1;
			break;
		case 1:
			xIncrement = -1;
			break;
		case 2:
			yIncrement = 1;
			break;
		case 3:
			xIncrement = 1;
			break;

		default:
			break;
		}

		bool branched = true;
		
		while (x >= 0 && x < Settings::GRID_WIDTH && y >= 0 && y < Settings::GRID_HEIGHT) {
			
			grid[y][x]++;
			if (roadsLeft > 0 && !branched && x > 0 && x < Settings::GRID_WIDTH-1 && y > 0 && y < Settings::GRID_HEIGHT-1 && Random::getRange(0, 1) < 0.3) {
				int directionChange = Random::getRangeInt(0, 1) * 2 - 1;
                int newDirection = (direction + directionChange) % 4;
				if (newDirection < 0) newDirection = 3;
                if (!isParallelNextTo(grid, x, y, newDirection)) {
                    generateRoad(x, y, newDirection, roadsLeft, grid);
					if (Random::getRangeInt(0, 1) < 0.3) {
						roadsLeft++;
						return;
					}
                    branched = true;
                }
				
				
			}
			else if (branched) {
				branched = false;
			}
			

			x += xIncrement;
			y += yIncrement;

			
		}


	}


	static bool isNextTo(std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& grid, int x, int y, int ignore=-1) {
		if (x < 0 || x > Settings::GRID_WIDTH-1 || y < 0 || y > Settings::GRID_HEIGHT-1) return false;
		if (x > 0 && grid[y][x-1] > 0 && (ignore != 3 && ignore != 1)) return true;
		if (x < Settings::GRID_WIDTH-1 && grid[y][x+1] > 0 && (ignore != 1 && ignore != 3)) return true;
		if (y > 0 && grid[y-1][x] > 0 && (ignore != 0 && ignore != 2)) return true;
		if (y < Settings::GRID_HEIGHT-1 && grid[y+1][x] > 0 && (ignore != 2 && ignore != 0)) return true;

		return false;
	}

	static int nextToCount(std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& grid, int x, int y) {
		int adj = 0;
		if (x < 0 || x > Settings::GRID_WIDTH - 1 || y < 0 || y > Settings::GRID_HEIGHT - 1) return 0;
		if (x > 0 && grid[y][x - 1] > 0) adj++;
		if (x < Settings::GRID_WIDTH - 1 && grid[y][x + 1] > 0) adj++;
		if (y > 0 && grid[y - 1][x] > 0) adj++;
		if (y < Settings::GRID_HEIGHT - 1 && grid[y + 1][x] > 0) adj++;
		return adj;
	}

	static bool isParallelNextTo(std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& grid, int x, int y, int direction) {
		int xIncrement = 0, yIncrement = 0;
		switch (direction)
		{
		case 0:
			yIncrement = -1;
			break;
		case 1:
			xIncrement = -1;
			break;
		case 2:
			yIncrement = 1;
			break;
		case 3:
			xIncrement = 1;
			break;

		default:
			break;
		}

		
		if (true) {
			x += xIncrement;
			y += yIncrement;

			return isNextTo(grid, x, y, direction);

		}
		else {
			return false;
		}


    }


	static int getDiagonals(std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& grid, int x, int y) {
		int diagonalCounter = 0;
		if (x > 0 && y > 0) {
			if (grid[y - 1][x - 1] > 0) diagonalCounter++;
		}
		if (x < Settings::GRID_WIDTH-1 && y > 0) {
			if (grid[y - 1][x + 1] > 0) diagonalCounter++;
		}
		if (x > 0 && y < Settings::GRID_HEIGHT-1) {
			if (grid[y + 1][x - 1] > 0) diagonalCounter++;
		}
		if (x < Settings::GRID_WIDTH-1 && y < Settings::GRID_HEIGHT-1) {
			if (grid[y + 1][x + 1] > 0) diagonalCounter++;
		}

		return diagonalCounter;

	}

	static void removeParallelStrips(std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& grid) {
		// Horizontal scanning
		for (int y = 0; y < Settings::GRID_HEIGHT - 1; y++) {
			int x = 0;
			while (x < Settings::GRID_WIDTH) {
				int start = x;
				while (x < Settings::GRID_WIDTH && grid[y][x] > 0 && grid[y + 1][x] > 0) {
					x++;
				}
				int length = x - start;

				if (length >= 3) {
					// Two horizontal rows from [start, x-1] are parallel
					if (shouldRemoveStrip(grid, y, y + 1, start, x - 1, true)) {
						removeMiddleOrOneStrip(grid, y + 1, start, x - 1, true);
					}
					else if (shouldRemoveStrip(grid, y + 1, y, start, x - 1, true)) {
						removeMiddleOrOneStrip(grid, y, start, x - 1, true);
					}
				}

				x++;
			}
		}

		// Vertical scanning
		for (int x = 0; x < Settings::GRID_WIDTH - 1; x++) {
			int y = 0;
			while (y < Settings::GRID_HEIGHT) {
				int start = y;
				while (y < Settings::GRID_HEIGHT && grid[y][x] > 0 && grid[y][x + 1] > 0) {
					y++;
				}
				int length = y - start;

				if (length >= 3) {
					// Two vertical columns from [start, y-1] are parallel
					if (shouldRemoveStrip(grid, x, x + 1, start, y - 1, false)) {
						removeMiddleOrOneStrip(grid, x + 1, start, y - 1, false);
					}
					else if (shouldRemoveStrip(grid, x + 1, x, start, y - 1, false)) {
						removeMiddleOrOneStrip(grid, x, start, y - 1, false);
					}
				}

				y++;
			}
		}
	}

	static bool shouldRemoveStrip(std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& grid,
		int y1orx1, int y2orx2, int from, int to, bool horizontal) {
		// Test if the secondary strip can be removed without isolating the primary
		for (int i = from; i <= to; i++) {
			int cx = horizontal ? i : y2orx2;
			int cy = horizontal ? y2orx2 : i;
			if (nextToCount(grid, cx, cy) <= 1) {
				return false;
			}
		}
		return true;
	}

	static void removeMiddleOrOneStrip(std::array<std::array<int, Settings::GRID_WIDTH>, Settings::GRID_HEIGHT>& grid,
		int fixed, int from, int to, bool horizontal) {
		for (int i = from; i <= to; i++) {
			int x = horizontal ? i : fixed;
			int y = horizontal ? fixed : i;

			// Keep endpoints if necessary
			if (i == from || i == to) continue;

			grid[y][x] = 0;
		}
	}



};