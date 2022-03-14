/*singleLEDLibrary
* A library for non interupting lighting effects for single LED's
* Pim Ostendorf - 2017.11.24
*/

#pragma once
#include "Arduino.h"

class sllib
{
    public:
    //public variables and fucntions
        sllib(int pin, int brightness = 255);

        //breathing
        void breathSingle(int speed);

        //blink
        void blinkSingle(int speed);
        void blinkSingle(int timeHigh, int timeLow);
        void blinkRandomSingle(int minTime, int maxTime);
        
        //flicker
        void flickerSingle();
        void flickerSingle(int intMin, int intMax);
        void flickerSingle(int intMin, int intMax, int speed);
        
        //blink pattern
        void patternSingle(int pattern[], int lengtharray);
        
        //future update function
        void update();
        void setPatternSingle(int pattern[], int lengtharray);
        void setBreathSingle(int speed);
        void setFlickerSingle();
        void setBlinkSingle(int speed);
        void setRandomBlinkSingle(int minTime, int maxTime);
        void setOnSingle();
        void setOffSingle();

        // Helper
        void on();
        void off();
        
    private:
    //private variables and fucntion
        //gobal variables
        int _pin;
        int _brightness;
        unsigned long milOld;
        int runningFunction = 0;
        int speedp;
        int timep;
        //int pPatt[];
        int* arrP = 0;

        //variables for blinking
        bool ioBlink = false;
        int rndTemp = 0;

        //variables for pattern
        int counter = 0;

};