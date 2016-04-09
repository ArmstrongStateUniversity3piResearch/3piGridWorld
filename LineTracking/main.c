// The 3pi include file must be at the beginning of any program that
// uses the Pololu AVR library and 3pi.
#include <pololu/3pi.h>
// This include file allows data to be stored in program space.  The
// ATmega168 has 16k of program space compared to 1k of RAM, so large
// pieces of static data should be stored in program space.
#include <avr/pgmspace.h>
#include "follow-segment.h"

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
// TODO implement user input with A, B, and C buttons to change this.
#define MAX_X 5
#define MAX_Y 5

// Introductory messages.  The "PROGMEM" identifier causes the data to
// go into program space.
const char welcome_line1[] PROGMEM = " Pololu";
const char welcome_line2[] PROGMEM = "3\xf7 Robot";
const char demo_name_line1[] PROGMEM = "Maze";
const char demo_name_line2[] PROGMEM = "solver";
// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char go[] PROGMEM = "L16 cdegreg4";
// const char mario[] PROGMEM = "T216 L4 e8e8r8e8r8c8egr<<grc.<g8r<e.aba#8ag6>e6>g6>a<f8g8r8ec8d8<b.";
const char mario1[] PROGMEM = "T216 L8 eererce4g4r4<<g4r4";
const char mario2[] PROGMEM = "T216 L8 c.4<gr4<e.4a4b4a#a4g6>e6>g6>a4<fgre4cd<b.4";

const char **NESW = {"North", "East", "South", "West"};
//					  0        1       2        3

struct Location {
	int x; // The robot's current x position. 
	int y; // The robot's current y position.
	unsigned int d:2;
		// Direction: 0 - North 1 - East 2 - South 3 - West mod 4
		// 2 bits allocated for 0 - 3 range; may automatically compute modulo if leading bits are omitted.
	int moveMemory [2][500];
		// Saves movement memory based off x and y values. With this, the 3pi will remember where it has been on the Grid World and will be able to retrace its previous path. x location data will be stored at [0][i] and y location data will be stored at [1][i].
		// TODO: Add function for recording these points.
};

struct Location 3PI;

// Initializes the 3pi, displays a welcome message, calibrates, and
// plays the initial music.
void initialize() {
	
	// Robot MUST be placed at (0, 0) facing "north": the top edge of the Grid World.
	3PI.x = 0;
	3PI.y = 0;
	3PI.d = 0;
	
	unsigned int counter;
	unsigned int sensors[5];
	
	pololu_3pi_init(2000);
	play_from_program_space(mario1);
	
	while(!button_is_pressed(BUTTON_B)) {
		int bat = read_battery_millivolts();
		
		clear();
		print_long(bat);
		print("mV");
		lcd_goto_xy(0, 1);
		print("Press B");
		play_from_program_space(mario2);
		
		delay_ms(100);
	}
	
	wait_for_button_release(BUTTON_B);
	delay_ms(1000);
	
	for(counter=0;counter<80;counter++) {
		if(counter <20 || counter >=60)	{
			set_motors(40, -40);
		} else {
			set_motors(-40, 40);
		}
		
		calibrate_line_sensors(IR_EMITTERS_ON);
		delay_ms(20);
	}
	
	set_motors(0,0);
	
	while(!button_is_pressed(BUTTON_B)) {
		unsigned int position = read_line(sensors, IR_EMITTERS_ON);
		clear();
		
		print_long(position);
	}
	
	wait_for_button_release(BUTTON_B);
	
	clear();
}


// IGNORE THIS METHOD FOR NOW, BUT IT HAS SOME USEFUL CODE
/*
void gotoPoint(int x, int y) {
	
	int last_proportional = 0;
	long integral=0; 
	
	// IF WE AREN'T AT THE POINT (BOOL VALUE OF 0)
	// THEN WE WANT TO FOLLOW AN ALGORITHM THAT WILL
	// BRING US TO THAT POINT
	
	while(atPoint(x, y) != 1) {	
				
		if(hasSegment() == 1) {
			break;
		} else if (hasSegment() == 2) {
			break;
		} else {
			
			// THIS IS A DIRECT RIP OF followSegment()
			// FROM followSegment.c
			// - RYAN
			unsigned int sensors[5];
			unsigned int position = read_line(sensors,IR_EMITTERS_ON);
			int proportional = ((int)position) - 2000;
			int derivative = proportional - last_proportional;
			integral += proportional;
			last_proportional = proportional;
			int power_difference = proportional/20 + integral/10000 + derivative*3/2;
			const int max = MAX_SPEED;
			
			if(power_difference > max)
			power_difference = max;
			if(power_difference < -max)
			power_difference = -max;
			
			if(power_difference < 0)
			set_motors(max+power_difference,max);
			else
			set_motors(max,max-power_difference);
		}
	}
}
*/

void turn(char dir)
{
	switch(dir)
	{
		case 'L':
		// Turn left.
		set_motors(-80,80);
		delay_ms(200);
		break;
		case 'R':
		// Turn right.
		set_motors(80,-80);
		delay_ms(200);
		break;
		case 'B':
		// Turn around.
		set_motors(80,-80);
		delay_ms(400);
		break;
		case 'S':
		// Don't do anything!
		break;
	}
}


void changeDir(int d) {
	
	if(3PI.d == d) return;
	int diff = d - 3PI.d;
	if(diff < 0) diff = diff * -1;
	
	if(diff == 2) {
		turn('B'); 
	} else {
		switch(3PI.d) {
			case NORTH:
				if(d == EAST) turn('R');// turn right
				if(d == WEST) turn('L');// turn left
				break;
			
			case SOUTH:
				if(d == EAST) turn('L'); // turn left
				if(d == WEST) turn('R'); // turn right
				break;
			
			case EAST:
				if(d == NORTH) turn('L'); // turn left
				if(d == SOUTH) turn('R'); // turn right
				break;
			
			case WEST:
				if(d == NORTH) turn('R'); // turn right
				if(d == SOUTH) turn('L'); // turn left
				break;
		}
		3PI.d = d;
	}
	
}

// Go to a specific point on the grid
void gotoPoint(int x, int y) {
	
		int xdiff = x - 3PI.x;
		int ydiff = y - 3PI.y;
		int xdir = 0;
		int ydir = 0;

		if(xdiff > 0) xdir = EAST;
		else xdir = WEST;
		if(ydiff > 0) ydir = NORTH;
		else ydir = SOUTH;

		changeDir(xdir);
		while(3PI.x != x) {
			// PROBABLY HAVE TO MOVE changeX() AND changeY() INTO follow_segment()
			follow_segment();
			changeX();
			// move forward
		}

		changeDir(ydir);
		while(3PI.y != y) {
			follow_segment();
			changeY();
			// move forward
		}

}

void changeX() {
	// based on current direction, change the x value
	if(3PI.d == EAST) 3PI.x++;
	else if(3PI.d == WEST) 3PI.x--;
}

void changeY() {
	// based on current direction, change the y value;
	if(3PI.d == NORTH) 3PI.y++;
	else if(3PI.d == SOUTH) 3PI.y--;
}

int foundIntersection() {
	return 0;
}

// 'BOOLEAN' value to see if we've reached a point
// Mostly just a failsafe if we need it
// RETURN 1 if true;
int atPoint(int x, int y) {
	if(3PI.x == x && 3PI.y == y) return 1;
	else return 0;
}

// USE gotoPoint for the remaining methods
// Boolean confirmation on intersection located.
int gotoIntersection(int x, int y) {
	gotoPoint(x, y);
	if (3PI.x == x && 3PI.y == y) return 1;
	return 0;
}

int gotoCorner(int c) {
	switch(c)
	{
		// 0 - 3 are corners around the Grid World going counterclockwise starting at (0, 0)
		case 0: return gotoIntersection(0, 0);
		case 1: return gotoIntersection(MAX_X, 0);
		case 2: return gotoIntersection(MAX_X, MAX_Y);
		case 3: return gotoIntersection(0, MAX_Y);
	}
}

int gotoEdge(int e) {
	switch(e)
	{
		// 0 - 3 are edges/sides around the Grid World going counterclockwise starting at the bottom edge.
		case 0: return gotoIntersection(0, 3PI.y);
		case 1: return gotoIntersection(3PI.x, 0);
		case 2: return gotoIntersection(MAX_X, 3PI.y);
		case 3: return gotoIntersection(3PI.x, MAX_Y);
	}
}

// Returns what type of segment we have
// Probably will be obsolete.
int hasSegment()
{
		if(sensors[1] < 100 && sensors[2] < 100 && sensors[3] < 100) {
			return 2; // HAS NOTHING
			print("NO LN");
		} else if(sensors[0] > 200 || sensors[4] > 200) {
			return 1; // HAS INTERSECTION
			print("INTER");
		} else {
			return 0; // HAS SEGMENT
			print("HAS SEG");
		}
}

void print3PI(struct Location r)
{
	print("(%d, %d) facing %s", r.x, r.y, NESW[r.d]);
}

int main()
{
	// set up the 3pi
	initialize();
	
	unsigned int position;
	char turns[] = {'S', 'S', 'S', 'S', 'R', 'S', 'S', 'S', 'S'}; // basic turning test
	int turnIndex = 0;
	while (1)
	{
		follow_segment();
		
		// Drive straight a bit.  This helps us in case we entered the
		// intersection at an angle.
		// Note that we are slowing down - this prevents the robot
		// from tipping forward too much.
		set_motors(50,50);
		delay_ms(50);

		// Now read the sensors and check the intersection type.
		read_line(sensors,IR_EMITTERS_ON);
		
		// Check for left and right exits
		
				
		// Drive straight a bit more - this is enough to line up our
		// wheels with the intersection.
		set_motors(40,40);
		delay_ms(200);

		// Check for a straight exit.
		read_line(sensors,IR_EMITTERS_ON);
		// if(sensors[1] > 200 || sensors[2] > 200 || sensors[3] > 200)
		
		
		// If the maze has been solved, we can follow the existing
		// path.  Otherwise, we need to learn the solution.
		//unsigned char dir = select_turn(found_left, found_straight, found_right);

		// Make the turn indicated by the path.
		//turn(dir);
		
		if (turnIndex < 8)
			turn(turns[turnIndex++]);
		else
		{
			set_motors(0, 0);
			while(!button_is_pressed(BUTTON_B));
			turnIndex = 0;
			delay_ms(1000);
		}
	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
