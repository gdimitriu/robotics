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
#include <ObserverList.h>
ObserverList::ObserverList()
{
	this->head = nullptr;
	this->tail = nullptr;
}
void ObserverList::addObserver(Observer *obs)
{
	Node *tmp = new Node(obs);
	if (head == tail && head == nullptr) {
		head = tail = tmp;
	}
	else {
		Node *iter = head;
		while (iter != nullptr) {
			if (iter->getObserver() == obs) {
				return;
			}
			iter = iter->getNextNode();
		}
		tail->setNextNode(tmp);
		tail = tmp;
	}
}

void ObserverList::updateObservers()
{
	Node *tmp = head;
	while (tmp != nullptr) {
		tmp->getObserver()->update();
		tmp = tmp->getNextNode();
	}
}