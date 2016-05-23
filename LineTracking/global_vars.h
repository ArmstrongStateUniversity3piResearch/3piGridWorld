/*
 * global_vars.h
 * Declares variables needed in various functions
 */ 
#ifndef GLOBAL_VARS_H_
#define GLOBAL_VARS_H_

// Cardinal directions
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

// Size of Grid World
#define MAX_X 5
#define MAX_Y 5

// Array of the cardinal directions
extern const char *NESW[];

// Represents location and cardinal direction of robot
struct Location {
	int x; // The robot's current x position.
	int y; // The robot's current y position.
	unsigned int d:2;
	// Direction: 0 - North 1 - East 2 - South 3 - West mod 4
	// 2 bits allocated for 0 - 3 range; may automatically compute modulo if leading bits are omitted.
	// int moveMemory [2][500]; giving error with 500
	// Saves movement memory based off x and y values. With this, the 3pi will remember where it has been on the Grid World and will be able to retrace its previous path. x location data will be stored at [0][i] and y location data will be stored at [1][i].
	// TODO: Add function for recording these points.
};

struct Location r3PI;

// Holds sensor values to know robot's position relative to line
unsigned int sensors[5];

#endif /* GLOBAL_VARS_H_ */