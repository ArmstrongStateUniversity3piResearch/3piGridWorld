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

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3


unsigned int SPEED = 100;
unsigned int sensors[5];

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
		//print_long(bat);
		//print("mV");
		
		//lcd_goto_xy(0, 1);
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

void follow_segment()
{
	int last_proportional = 0;
	long integral=0;

	while(1)
	{
		// Normally, we will be following a line.  The code below is
		// similar to the 3pi-linefollower-pid example, but the maximum
		// speed is turned down to 60 for reliability.

		// Get the position of the line.
		unsigned int sensors[5];
		unsigned int position = read_line(sensors,IR_EMITTERS_ON);

		// The "proportional" term should be 0 when we are on the line.
		int proportional = ((int)position) - 2000;
		
		// Compute the derivative (change) and integral (sum) of the
		// position.
		int derivative = proportional - last_proportional;
		integral += proportional;

		// Remember the last position.
		last_proportional = proportional;

		// Compute the difference between the two motor power settings,
		// m1 - m2.  If this is a positive number the robot will turn
		// to the left.  If it is a negative number, the robot will
		// turn to the right, and the magnitude of the number determines
		// the sharpness of the turn.
		int power_difference = proportional/20 + integral/10000 + derivative*3/2;

		// Compute the actual motor settings.  We never set either motor
		// to a negative value.
		const int max = 40; // the maximum speed
		if(power_difference > max)
		power_difference = max;
		if(power_difference < -max)
		power_difference = -max;
		
		if(power_difference < 0)
		set_motors(max+power_difference,max);
		else
		set_motors(max,max-power_difference);

		// We use the inner three sensors (1, 2, and 3) for
		// determining whether there is a line straight ahead, and the
		// sensors 0 and 4 for detecting lines going to the left and
		// right.

		if(sensors[1] < 100 && sensors[2] < 100 && sensors[3] < 100)
		{
			// There is no line visible ahead, and we didn't see any
			// intersection.  Must be a dead end.
			return;
		}
		else if(sensors[0] > 400 || sensors[4] > 400)
		{
			// Found an intersection.
			return;
		}
		
	}
}

void turn(char direction)
{
	switch(direction)
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
	
	if(dir = d) return;
	int diff = d - dir;
	if(diff < 0) diff = diff * -1;
	
	if(diff == 2) {
		turn('B'); 
	} else {
		switch(dir) {
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
		}
	}
	dir = d;
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
			follow_segment();
			changeX();
		}

		changeDir(ydir);
		while(py !=y) {
			follow_segment();
			changeY();
		}

}

void changeX() {
	if(dir == EAST) px = px+1;
	else if(dir == WEST) px = px-1;
	else set_motors(0, 0);
}

void changeY() {
	if(dir == NORTH) py = py+1;
	else if(dir == WEST) py = py-1;
	else set_motors(0, 0);
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
