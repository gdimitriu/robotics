#include <EnableInterrupt.h>

#define FRONT_ENGINE_BUTTON 3
#define BACK_ENGINE_BUTTON 4

#define LEFT_ENGINE_BUTTON 6
#define RIGHT_ENGINE_BUTTON 7

#define FRONT_ENGINE_IDX 0
#define RIGHT_ENGINE_IDX 1
#define BACK_ENGINE_IDX 2
#define LEFT_ENGINE_IDX 3
int enginePower[4];

bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character

boolean isValidNumber(char *data, int size)
{
  if (size == 0 ) return false;
   boolean isNum=false;
   if(!(data[0] == '+' || data[0] == '-' || isDigit(data[0]))) return false;

   for(byte i=1;i<size;i++)
   {
       if(!(isDigit(data[i]) || data[i] == '.')) return false;
   }
   return true;
}

void powerToAllEngines(int value) {
  Serial.print(" Power all engines to value:");
  Serial.println(value);
  enginePower[FRONT_ENGINE_IDX]=value;
  enginePower[RIGHT_ENGINE_IDX]=value;
  enginePower[BACK_ENGINE_IDX]=value;
  enginePower[LEFT_ENGINE_IDX]=value;
}

void powerToFrontEngine(int value) {
  Serial.print("Power to front engine from : ");
  Serial.print(enginePower[FRONT_ENGINE_IDX]);
  Serial.print(" to : ");
  Serial.println(value);
  enginePower[FRONT_ENGINE_IDX] = value;
}

void powerToRightEngine(int value) {
  Serial.print("Power to right engine from : ");
  Serial.print(enginePower[RIGHT_ENGINE_IDX]);
  Serial.print(" to : ");
  Serial.println(value);
  enginePower[RIGHT_ENGINE_IDX] = value;
}

void powerToBackEngine(int value) {
  Serial.print("Power to back engine from : ");
  Serial.print(enginePower[BACK_ENGINE_IDX]);
  Serial.print(" to : ");
  Serial.println(value);
  enginePower[BACK_ENGINE_IDX] = value;
}

void powerToLeftEngine(int value) {
  Serial.print("Power to left engine from : ");
  Serial.print(enginePower[LEFT_ENGINE_IDX]);
  Serial.print(" to : ");
  Serial.println(value);
  enginePower[LEFT_ENGINE_IDX] = value;
}

void setup() {
  Serial.begin(9600);
  enginePower[FRONT_ENGINE_IDX]=0;
  enginePower[RIGHT_ENGINE_IDX]=0;
  enginePower[BACK_ENGINE_IDX]=0;
  enginePower[LEFT_ENGINE_IDX]=0;
}

void printAllEnginesLevel() {
  Serial.print("Front engine power level : "); Serial.println(enginePower[FRONT_ENGINE_IDX]);
  Serial.print("Right engine power level : "); Serial.println(enginePower[RIGHT_ENGINE_IDX]);
  Serial.print("Back engine power level : "); Serial.println(enginePower[BACK_ENGINE_IDX]);
  Serial.print("Left engine power level : "); Serial.println(enginePower[LEFT_ENGINE_IDX]);
}

void loop() {
  Serial.println( "------------------------------------------------------------------" );
  Serial.println( "Engines transmitter on quadcopter");
  Serial.println( "------------------------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "s# Stop" );
  Serial.println( "axx# Power to all engines" );
  Serial.println( "fxx# Power to front engine" );
  Serial.println( "rxx# Power to right engine" );
  Serial.println( "bxx# Power to back engine" );
  Serial.println( "lxx# Power to left engine" );
  Serial.println( "p# print all engines power level" );
  Serial.println( "-----------------------------" );
   do {
    for (index = 0; index < 20; index++)
    {
       inData[index] = '\0';
    }
    inChar = '0';
    index = 0;
    while(inChar != '#') {
      while( !Serial.available() )
        ; // LOOP...
      while(Serial.available() > 0) // Don't read unless
                                                 // there you know there is data
      {
          if(index < 19) // One less than the size of the array
          {
              inChar = Serial.read(); // Read a character
              inData[index] = inChar; // Store it
              index++; // Increment where to write next
              inData[index] = '\0'; // Null terminate the string
          }
      }
    }
    if (index > 0) {
      inData[index-1] = '\0';
    }
    if (strcmp(inData,"s") == 0) {
      Serial.println("stop");
      powerToAllEngines(0);
      isValidInput = true;
    } else if (strcmp(inData,"p") == 0) {
      printAllEnginesLevel();
      isValidInput = true;
    } else if (strlen(inData) > 1) {
      if (inData[0] == 'a') {
        //remove a from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        powerToAllEngines(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'f') {
        //remove f from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        powerToFrontEngine(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'r') {
        //remove r from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        powerToRightEngine(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'b') {
        //remove b from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        powerToBackEngine(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'l') {
        //remove l from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        powerToLeftEngine(atoi(inData));
        isValidInput = true;
      } else {
        isValidInput = false;
      }
    } else {
        isValidInput = false;
    }
  } while( isValidInput == true );
}
