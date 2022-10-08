/*
 * Copyright 2018 Gabriel Dimitriu

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

#pragma once
#ifndef CommanExecutionList_h
#define CommanExecutionList_h
#include "CommandNode.h"

template <typename OBJECT, typename FUNCS, typename FUNCG>
class CommandExecutionList
{
public:
	CommandExecutionList();
	~CommandExecutionList();
	void add(string *name, OBJECT *obj, FUNCS setter, FUNCG getter);
	bool setValue(string *name, int value);
	int getValue(string *name);
	bool isAvailable(string *name);
	void printAll();
	char getArgumentType();
private:
	CommandNode<OBJECT, FUNCS, FUNCG> * head = nullptr;
	CommandNode<OBJECT, FUNCS, FUNCG> * tail = nullptr;
	OBJECT *objType = nullptr;
};

template <typename OBJECT, typename FUNCS, typename FUNCG>
CommandExecutionList<OBJECT, FUNCS, FUNCG>::CommandExecutionList()
{
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
CommandExecutionList<OBJECT, FUNCS, FUNCG>::~CommandExecutionList()
{

}

template <typename OBJECT, typename FUNCS, typename FUNCG>
char CommandExecutionList<OBJECT, FUNCS, FUNCG>::getArgumentType()
{
	if (objType != nullptr) return objType->getArgumentType();
	return 'l';
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
void CommandExecutionList<OBJECT, FUNCS, FUNCG>::add(string *name, OBJECT *obj, FUNCS setter, FUNCG getter)
{
	if (objType == nullptr) objType = obj;
	CommandNode<OBJECT, FUNCS, FUNCG> *temp = new CommandNode<OBJECT, FUNCS, FUNCG>(name, obj, setter, getter);
	if (head == tail && head == nullptr) {
		head = temp;
		tail = head;
		return;
	}
	if (tail != nullptr) {
		tail->setNext(temp);
		tail = temp;
	}
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
bool CommandExecutionList<OBJECT, FUNCS, FUNCG>::setValue(string *name, int value)
{
	CommandNode<OBJECT, FUNCS, FUNCG> *temp;
	temp = head;
	while (temp->getNext() != nullptr) {
		if (*(temp->getName()) == *name) {
			temp->executeSetter(value);
			return true;
		}
		temp = temp->getNext();
	}
	return false;
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
int CommandExecutionList<OBJECT, FUNCS, FUNCG>::getValue(string *name)
{
	CommandNode<OBJECT, FUNCS, FUNCG> *temp;
	temp = head;
	while (temp != nullptr) {
		if ((*temp->getName()) == *name) {
			return temp->executeGetter();
		}
		temp = temp->getNext();
	}
	return -1;
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
bool CommandExecutionList<OBJECT, FUNCS, FUNCG>::isAvailable(string *name)
{
	CommandNode<OBJECT, FUNCS, FUNCG> *temp;
	
	temp = head;
	while (temp != nullptr) {
		if (*(temp->getName()) == *name) {
			return true;
		}
		temp = temp->getNext();
	}
	return false;
}
template <typename OBJECT, typename FUNCS, typename FUNCG>
void CommandExecutionList<OBJECT, FUNCS, FUNCG>::printAll()
{
	CommandNode<OBJECT, FUNCS, FUNCG> *temp;
	temp = head;
	while (temp != nullptr) {
		Serial.println(*temp->getName());
		delay(100);
		temp = temp->getNext();
	}
}
#endif
