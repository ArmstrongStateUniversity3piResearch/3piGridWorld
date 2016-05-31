/*
 * prompt_user.c
 * Defines functions that prompt the user to set robot values.
 */ 
#include <pololu/3pi.h>
#include "global_vars.h"
#include "prompt_user.h"

// Prompts user to set robot direction
void prompt_set_direction()
{
	// Set robot direction
	while (!button_is_pressed(BUTTON_A))
	{
		clear();
		
		print("Set dir:");
		
		lcd_goto_xy(0, 1);
		
		print(NESW[r3PI.d]);
		
		if (button_is_pressed(BUTTON_B))
		r3PI.d < 3 ? r3PI.d++ : (r3PI.d = 0);
		
		delay_ms(100);
	}
	wait_for_button_release(BUTTON_A);
	delay_ms(100);
}

// Prompts user to set robot location
void prompt_set_location()
{
	// Set robot location
	while (!button_is_pressed(BUTTON_A))
	{
		clear();
		
		print("Start loc: ");
		
		lcd_goto_xy(0, 1);
		
		print_long(r3PI.x);
		print(" ");
		print_long(r3PI.y);
		
		if (button_is_pressed(BUTTON_B))
			r3PI.x < MAX_X ? r3PI.x++ : (r3PI.x = 0);
		if (button_is_pressed(BUTTON_C))
			r3PI.y < MAX_Y ? r3PI.y++ : (r3PI.y = 0);
		
		delay_ms(100);
	}
	wait_for_button_release(BUTTON_A);
	delay_ms(100);
}