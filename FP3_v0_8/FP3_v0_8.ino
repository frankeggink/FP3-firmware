/*
* Copyright 2012/2013 by Frank Eggink <http://github.com/frankeggink/FP3-firmware>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>
*/


//FP3 Embedded Software v0.8
//Frank Eggink last updated on 31-01-2013 

#include <Servo.h>
#include "StepperMotor.h"

//for debugging purposes
#define OUTPUT_BRESENHAM_DATA
#undef OUTPUT_BRESENHAM_DATA

//version number
char *version = "FP3 v0.8";

//Steppers
StepperMotor yMotor(6,7,5,4,400); //was x
StepperMotor xMotor(10,12,9,8,400); 
int currentX = 0; //Stepper x-position
int currentY = 0; //Stepper y-position

//servo variables
Servo servo1;
#define DEFAULT_PEN_UP_POSITION 180          
#define SERVOPIN 2
boolean servoEnabled = true;

//parser variables
char *strchr_pointer;
double value;
int codenum;

//comm variables
const int MAX_CMD_SIZE = 256;
char buffer[MAX_CMD_SIZE];
char serial_char;
int serial_count = 0;
boolean comment_mode = false;

//other variables          
//TODO: variables absoluteMode and zoomFactor are not being used in the code
boolean absoluteMode = true;
double zoomFactor = 1.0;
double oldX = 0.;
double oldY = 0.;


/**
* This function does some initialization and is run only once at the start of the program.
*/
void setup() 
{
	Serial.begin(9600);
	servo1.attach(SERVOPIN);
	servo1.write(DEFAULT_PEN_UP_POSITION);

	xMotor.setSpeed(10);
	yMotor.setSpeed(10);

	//set stepper direction
	xMotor.setDirection(RIGHT);
	yMotor.setDirection(RIGHT);
	//move Y-Stepper bit to the right
	yMotor.doStep(2);
}

/**
* This function is run continuously.
*/
void loop()
{
	getSerialCommand();
}


/**
* This function is used to move both steppermotors to a new postition.
* @param targetX is the new X coordinate were you want te stepper to move to.
* @param targetY is the new Y coordinate were you want the stepper to move to.
*/
void moveSteppers(int targetX, int targetY) {
	if ((targetX == currentX) && (targetY == currentY)) {
		//do nothing
		Serial.println("no movement");
	}

	else if (targetX == currentX ) {
		//only Y-movement
		Serial.println("Only y-displacement");
		int deltaY = targetY - currentY;
		Serial.print("CurrentY = ");
		Serial.println(currentY);
		Serial.print("TargetY = ");
		Serial.println(targetY);
		Serial.print("deltaY = ");
		Serial.println(deltaY);

		if(deltaY < 0) {
			//left
			yMotor.setDirection(LEFT);
		}
		if(deltaY > 0) {
			//right
			yMotor.setDirection(RIGHT);
		} 

		deltaY = abs(deltaY);
		yMotor.doStep(deltaY);    
		currentY = targetY; //set the new currentY 
	}

	else if (targetY == currentY ) {
		//only X-movement

		Serial.println("Only x-displacement");
		int deltaX = targetX - currentX;
		Serial.print("CurrentX = ");
		Serial.println(currentX);
		Serial.print("TargetX = ");
		Serial.println(targetX);
		Serial.print("deltaX = ");
		Serial.println(deltaX);

		if(deltaX < 0) {
			//left
			xMotor.setDirection(LEFT);
		}
		if(deltaX > 0) {
			//right
			xMotor.setDirection(RIGHT);
		} 

		deltaX = abs(deltaX);
		xMotor.doStep(deltaX);    
		currentX = targetX; 
	}
	else {
		//use bresenham
		Serial.println("Using bresenham algorithm");
		bresenham(currentX, currentY, targetX, targetY);
		currentX = targetX; 
		currentY = targetY;
	}
}




/**
* This function is uses by the function "moveSteppers".
* When both the targetX and targetY differ from the current values we use this function to calculate the necessary x,y steps to take using the "bresenham algorithm"
* @param x0 is the current x value.
* @param y0 is the current y value.
* @param x1 is the new target x value.
* @param y1 is the new target y value.
*/
void bresenham(int x0, int y0, int x1, int y1)
{
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = dx+dy, e2; /* error value e_xy */


	int targetX = x1;
	int targetY = y1;


	//determine direction
	if (x1 < x0) {
		//x left
		xMotor.setDirection(LEFT);
	}
	else if (x1 > x0) {
		//x right
		xMotor.setDirection(RIGHT);
	}


	//determine direction
	if (y1 < y0) {
		//y left
		yMotor.setDirection(LEFT);                  
	}
	else if (y1 > y0) {
		//y right
		yMotor.setDirection(RIGHT);                  
	}



	while (!(x0==x1 && y0==y1)){
		e2 = 2*err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */

		//step x-motor
		int xStepToTake = x0 - currentX;
		Serial.print("xStepToTake = ");
		Serial.println(xStepToTake);
		xMotor.doStep(abs(xStepToTake));
		currentX = x0;

		//step y-motor
		int yStepToTake = y0 - currentY;
		Serial.print("yStepToTake = ");
		Serial.println(yStepToTake);  
		yMotor.doStep(abs(yStepToTake));
		currentY = y0;
	}
}


/**
* This function is used only for debugging purposes.
*
*/
void plotLine(int x0, int y0, int x1, int y1)
{
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = dx+dy, e2; /* error value e_xy */
	while (!(x0==x1 && y0==y1)){
		Serial.print(x0);
		Serial.print(",");
		Serial.println(y0);

		e2 = 2*err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}

/**
* This function is used to retrieve the commands the arduino receives on the serialport.
*/
void getSerialCommand()
{
	if(Serial.available() > 0) {
		serial_char = Serial.read();
		//say what you got:
		//Serial.print("I received: ");
		//Serial.println(serial_char, DEC);

		if (serial_char == '\n' || serial_char == '\r')
		{
			buffer[serial_count] = 0;
			//TODO: process command
			parse(buffer);

			//Serial.println(buffer);
			//Serial.println();
			clear_buffer();
			comment_mode = false;
		}
		else //not end of command
		{
			if(serial_char == ';' || serial_char == '(') 
			{ 
				comment_mode = true;
			}
			if (comment_mode != true)
			{
				buffer[serial_count] = serial_char;
				serial_count++;
				if(serial_count > MAX_CMD_SIZE)
				{
					clear_buffer();
					Serial.flush();

				}
			}
		}     
	}
}

/**
* This function is used to clear the character buffer.
*/
void clear_buffer()
{
	serial_count = 0;
}


/**
* This function is used to parse simple gcode commands.
*/
void parse(char *command) 
{

	double xVal;
	double yVal;
	double pVal;
	double fVal;
	double sVal;

	boolean hasXVal;
	boolean hasYVal;
	boolean hasPVal;
	boolean hasFVal;
	boolean hasSVal;


	if (command != NULL) {
		if (command[0] == 'G') {
			//G code 


			if(isdigit(command[1])) {  //is the second character a number?
				codenum = (int)strtod(&command[1],NULL);
			} else { 
				codenum = -1; //something is wrong with the received command
			}

			hasXVal = getValue('X', command, &xVal);
			hasYVal = getValue('Y', command, &yVal);
			hasPVal = getValue('P', command, &pVal);
			hasFVal = getValue('F', command, &fVal);


			switch(codenum)
			{
			case 0: 
				break;
			case 1: 
				// Serial.println();

#ifdef OUTPUT_BRESENHAM_DATA  
				plotLine((int)round(oldX),(int)round(oldY),(int)round(xVal), (int)round(yVal));
#else
				moveSteppers((int)round(xVal), (int)round(yVal));
#endif

				oldX = xVal; //used by the debug function "plotLine" to remember the current X position
				oldY = yVal; //used by the debug function "plotLine" to remember the current Y position
				break;
			case 2: //G2, Clockwise arc
			case 3: //G3, Counterclockwise arc

				break;
			case 4: //G4, Delay P ms
				//Serial.println("G4 not implemented");
				if (hasPVal) {
					unsigned long endDelay = millis() + (unsigned long)pVal;
					while (millis() < endDelay)
					{
						delay(1);
					}
				}
				break;
			case 90: //G90, Absolute Positioning
				//Serial.println("G90 not implemented");
				absoluteMode = true;
				break;
			case 91: //G91, Incremental Positioning
				//Serial.println("G91 not implemented");
				absoluteMode = false;
				break;
			default:
				//Serial.println("you've entered the default case!! error!?!?");
				;
			}      
		} else if (command[0] == 'M') {
			//M code

			if(isdigit(command[1])) {  //is the second character a number?
				codenum = (int)strtod(&command[1],NULL);
			} else { 
				codenum = -1; //something is wrong with the received command
			}

			hasSVal = getValue('S', command, &sVal);


			switch(codenum)
			{
			case 18: //M18, disable drives

				break;
			case 300: //M300, servo position
				if(hasSVal) {
					servoEnabled = true;
				}
				if (sVal < 0.) {
					sVal = 0;
				} else if (sVal > 180.) {
					sVal = DEFAULT_PEN_UP_POSITION;
					servo1.write((int)sVal);
				}
				servoEnabled = false;

				servo1.write((int)sVal);
				break;
			case 400: //M400, Reset X-Axix-Stepper settings to new object diameter

				break;
			case 401: //M401, Reset Y-Axis-Stepper settings to new object diameter

				break;
			case 402: //M402, set global zoom factor

				break;
			default:
				;
			}      
		} else if (command[0] == '@') {
			Serial.println(version);
		} else {
			//unknown command
			//Serial.println("Unknown command");
		}
	}
	// done processing commands

	Serial.println("ok:");

	//Have to move these 2 statements somewhere else!!!!!!!!!!!!!!!!
	//xMotor.motorOff(); //FE 21-01-2013
	//yMotor.motorOff(); //
}

/**
* This is a helper function to determine if a command contains a specific key value.
*/
boolean getValue(char key, char *command, double *value)
{
	//find key parameter
	strchr_pointer = strchr(command,key);
	if (strchr_pointer != NULL) //We found a key value
	{
		*value = (double)strtod(&command[strchr_pointer-command+1],NULL);
		return true;
	} else {
		return false;
	}
}
