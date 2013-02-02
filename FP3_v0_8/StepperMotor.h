/*
* Copyright 2012/2013 by Frank Eggink <http://github.com/frankeggink/steppermotor>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#if defined(ARDUINO) && (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#ifndef STEPPERMOTOR
#define STEPPERMOTOR

#define RIGHT true
#define LEFT  false

//#define  NUM_STEPS  8

/**
* StepperMotor Class for use with the DFRobot L298 Motor shield.
* At the moment this class only support 4-wire bipolar steppers.
* This class uses half stepping to drive the steppermotors.
*/
class StepperMotor
{
private:

	int pin1; //E1
	int pin2; //M1
	int pin3; //E2
	int pin4; //M2

	volatile bool direction;

	int currentStep; //which step is motor on
	uint32_t stepsPerRev; //steps per revolution
	uint32_t millisecs_per_step; //speed


	//private methods 
	void step0();
	void step1();
	void step2();
	void step3();
	void step4();
	void step5();
	void step6();
	void step7();  	
	void makeStep(int step);
	void calculateNextStep();

public:
	/**
	* StepperMotor Constructor.
	* @param pin1,pin2,pin3,pin4 are the arduino pin numbers that are connected to the motorshield pins (E1/M1/E2/M2).
	* @param stepsPerRevolution is the number of steps it takes for the steppermotor to make one revolution.
	*/
	StepperMotor(int pin1, int pin2, int pin3, int pin4, int stepsPerRevolution);
	
	
	/**
	* This function is used to set the direction the steppermotor is turning.
	* @param direction sets the direction (LEFT or RIGHT).
	*/
	void setDirection(bool direction);
	
	
	/**
	* This function resets the stepper to a known state.
	*/
	void resetStepper();


	/**
	* This function turns the steppermotor off to prevent overheating and preserving power.
	*/
	void motorOff();


	/**
	* This function steps the steppermotor.
	* @param steps is the number of steps you want the steppermotor to make.
	*/
	void doStep(int steps);


	/**
	* This function returns the stepnumber the steppermotor is currently on.
	* @return the stepnumber the steppermotor is on.
	*/
	int getCurrentStep();


	/**
	* This function sets the speed of the steppermotor.
	* @param rpm is the speeds in rounds per minute.
	*/
	void setSpeed(uint32_t rpm);
};

#endif
