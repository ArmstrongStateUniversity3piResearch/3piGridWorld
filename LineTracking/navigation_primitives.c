/*
 * navigation_primitives.c
 *
 * Defines functions needed for basic robot navigation.
 */ 

// The 3pi include file must be at the beginning of any program that
// uses the Pololu AVR library and 3pi.
#include <pololu/3pi.h>
// This include file allows data to be stored in program space.  The
// ATmega168 has 16k of program space compared to 1k of RAM, so large
// pieces of static data should be stored in program space.
#include <avr/pgmspace.h>

#include "global_vars.h"
#include "navigation_primitives.h"
#include "follow-segment.h"

// Introductory messages.  The "PROGMEM" identifier causes the data to
// go into program space.
const char welcome_line1[] PROGMEM = " Pololu";
const char welcome_line2[] PROGMEM = "3\xf7 Robot";
// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char go[] PROGMEM = "L16 cdegreg4";
// const char mario[] PROGMEM = "T216 L4 e8e8r8e8r8c8egr<<grc.<g8r<e.aba#8ag6>e6>g6>a<f8g8r8ec8d8<b.";
const char mario1[] PROGMEM = "T216 L8 eererce4g4r4<<g4r4";
const char mario2[] PROGMEM = "T216 L8 c.4<gr4<e.4a4b4a#a4g6>e6>g6>a4<fgre4cd<b.4";

void initialize() {
	// Robot MUST be placed at (0, 0) facing "north": the top edge of the Grid World.
	r3PI.x = 0;
	r3PI.y = 0;
	r3PI.d = 0;
	
	unsigned int counter;
	
	pololu_3pi_init(2000);
	play_from_program_space(mario1);
	// while(is_playing());
	
	while(!button_is_pressed(BUTTON_B)) {
		int bat = read_battery_millivolts();
		
		clear();
		print_long(bat);
		print("mV");
		lcd_goto_xy(0, 1);
		print("Press B");
		// play_from_program_space(mario1);
		// while (is_playing());
		
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

void turn(char dir)
{
	switch(dir)
	{
		case 'L':
		// Turn left.
		set_motors(-80,80);
		delay_ms(200);
		set_motors(0, 0);
		break;
		
		case 'R':
		// Turn right.
		set_motors(80,-80);
		delay_ms(200);
		set_motors(0, 0);
		break;
		
		case 'B':
		// Turn around.
		set_motors(80,-80);
		delay_ms(400);
		set_motors(0, 0);
		break;
		
		case 'S':
		// Don't do anything!
		break;
	}
}

void changeDir(int d) {
	
	if(r3PI.d == d) return;
	int diff = d - r3PI.d;
	if(diff < 0) diff = diff * -1;
	
	if(diff == 2) {
		turn('B');
		} else {
		switch(r3PI.d) {
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
	}
	r3PI.d = d;
}

void changeX() {
	// based on current direction, change the x value
	if(r3PI.d == EAST) r3PI.x++;
	else if(r3PI.d == WEST) r3PI.x--;
}

void changeY() {
	// based on current direction, change the y value;
	if(r3PI.d == NORTH) r3PI.y++;
	else if(r3PI.d == SOUTH) r3PI.y--;
}

void print3PI()
{
	// print("(%d, %d) facing %s", r.x, r.y, NESW[r.d]);
	clear();
	
	print("(");
	print_long(r3PI.x);
	print(", ");
	print_long(r3PI.y);
	print(")");
	lcd_goto_xy(0, 1);
	print("Facing ");
	print(NESW[r3PI.d]);
}

void gotoPoint(int x, int y) {
	// Tracks if robot has moved from its initial placement on the grid
	// Moves robot ahead a little from initial placement after its first turn (if any)
	// Ensures robot does not read initial intersection it is placed on as next intersection detected
	// Robot should be able to navigate without need for more delays at other intersections
	static int isMoved = 0;
	
	// Already at point
	if (r3PI.x == x && r3PI.y == y)
	{
		set_motors(0, 0);
		return;
	}
	
	int xdiff = x - r3PI.x;
	int ydiff = y - r3PI.y;
	int xdir = 0;
	int ydir = 0;

	if(xdiff > 0) xdir = EAST;
	else xdir = WEST;
	
	if(ydiff > 0) ydir = NORTH;
	else ydir = SOUTH;
	
	// Change direction towards x coordinate if not at x coordinate
	if (r3PI.x != x)
	{
		changeDir(xdir);
		
		if (!isMoved)
		{
			isMoved = 1;
			set_motors(50,50);
			delay_ms(50);
		}
	}
	
	// Navigate towards x coordinate
	while(r3PI.x != x) {
		// PROBABLY HAVE TO MOVE changeX() AND changeY() INTO follow_segment()
		follow_segment();
		
		// Drive straight a bit.  This helps us in case we entered the
		// intersection at an angle.
		// Note that we are slowing down - this prevents the robot
		// from tipping forward too much.
		set_motors(50,50);
		delay_ms(50);
		
		// Drive straight a bit more - this is enough to line up our
		// wheels with the intersection.
		set_motors(40,40);
		delay_ms(200);
		
		changeX();
		// move forward
	}

	// Change direction towards y coordinate if not at y coordinate
	if (r3PI.y != y)
	{
		changeDir(ydir);
		if (!isMoved)
		{
			isMoved = 1;
			set_motors(50,50);
			delay_ms(50);
		}
	}
	
	// Navigate towards y coordinate
	while(r3PI.y != y) {
		follow_segment();
		// Drive straight a bit.  This helps us in case we entered the
		// intersection at an angle.
		// Note that we are slowing down - this prevents the robot
		// from tipping forward too much.
		set_motors(50,50);
		delay_ms(50);
		
		// Drive straight a bit more - this is enough to line up our
		// wheels with the intersection.
		set_motors(40,40);
		delay_ms(200);
		
		changeY();
		// move forward
	}
	
	set_motors(0, 0);
}

int foundIntersection() {
	return 0;
}

// 'BOOLEAN' value to see if we've reached a point
// Mostly just a failsafe if we need it
// RETURN 1 if true;
int atPoint(int x, int y) {
	if(r3PI.x == x && r3PI.y == y) return 1;
	else return 0;
}

// USE gotoPoint for the remaining methods
// Boolean confirmation on intersection located.
int gotoIntersection(int x, int y) {
	gotoPoint(x, y);
	if (r3PI.x == x && r3PI.y == y) return 1;
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
		case 0: return gotoIntersection(r3PI.x, MAX_Y);
		case 1: return gotoIntersection(MAX_X, r3PI.y);
		case 2: return gotoIntersection(r3PI.x, 0);
		case 3: return gotoIntersection(0, r3PI.y);
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

int move(int dir, int units)
{
	switch(dir)
	{
		case 0: gotoPoint(r3PI.x, r3PI.y + units); break;
		case 1: gotoPoint(r3PI.x + units, r3PI.y); break;
		case 2: gotoPoint(r3PI.x, r3PI.y - units); break;
		case 3: gotoPoint(r3PI.x - units, r3PI.y); break;
	}
}
