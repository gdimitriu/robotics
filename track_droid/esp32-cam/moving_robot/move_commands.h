#ifndef __MOVE_COMMANDS_H__
#define __MOVE_COMMANDS_H__

#include "configuration.h"

bool moveOrRoatateWithDistanceCommand(char *realData);

bool isValidNumber( char *data, int size );

bool setMinPowerCommand(char *realData);

bool setMaxPowerCommand(char *realData);

bool setCurrentPowerCommand(char *realData);

bool moveOrRotateUntilNextCommand(char *realData);

void setupEngines();

void go( int speedLeft, int speedRight );
#endif
