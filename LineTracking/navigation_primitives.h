/*
 * navigation_primitives.h
 *
 * Declares functions needed for basic robot navigation.
 */ 
#include "global_vars.h"



#ifndef NAVIGATION_PRIMITIVES_H_
#define NAVIGATION_PRIMITIVES_H_

// Initializes robot's sensors for navigation
void initialize();

// Changes robot direction
void turn(char dir);

// Helper function to change robot direction
void changeDir(int d);

// Updates robot's x-coordinate value
void changeX();

// Updates robot's y-coordinate value
void changeY();

// Prints robot's location (coordinates and direction)
void print3PI();

void gotoPoint(int x, int y);

int foundIntersection();

// 'BOOLEAN' value to see if we've reached a point
// Mostly just a failsafe if we need it
// RETURN 1 if true;
int atPoint(int x, int y);

// USE gotoPoint for the remaining methods
// Boolean confirmation on intersection located.
int gotoIntersection(int x, int y);

int gotoCorner(int c);

int gotoEdge(int e);

// Returns what type of segment we have
// Probably will be obsolete.
int hasSegment();

int move(int dir, int units);


#endif /* NAVIGATION_PRIMITIVES_H_ */