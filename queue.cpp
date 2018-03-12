/*
Author: Zach Sotak
Date: 11/10/2017

queue.cpp

*/


#include "queue.h"
#include <iostream>

queue::queue()
{
    front = NULL;
}

queue::~queue()
{

}

void queue::insert(double time, char type, node& src, node& dest, string *dvDest, string *dvCost, int dvSize)
{
    //create new event
    event *temp, *q;
    temp = new event;
    temp->time = time;
    temp->type = type;
    temp->src = &src;
    temp->dest = &dest;
    temp->dvDest = dvDest;
    temp->dvCost = dvCost;
    temp->dvSize = dvSize;
    //if it's the first event/most prioritized event...
    if (front == NULL || time < front->time)
    {
        temp->head = front;
        front = temp;
    }
    else
    {
        //scan to find the proper place in the priority queue
        q = front;
        while (q->head != NULL && q->head->time <= time)
            q = q->head;
        temp->head = q->head;
        q->head = temp;
    }
}

void queue::del()
{
    event *temp;
    if (front == NULL)
        cout << "Cannot delete... Queue empty\n";
    else
    {
        temp = front;
        front = front->head;
        free(temp);
    }
}

void queue::printQueue()
{
    int eventNumber = 0;
    event *ptr;
    ptr = front;
    if (front == NULL)
        cout << "Event Queue is empty...\n";
    else
    {
        cout << "\t\t\t*** Event Queue ***\n";
        cout << "Time\tType\tSrcnode\t\tDestnode\n";
        while (ptr != NULL) {
            cout << ptr->time << "\t" << ptr->type << "\t" << "(" << ptr->src->getNodeName() << ")" << ptr->src << "\t"
            << "(" << ptr->dest->getNodeName() << ")" << ptr->dest << "\t\tEvent " << eventNumber << "\n";
            ++eventNumber;
            ptr = ptr->head;
        }
    }
}

void queue::printDVPackets()
{
    short eventNumber = 0;
    event *ptr;
    ptr = front;
    if (front == NULL)
        cout << "Event Queue is empty...\n";
    else
    {
        cout << "\t\t\t*** DV Packets ***\n";
        cout << "DV src\t\tDest\t\tCost\n";
        while (ptr != NULL) {
            cout << ptr->src->getNodeName() << "\n";
            for (int i = 0; i < ptr->src->getRTSize(); ++i) {
                cout << "\t\t" << ptr->dvDest[i] << "\t\t" << ptr->dvCost[i] << "\t" << "Event #" << eventNumber << "\n";
            }
            ++eventNumber;
            ptr = ptr->head;
        }
    }
}

double queue::getCurrTime()
{
    return front->time;
}

char queue::getCurrentType()
{
    return front->type;
}

string* queue::getCurrentDVDest()
{
    return front->dvDest;
}

string* queue::getCurrentDVCost()
{
    return front->dvCost;
}

int queue::getCurrentDVSize()
{
    return front->dvSize;
}

string queue::getCurrentSrcName()
{
    return front->src->getNodeName();
}

string queue::getCurrentDestName()
{
    return front->dest->getNodeName();
}

bool queue::isEmpty()
{
    if (front == NULL)
        return true;
    else
        return false;
}

