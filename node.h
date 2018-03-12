
/*
Author: Zach Sotak
Date: 11/10/2017

node.h

*/

#ifndef DVsimulation_node
#define DVsimulation_node

#include <string>
#include <vector>
#include <stdio.h>

using namespace std;

class node
{
public:
    ~node();
    node(char& src, char& dest, char& cost);
    void updateRT(char& dest, char& cost, char& nextHop);
    void printRT() const;
    short getRTSize() const;
    string getNodeName() const;
    string getRTDest(int item);
    string getRTNextHop(int item);
    void updateNeighbor(char& name, char& cost);
    void printNeighbors() const;

    //DV Packet implemented as parallel array
    //these give new dest/cost arrays so that their data isn't overwritten by an event
    string* getDVDest();
    string* getDVCost();
    bool updateRTDV(string* destDV, string* costDV, string nextHopDV, int dvSize, string nCost);


    //Pass neighbor information
    short getNumOfNeighbors();
    string getNeighborName(int item);
    string getNeighborCost(int item);

private:
    //Routing table variables/functions
    string *destination;
    string *cost;
    string *nextHop;
    short RTSize;
    short maxRTSize;
    void allocateRoutingTable();
    //node Attributes
    string nodeName;


    // Neighbor information
    vector<string> neighborName;
    vector<string> neighborCost;
    short numOfNeighbors;

    //Update Routing Table temps
    char srcRT[7];
    char destRT[7];
    char costRT[7];

};

#endif /* defined(DVsimulation_node) */

