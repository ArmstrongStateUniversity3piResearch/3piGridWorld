// The 3pi include file must be at the beginning of any program that
// uses the Pololu AVR library and 3pi.
#include <pololu/3pi.h>

#include "global_vars.h"
#include "navigation_primitives.h"
#include "prompt_user.h"

int main()
{
	// set up the 3pi
	initialize();
	
	// goto location
	int gotoX = 4;
	int gotoY = 2;
	
	gotoPoint(gotoX, gotoY);
	print3PI();
	
	for (int i = 0; i <= 5; i++)
	{
		gotoPoint(i, i);
	}
	print3PI();
	
	move(SOUTH, 2);
	print3PI();
	
	gotoCorner(3);
	print3PI();
	
	move(SOUTH, 2);
	print3PI();
	
	gotoEdge(2);
	print3PI();
	
	prompt_set_direction();
	print3PI();
	
	prompt_set_location();
	print3PI();
	
	prompt_set_direction();
	print3PI();
	
	while(1);
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
