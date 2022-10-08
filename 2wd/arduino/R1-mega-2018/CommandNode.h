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
#ifndef CommandNode_h
#define CommandNode_h
#define NAVIGATION nav
#ifdef NAVIGATION
	#include <Arduino.h>
	typedef String string;
#else
	typedef std::string string;
#endif
template <typename OBJECT, typename FUNCS, typename FUNCG>
class CommandNode
{
public:
	CommandNode(string *name, OBJECT *obj, FUNCS setter, FUNCG getter);
	string *getName();
	int executeGetter();
	void executeSetter(int value);
	~CommandNode();
private:
	CommandNode<OBJECT, FUNCS, FUNCG> *next;
	OBJECT *obj;
	FUNCS set;
	FUNCG get;
	string *name;
public:
	void setNext(CommandNode<OBJECT, FUNCS, FUNCG> *next);
	CommandNode<OBJECT, FUNCS, FUNCG> *getNext();
};

template <typename OBJECT, typename FUNCS, typename FUNCG>
CommandNode<OBJECT, FUNCS, FUNCG>::CommandNode(string *name, OBJECT *obj, FUNCS setter, FUNCG getter)
{
	this->name = name;
	this->obj = obj;
	this->set = setter;
	this->get = getter;
	this->next = nullptr;
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
CommandNode<OBJECT, FUNCS, FUNCG>::~CommandNode()
{
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
void CommandNode<OBJECT, FUNCS, FUNCG>::setNext(CommandNode<OBJECT, FUNCS, FUNCG> *next)
{
	this->next = next;
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
CommandNode<OBJECT, FUNCS, FUNCG> *CommandNode<OBJECT, FUNCS, FUNCG>::getNext()
{
	return this->next;
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
int CommandNode<OBJECT, FUNCS, FUNCG>::executeGetter()
{
	return (this->obj->*get)();
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
void CommandNode<OBJECT, FUNCS, FUNCG>::executeSetter(int value)
{
	(this->obj->*set)(value);
}

template <typename OBJECT, typename FUNCS, typename FUNCG>
string  *CommandNode<OBJECT, FUNCS, FUNCG>::getName()
{
	return this->name;
}

#endif