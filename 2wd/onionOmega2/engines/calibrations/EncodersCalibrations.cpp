/*
Driver calibrations of encoders.
Copyright (C) 2020 Gabriel Dimitriu
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */

#include<iostream>
#include<string>
#include<unistd.h>

#include "../EnginesWithEncoderMX1508SX1509.h"

int main(int args, char* argv[]){
	EnginesWithEncoder * encoders;
	std::cout<<"Calibrations of encoders on onion Omega"<<std::endl;
	std::cout<<"Print Encoders values (p)"<<std::endl;
	std::cout<<"Reset Encoders values (r)"<<std::endl;
	std::cout<<"Quit (q)"<<std::endl;
	encoders = new EnginesWithEncoderMX1508SX1509(3, 1452, 1468, 9, 10, 3, 2);
	std::string inputParameter;
	std::getline(std::cin, inputParameter);
	encoders->startIsr();
	while(1) {
		if (!inputParameter.empty()) {
			switch (inputParameter.front()){
				case 'q':
					encoders->stopIsr();
					delete encoders;
					return 0;
				case 'p' :
					std::cout<<"Left Encoder="<<encoders->getLeftCounter();
					std::cout<<" Right Encoder="<<encoders->getRightCounter()<<std::endl;
					break;
				case 'r' :
					std::cout<<"Reset Counters with actual values"<<std::endl;
					std::cout<<"Left Encoder="<<encoders->getLeftCounter();
					std::cout<<" Right Encoder="<<encoders->getRightCounter()<<std::endl;
					encoders->resetCounters();
					break;
			}
		}
		std::getline(std::cin, inputParameter);
	}
	return 0;
}
