// ----------------------------------------------------------------------------
// Description
// ----------------------------------------------------------------------------

/*
	OutputDebugStringA()
	WriteConsole() (CAN'T)
*/

// ----------------------------------------------------------------------------
// Headers
// ----------------------------------------------------------------------------
#pragma warning(push, 0)
#include <stdint.h>
#include <string.h> // memset()
#include <stdio.h>  // sprintf()
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>
#pragma(pop)

#pragma warning(disable :5045)
// #pragma warning(disable :4711)
// #pragma warning(disable :4710)

// #define GRID_NEXT 0
// #define GRID_CURR 1

// ----------------------------------------------------------------------------
// Structs
// ----------------------------------------------------------------------------
typedef struct {
	uint32_t x;
	uint32_t y;
} start_coord_t;

// ----------------------------------------------------------------------------
// Globals
// ----------------------------------------------------------------------------
int is_allocated = 0;
uint8_t *grids = NULL;
uint8_t *ptr_next = NULL;
uint8_t *ptr_curr = NULL;

// Neighbour coord deltas
int dx[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
int dy[8] = {-1,  0,  1, -1, 1, -1, 0, 1};

// ----------------------------------------------------------------------------
// Solution Functions
// ----------------------------------------------------------------------------
static uint8_t* get_cell(uint8_t *map, uint32_t map_dim, uint32_t x, uint32_t y) {
	return &map[x + y * map_dim];
}

static void print_grid(uint8_t *map, const char *grid_name, uint32_t grid_dim) {
	char debug_buff[2048];
	char digits[10] = "0123456789";

	sprintf(debug_buff, "%s (%dx%d):\n", grid_name, grid_dim, grid_dim);
	OutputDebugStringA(debug_buff);
	
	// Print top ruler
	for (uint32_t i = 0; i < grid_dim; ++i) {
		sprintf(debug_buff, "%c", digits[i % 10]);
		OutputDebugStringA(debug_buff);
	}
	sprintf(debug_buff, "\n");
	OutputDebugStringA(debug_buff);

	// Print grid
	for (uint32_t r = 0; r < grid_dim; ++r) {
		sprintf(debug_buff, "%c", digits[r % 10]);
		OutputDebugStringA(debug_buff);

		for (uint32_t c = 0; c < grid_dim; ++c) {
			uint8_t *cell = get_cell(map, grid_dim, c, r);
			sprintf(debug_buff, "%c", *cell == 0 ? ' ' : '#');
			OutputDebugStringA(debug_buff);
		}
		sprintf(debug_buff, "\n");
		OutputDebugStringA(debug_buff);
	}
}

static void print_list(start_coord_t *initial_points, uint32_t initial_point_count) {
	char debug_buff[2048];

	sprintf(debug_buff, "initial_points = ");
	OutputDebugStringA(debug_buff);

	for (uint32_t i = 0; i < initial_point_count; ++i) {
		sprintf(debug_buff, "(%d, %d), ", initial_points[i].x, initial_points[i].y);
		OutputDebugStringA(debug_buff);
	}

	sprintf(debug_buff, "\n");
	OutputDebugStringA(debug_buff);
}

static int count_alive_neighbours(uint8_t *map, uint32_t map_dim, uint32_t x, uint32_t y) {
	int alive_neighbour_count = 0;

	for (int i = 0; i < 8; ++i) {
		// TODO: eliminate %
		int nx = (x + dx[i] + map_dim) % map_dim;
		int ny = (y + dy[i] + map_dim) % map_dim;

		uint8_t *neighbour_cell_to_check = get_cell(map, map_dim, nx, ny);

		if (*neighbour_cell_to_check) {
			alive_neighbour_count++;
		}
	}

	return alive_neighbour_count;
}

uint8_t *simulate_life(uint32_t grid_dim, start_coord_t *initial_points, uint32_t initial_point_count) {
	// TODO: Comment out
	// char debug_buff[2048];
	
	// Allocate
	// static int max_alive_cells = -1;
	// char buf[64] = {0};
	// snprintf(buf, sizeof(buf), "%d\n", max_alive_cells);
	// OutputDebugStringA(buf);

	const uint32_t GRID_SIZE = grid_dim * grid_dim;

	if (!is_allocated) {
		grids = calloc(GRID_SIZE * 2, sizeof(uint8_t));
		is_allocated = 1;

		ptr_curr = grids;
		ptr_next = grids + GRID_SIZE;

		// Put initial array into CURR
		for (uint32_t i = 0; i < initial_point_count; ++i) {
			uint8_t *curr_cell = get_cell(ptr_curr, grid_dim, initial_points[i].x, initial_points[i].y);
			*curr_cell = 1;
		}
	} else {
		// Swap next <--> curr
		uint8_t *tmp = ptr_curr;
		ptr_curr = ptr_next;
		ptr_next = tmp;

		// Flush NEXT
		memset(ptr_next, 0, GRID_SIZE * sizeof(uint8_t));
	}

	// TODO: For debug, print `initial_points` list`curr` grid
	// print_list(initial_points, initial_point_count);
	// TODO: For debug, print `curr` grid
	// print_grid(grids, GRID_CURR, grid_dim);
	// TODO: For debug, print `next` grid (should be all 0's now)
	// print_grid(grids, GRID_NEXT, grid_dim);

	// For each alive cell
	// int alive_cell_encountered = 0;
	// int next_alive_cells_counter = 0;

	for (uint32_t r = 0; r < grid_dim; ++r) {
		for (uint32_t c = 0; c < grid_dim; ++c) {
			// if (max_alive_cells != -1) { 
			// 	if (alive_cell_encountered > max_alive_cells) {
			// 		sprintf(debug_buff, "MAX ALIVE CELLS ENCOUNTERED\n");
			// 		OutputDebugStringA(debug_buff);
			// 		break;
			// 	}
			// }

			uint8_t *check_cell = get_cell(ptr_curr, grid_dim, c, r);
			if (*check_cell == 0) {
				continue; // It's dead, only check for alive cells
			} 
			// else {
			// 	alive_cell_encountered++;
			// }

			int alive_neighbour_count = count_alive_neighbours(ptr_curr, grid_dim, c, r);
		
			// Should this cell die
			uint8_t *next_cell = get_cell(ptr_next, grid_dim, c, r);

			if (alive_neighbour_count != 2 && alive_neighbour_count != 3) {
				*next_cell = 0;
				// sprintf(debug_buff, "Killed cell = (%d, %d)\n", initial_points[i].x, initial_points[i].y);
				// OutputDebugStringA(debug_buff);
			} else {
				*next_cell = 1;
				// next_alive_cells_counter++;
			}
			
			// TODO: Print alive neighbour count
			// char debug_buff[2048];
			// sprintf(debug_buff, "(Alive cell) alive_neighbour_count = %d\n", alive_neighbour_count);
			// OutputDebugStringA(debug_buff);
		
			// For each dead neighbour of alive cell
			for (uint32_t j = 0; j < 8; ++j) {
				// TODO: eliminate %
				int nx = (c + dx[j] + grid_dim) % grid_dim;
				int ny = (r + dy[j] + grid_dim) % grid_dim;

				uint8_t *dead_neighbour_cell = get_cell(ptr_curr, grid_dim, nx, ny);
				if (*dead_neighbour_cell == 1) {
					continue; // It's not dead, only check for dead neighbours
				}
				
				int alive_neighbour_count = count_alive_neighbours(ptr_curr, grid_dim, nx, ny);

				// Should this cell revive
				if (alive_neighbour_count == 3) {
					uint8_t *next_cell = get_cell(ptr_next, grid_dim, nx, ny);
					*next_cell = 1;
					// next_alive_cells_counter++;

					// sprintf(debug_buff, "Killed cell = (%d, %d)\n", nx, ny);
					// OutputDebugStringA(debug_buff);
				}
			}
		}

		// if (alive_cell_encountered > max_alive_cells) {
		// 	break;
		// }
	}

// after_iter:

	// TODO: For debug, print `next` grid
	// print_grid(grids, GRID_NEXT, grid_dim);

	// max_alive_cells = next_alive_cells_counter;

	return ptr_curr;
}

