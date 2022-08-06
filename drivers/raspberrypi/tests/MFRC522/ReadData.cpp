/*
 * ReadData.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.
 *
 * This file is part of Robotics project.

 * Robotics is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * Robotics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with Robotics; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <CMFRC522.h>
#include <pigpio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>

using namespace std;

int main(int argc, char *argv[]) {
	unsigned char buffer[18];  //data transfer buffer (16+2 bytes data+CRC)
	unsigned char size = sizeof(buffer);

	uint8_t pageAddr = 0x06;  //In this example we will write/read 16 bytes (page 6,7,8 and 9).
	                            //Ultraligth mem = 16 pages. 4 bytes per page.
	                            //Pages 0 to 4 are for special functions.
	CMFRC522::StatusCode status; //variable to get card status

	gpioInitialise();
	CMFRC522 mfrc522;
    // Initializes PCD
    mfrc522.PCD_Init(22);
//    if (!mfrc522.PCD_PerformSelfTest()) {
//    	cout<<"Perform SelfTest failed\n";
//    }
    mfrc522.PCD_DumpVersionToSerial();
//    mfrc522.PCD_Init(22);
    while(1) {
           if (mfrc522.isNewCardPresent()==0) { // Detect whether a new card is present
        	   mfrc522.RC522_Anticoll(mfrc522._id);
        	   for (uint8_t i = 0; i < 8; i++) {
        	   	   std::cout<<std::hex<<int(mfrc522._id[i])<<" ";
        	   }
//        	   cout<<endl<<"Dump data"<<endl;
//        	   mfrc522.PICC_DumpMifareUltralightToSerial();
//        	   cout<<"End dump data"<<endl;
        	   // Select one of the cards
        	   status = mfrc522.PICC_Select(&mfrc522.uid);
        	   if ( status != CMFRC522::STATUS_OK) {
        		   cout<<"Could not read Card Serial with status code: <<"<<mfrc522.GetStatusCodeName(status)<<">>"<<endl;
        	       //continue;
        	   }
        	   cout<<"Reading data ... "<<endl;
        	    //data in 4 block is readed at once.
        	   size = sizeof(buffer);
        	   status = (CMFRC522::StatusCode) mfrc522.MIFARE_Read(pageAddr, buffer, &size);
        	   if (status != CMFRC522::STATUS_OK) {
        	     cout<<"MIFARE_Read() failed: ";
        	     cout<<mfrc522.GetStatusCodeName(status)<<endl;
        	     continue;
        	   }

        	    cout<<"Readed data: "<<endl;
        	    //Dump a byte array to Serial
        	    for (unsigned char i = 0; i < 16; i++) {
        	      cout<<buffer[i];
        	    }
           }
    }
}
