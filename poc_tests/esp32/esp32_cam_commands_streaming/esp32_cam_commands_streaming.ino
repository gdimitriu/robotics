/*
 * esp32_cam_commands
 * Copyright 2023 Gabriel Dimitriu
 *
 * This file is part of robotics project.

 * robotics is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * robotics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with robotics; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/


#include <WiFi.h>
#include "camera_streaming.h"

const char* ssid     = "xxxx";
const char* password = "xxxx";

WiFiServer server(4242);
WiFiClient client;

#define SERIAL_BUFFER 50
#define ABSOLUTE_MAX_POWER 65550
// for input data from wifi
boolean cleanupSerial;
bool isValidInput;
char inData[SERIAL_BUFFER]; // Allocate some space for the string
char buffer[SERIAL_BUFFER];
char inChar; // Where to store the character read
int indexReceive = 0; // indexReceive into array; where to store the character
int currentPower = 0;
int minPower = 0;
int maxPower = 0;
boolean hasConnection = false;

void setup()
{
    isValidInput = false;
    cleanupSerial = false;
    Serial.begin(115200);

    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    initCamera();
    
    server.begin();
    client = server.available();   // listen for incoming clients
    if ( client ) {
      hasConnection = true;
      Serial.println("New Client.");
    }
}

boolean isValidNumber( char *data, int size )
{
  if ( size == 0 ) return false;
   boolean isNum=false;
   if( !( data[0] == '+' || data[0] == '-' || isDigit(data[0]) ) ) return false;

   for( byte i=1; i<size; i++ )
   {
       if( !( isDigit(data[i]) || data[i] == '.' ) ) return false;
   }
   return true;
}

void makeCleanup() {
  Serial.print("MakeCleanup core=");Serial.println(xPortGetCoreID());
  Serial.flush();
  for ( indexReceive = 0; indexReceive < SERIAL_BUFFER; indexReceive++ ) {
    inData[indexReceive] = '\0';
  }
  indexReceive = 0;
  inChar ='\0';
  //client.flush();
}

void sendOK() {
  client.println("OK");
}

bool setMaxPowerCommand() {
  sendOK();
  //remove V from command
  for ( uint8_t i = 0 ; i < strlen(inData); i++ ) {
    inData[i]=inData[i+1];
  }
  if ( !isValidNumber(inData, indexReceive - 2) ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  if ( atol(inData) > ABSOLUTE_MAX_POWER || atol(inData) < 0 ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  maxPower = atol(inData);
  makeCleanup();
  isValidInput = true;
  return true;
}

bool setMinPowerCommand() {
  sendOK();
  //remove v from command
  for ( uint8_t i = 0 ; i < strlen(inData); i++ ) {
    inData[i]=inData[i+1];
  }
  if ( !isValidNumber(inData, indexReceive - 2) ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  if ( atol(inData) > maxPower || atol(inData) < 0 ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  minPower = atol(inData);
  makeCleanup();
  isValidInput = true;
  return true;
}

bool setCurrentPowerCommand() {
  sendOK();
  //remove c from command
  for ( uint8_t i = 0 ; i < strlen(inData); i++ ) {
    inData[i]=inData[i+1];
  }
  if ( !isValidNumber(inData, indexReceive - 2) ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  if ( atol(inData) > maxPower || atol(inData) < 0 ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  currentPower = atol(inData);
  makeCleanup();
  isValidInput = true;
  return true;
}

bool processPlatformUnsupportedCommand() {
  sendOK();
  makeCleanup();
  isValidInput = true;
  return true;
}

bool moveOrRotateUntilNextCommand() {
  //remove M from command
  for ( uint8_t i = 0 ; i < strlen(inData); i++ ) {
    inData[i]=inData[i+1];
  }
  inData[strlen(inData)] = '\0';
  int position;
  for ( uint8_t i = 0; i < strlen(inData); i++ ) {
    if ( inData[i] == ',' ) {
      position = i;
      break;
    }
  }
  char buf[10];
  for ( int i = 0; i < 10; i++ ) {
    buf[i] = '\0';
  }
  for ( int i = 0 ; i < position; i++ ) {
    buf[i] = inData[i];
  }
  int moveData = atoi(buf);
  for ( int i = 0; i < 10; i++ ) {
    buf[i] = '\0';
  }
  int idx = 0;
  for ( int i = position + 1; i < strlen(inData); i++ ) {
    buf[idx] = inData[i];
    idx++;
  }
  int rotateData = atoi(buf);
  Serial.print("move=");Serial.print(moveData);Serial.print(" rotate=");Serial.println(rotateData);
  makeCleanup();
  isValidInput = true;
  return true;
}

bool moveOrRoatateWithDistanceCommand() {
  //remove m from command
  for ( uint8_t i = 0 ; i < strlen(inData); i++ ) {
    inData[i]=inData[i+1];
  }
  inData[strlen(inData)] = '\0';
  int position;
  for ( uint8_t i = 0; i < strlen(inData); i++ ) {
    if ( inData[i] == ',' ) {
      position = i;
      break;
    }
  }
  char buf[10];
  for ( int i = 0; i < 10; i++ ) {
    buf[i] = '\0';
  }
  for ( int i = 0 ; i < position; i++ ) {
    buf[i] = inData[i];
  }
  int moveData = atoi(buf);
  for ( int i = 0; i < 10; i++ ) {
    buf[i] = '\0';
  }
  int idx = 0;
  for ( int i = position + 1; i < strlen(inData); i++ ) {
    buf[idx] = inData[i];
    idx++;
  }
  int rotateData = atoi(buf);
  if ( moveData == 0 && rotateData == 0 ) {
    Serial.println("coasting");
  } else if ( rotateData == 0 ) {
    Serial.print("move=");Serial.println(rotateData);
  } else {
    Serial.print("rotate=");Serial.println(rotateData);
  }
  makeCleanup();
  isValidInput = true;
  return true;
}

bool makeMove() {
  for ( int i = 0; i < SERIAL_BUFFER; i++ ) {
    buffer[i] = '\0';
  }
  if ( indexReceive > 0 ) {
     inData[indexReceive-1] = '\0';
  }
  if ( strlen(inData) == 1 ) {
    if ( inData[0] == 'I' ) {
      sprintf(buffer,"unsupported\r\n");
      client.print(buffer);
    } else if ( inData[0] == 'V' ) {      
      sprintf(buffer,"%d\r\n",maxPower);
      client.print(buffer);
    } else if ( inData[0] == 'v' ) {
      sprintf(buffer,"%d\r\n",minPower);
      client.print(buffer);
    } else if ( inData[0] == 'c' ) {
      sprintf(buffer,"%d\r\n",currentPower);
      client.print(buffer);
    }  else if ( inData[0] == 'd' ) {
      sprintf(buffer,"%d\r\n",0);
      client.print(buffer);
    } else if ( inData[0] == 's' ) {
      sprintf(buffer,"%d\r\n",0);
      client.print(buffer);
    } else if ( inData[0] == 'b' ) {  //break all engines
      Serial.println("Break all engines");
    } else if ( inData[0] == 'C' ) { //get encoder values
      sprintf(buffer,"%ld:%ld:%ld:%ld\r\n",200,200,200,200);
      client.print(buffer);
    } else if (inData[0] == 'R') { //reset encoders
      Serial.println("Reset encoders");
      sprintf(buffer,"%d\r\n",0);
      client.print(buffer);
    } else {
      sprintf(buffer,"%d\r\n",0);
      client.print(buffer);
      makeCleanup();
      isValidInput = false;
      return false;
    }
  } else {
      if ( inData[0] == 'V' ) {
        return setMaxPowerCommand();
      } else if ( inData[0] == 'v' ) {
        return setMinPowerCommand();
      } else if ( inData[0] == 'c' ) {
        return  setCurrentPowerCommand();
      } else if ( inData[0] == 'd' ) {
        //this does not support low power distance
        return processPlatformUnsupportedCommand();
      } else if ( inData[0] == 's' ) {
        //this does not support stop distance
        return processPlatformUnsupportedCommand();
      } else if ( inData[0] == 'M' ) {
        return moveOrRotateUntilNextCommand();
      } else if ( inData[0] == 'm' ) { //move or rotate with distance
        return moveOrRoatateWithDistanceCommand();        
      } else if ( inData[0] == 'r' ) {
        if ( inData[1] == 'u' ) {
          Serial.println("Rise rfid");
          sprintf(buffer,"OK\r\n");
          client.print(buffer);
        } else if ( inData[1] == 'l' ) {
          Serial.println("Lower Rfid");
          sprintf(buffer,"OK\r\n");
          client.print(buffer);
        } else if ( inData[1] == 'p' ) {
          sprintf(buffer,"%d\r\n", 10);
          client.print(buffer);
        } else {
          sprintf(buffer,"unsupported\r\n");
          client.print(buffer);
          makeCleanup();
          isValidInput = false;
          return false;
        }
      } else {
        sprintf(buffer,"%d\r\n",0);
        client.print(buffer);
        makeCleanup();
        isValidInput = false;
        return false;
      }
    }
    isValidInput = true;
    makeCleanup();
    return true;
}

void receiveCommands() {
  while ( client.connected() ) {
    while ( client.available() > 0 ) // Don't read unless there you know there is data
    {
      Serial.print("receive core=");Serial.println(xPortGetCoreID());
      Serial.flush();
      if ( indexReceive < (SERIAL_BUFFER - 1) ) // One less than the size of the array
      {
          inChar = client.read(); // Read a character
          if ( inChar=='\r' || inChar=='\n' || inChar =='\0' || inChar < 35 || inChar > 122 ) {
            continue;
          }
          //commands start with a letter capital or small
          if ( indexReceive == 0 && !( ( inChar >64 && inChar <91 ) || ( inChar > 96 && inChar<123 ) ) ) {
            continue;
          }    
          inData[indexReceive++] = inChar; // Store it
          inData[indexReceive] = '\0'; // Null terminate the string
          if ( inChar == '#' )
            break;
      } else {
          makeCleanup();
          cleanupSerial = true;
      }
    }
    if ( indexReceive >= 1 ) {
      if ( inData[indexReceive - 1] == '#' ) {
        Serial.print("indexReceive = ");Serial.println(indexReceive);
        Serial.print("Make move = ");Serial.println(inData);
        makeMove();
      } else if ( cleanupSerial ) {
        Serial.print("indexReceive = ");Serial.println(indexReceive);
        Serial.print("Make cleanup extern = ");Serial.println(inData);
        makeCleanup();
        cleanupSerial = false;
      } else {
        delay(10);
      }
    }
  }
 if ( !client.connected() ) {
  Serial.println("Client disconnected!");Serial.flush();
  client.flush();
  client.stop();
  hasConnection = false;
 }
}
void loop()
{
  if ( !hasConnection ) {
    client = server.available();   // listen for incoming clients
    if (client) {                             // if you get a client,
        Serial.println("New Client.");           // print a message out the serial port
        hasConnection = true;
        receiveCommands();
    }
  }
  
}
