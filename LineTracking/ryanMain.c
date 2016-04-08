/* 3pi_app1 - an application for the Pololu 3pi Robot
 *
 * This application uses the Pololu AVR C/C++ Library.  For help, see:
 * -User's guide: http://www.pololu.com/docs/0J20
 * -Command reference: http://www.pololu.com/docs/0J18
 *
 * Created: 1/15/2016 3:05:53 PM
 *  Author: Ryan
 */

#include <pololu/3pi.h> 


unsigned int SPEED = 100;
unsigned int sensors[5];

static int NORTH = 0;
static int EAST = 1;
static int SOUTH = 2;
static int WEST = 3;

unsigned int px = 0;
unsigned int py = 0;
unsigned int dir = NORTH; //START OFF BY GOING NORTH

unsigned int MAX_SPEED = 60;

void initialize() {
	
	unsigned int counter;
	
	pololu_3pi_init(2000);
	
	while(!button_is_pressed(BUTTON_B)) {
		int bat = read_battery_millivolts();
		
		clear();
		print_long(bat);
		print("mV");
		
		lcd_goto_xy(0, 1);
		print("Press B");
		
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
}*/

void changeDir(int d) {
	
	if(dir = d) return;
	int diff = d - dir;
	if(diff < 0) diff = diff * -1;
	
	if(diff == 2) {
		// rotate 180 degrees
	} else {
		switch(dir) {
			case NORTH:
			if(d == EAST) // turn right
			if(d == WEST) // turn left
			break;
			
			case SOUTH:
			if(d == EAST) // turn left
			if(d == WEST) // turn right
			break;
			
			case EAST:
			if(d == NORTH) // turn left
			if(d == SOUTH) // turn right
			break;
			
			case WEST:
			if(d == NORTH) // turn right
			if(d == SOUTH) // turn left
		}
	}
	
}

void gotoPoint(int x, int y) {

		int xdiff = x- px;
		int ydiff = y- py;
		int xdir = 0;
		int ydir = 0;

		if(xdiff > 0) xdir = EAST;
		else xdir = WEST;
		if(ydiff > 0) ydir = NORTH;
		else ydir = SOUTH;

		changeDir(xdir);
		while(px !=x) {
			if(foundIntersection() == 1) {
				changeX();
			}
			// move forward
		}

		changeDir(ydir);
		while(py !=y) {
			if(foundIntersection() == 1) {
				changeY();
			}
			// move forward
		}

}

void changeX() {
	// based on current direction, change the x value
}

void changeY() {
	// based on current direction, change the y value;
}

int foundIntersection() {
	return 0;
}


int atPoint(int x, int y) {
	if(px == x && py == y) return 1;
	else return 0;
}


int gotoIntersection(int x, int y) {
	return 0;
}

int gotoCorner(int c) {
	
	// switch statement, gotoIntersection
	return 0;
}

int gotoEdge(int e) {
	return 0;
}


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

int main()
{

	//unsigned int sensors[5];
	initialize();
	gotoPoint(1, 0);
	
	
}
