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


#include "StepperMotor.h"


StepperMotor::StepperMotor(int pin1, int pin2, int pin3, int pin4, int stepsPerRevolution) {
	this->pin1=pin1;
	this->pin2=pin2;
	this->pin3=pin3;
	this->pin4=pin4;
	this->stepsPerRev = stepsPerRevolution;
	this->currentStep = 7;

	pinMode(pin2, OUTPUT);  
	pinMode(pin4, OUTPUT); 
}

void StepperMotor::setDirection(bool direction) {
	this->direction = direction;
}


void StepperMotor::motorOff() {
	digitalWrite(pin1, 0); 
	digitalWrite(pin3, 0);   
}

void StepperMotor::doStep(int steps) {
	int i;	

	if(direction) {
		//turn right
		for(i = 0; i < steps; i++) {
			//makeStep(i % NUM_STEPS);
			calculateNextStep();
			//makeStep(stepNumber % NUM_STEPS);	
			makeStep(currentStep);
			//Serial.println(getCurrentStep());    //DEBUGGING
			delay(millisecs_per_step); 
		}
	}

	else {
		//turn left
		for (i=steps; i > 0; i--){
			//makeStep(i % NUM_STEPS); 
			calculateNextStep();
			//makeStep(stepNumber % NUM_STEPS);
			makeStep(currentStep);
			//Serial.println(getCurrentStep());    //DEBUGGING
			delay(millisecs_per_step);
		}
	}
}

void StepperMotor::setSpeed(uint32_t rpm)
{
	//revsteps : gives steps per rev
	//revsteps x rpm = steps taken per min
	//steps take per microsec = (steps take per min)/ 60 x 1000
	//
	uint32_t stepsPerMin = rpm * stepsPerRev;
	millisecs_per_step   =(uint32_t) ( (60000)/(stepsPerMin));
}

int StepperMotor::getCurrentStep()
{
	return this->currentStep;
}



void StepperMotor::calculateNextStep() {
	if(direction) {
		//right
		currentStep++;
		if (currentStep == 8) {
			currentStep = 0;
		}
	}
	else {
		//left
		currentStep--;
		if (currentStep == -1) {
			currentStep = 7;
		}
	}
}
void StepperMotor::makeStep(int step) {	
	switch (step) {
	case 0:    
		step0();
		break;
	case 1:   
		step1();
		break;
	case 2:   
		step2();
		break;
	case 3:   
		step3();
		break;
	case 4:    
		step4();
		break;
	case 5:   
		step5();
		break;
	case 6:   
		step6();
		break;
	case 7:   
		step7();
		break;
	} 
}

void StepperMotor::step0() {
	digitalWrite(pin2,HIGH);  
	digitalWrite(pin1,HIGH);   
	digitalWrite(pin4,HIGH);      
	digitalWrite(pin3,LOW);   
}

void StepperMotor::step1() {
	digitalWrite(pin2,HIGH);  
	digitalWrite(pin1,HIGH);   
	digitalWrite(pin4,HIGH);      
	digitalWrite(pin3,HIGH);   
}

void StepperMotor::step2() {
	digitalWrite(pin2,HIGH);  
	digitalWrite(pin1,LOW);   
	digitalWrite(pin4,HIGH);      
	digitalWrite(pin3,HIGH);   
}

void StepperMotor::step3() {
	digitalWrite(pin2,LOW);  
	digitalWrite(pin1,HIGH);   
	digitalWrite(pin4,HIGH);      
	digitalWrite(pin3,HIGH); 
}

void StepperMotor::step4() {
	digitalWrite(pin2,LOW);  
	digitalWrite(pin1,HIGH);   
	digitalWrite(pin4,HIGH);      
	digitalWrite(pin3,LOW);  
}

void StepperMotor::step5() {
	digitalWrite(pin2,LOW);  
	digitalWrite(pin1,HIGH);  
	digitalWrite(pin4,LOW);      
	digitalWrite(pin3,HIGH);   
}

void StepperMotor::step6() {
	digitalWrite(pin2,LOW);  
	digitalWrite(pin1,LOW);   
	digitalWrite(pin4,LOW);      
	digitalWrite(pin3,HIGH);  
}

void StepperMotor::step7() {
	digitalWrite(pin2,HIGH);  
	digitalWrite(pin1,HIGH);   
	digitalWrite(pin4,LOW);      
	digitalWrite(pin3,HIGH);   
}

