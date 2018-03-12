/*
Author: Zach Sotak
Date: 11/10/2017

queue.h

*/

#ifndef DVsimulation_queue
#define DVsimulation_queue

#include <stdio.h>
#include "node.h"

using namespace std;

class queue
{
public:
    queue();
    ~queue();
    void insert(double time, char type, node& src, node& dest, string *dvDest, string *dvCost, int dvSize);
    void del();
    void printQueue();
    void printDVPackets();
    void processEvent();
    double getCurrTime();
    char getCurrentType();
    string* getCurrentDVDest();
    string* getCurrentDVCost();
    int getCurrentDVSize();
    string getCurrentSrcName();
    string getCurrentDestName();
    bool isEmpty();

private:
    struct event
    {
        double time;
        char type;
        node *src;
        node *dest;
        string *dvDest;
        string *dvCost;
        int dvSize;
        event *head;
    };

    event *front;
};


#endif /* defined(DVsimulation_queue) */

