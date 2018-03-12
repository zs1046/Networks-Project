/*
Author: Zach Sotak
Date: 11/10/2017

node.cpp

*/


#include "node.h"
#include <iostream>
#include <cstring>
#include <string>

using namespace std;

node::node(char& src, char& dest, char& cost) : destination(), cost(), nextHop(), RTSize(0), maxRTSize(0),
                                                                neighborName(), neighborCost(),  numOfNeighbors(0) {

    strcpy(srcRT, &src);
    strcpy(destRT, &dest);
    strcpy(costRT, &cost);

    nodeName = srcRT;
    updateRT(*destRT, *costRT, *destRT);
    updateNeighbor(*destRT, *costRT);
}

node::~node() {

}

void node::updateRT(char& newDest, char& newCost, char& newNextHop)
{
    //no allocation needed
    if (RTSize < maxRTSize)
    {
        destination[RTSize] = &newDest;
        cost[RTSize] = &newCost;
        nextHop[RTSize] = &newNextHop;
        ++RTSize;
    }
    //allocation needed
    else
    {
        allocateRoutingTable();

        destination[RTSize] = &newDest;
        cost[RTSize] = &newCost;
        nextHop[RTSize] = &newNextHop;
        ++RTSize;
    }
}

void node::printRT() const
{
    cout << "\t\t" << getNodeName() << "'s Routing Table\n";
    cout << "Destination\t\tCost\t\tNextHop\n";

    for (int i = 0; i < RTSize; ++i)
    {
        cout << destination[i] << "\t\t\t" << cost[i] << "\t\t" << nextHop[i] << "\n";
    }
    cout << "\n";
}

short node::getRTSize() const
{
    return RTSize;
}

string node::getNodeName() const
{
    return nodeName;
}

string node::getRTDest(int item)
{
    return destination[item];
}

string node::getRTNextHop(int item)
{
    return nextHop[item];
}

void node::updateNeighbor(char& name, char& cost)
{
    string sName = &name;
    string sCost = &cost;


    neighborName.push_back(sName);
    neighborCost.push_back(sCost);

    ++numOfNeighbors;
}

void node::printNeighbors() const
{
    cout << "\t\t" << getNodeName() << "'s Neighbors\n";
    cout << "Name\t\tCost\n";

    for (int i = 0; i < numOfNeighbors; ++i)
    {
        cout << neighborName[i] << "\t\t" << neighborCost[i] << "\n";
    }
    cout << "\n\n";
}

short node::getNumOfNeighbors()
{
    return numOfNeighbors;
}

string node::getNeighborName(int item)
{
    return neighborName[item];
}

string node::getNeighborCost(int item)
{
    return neighborCost[item];
}



string* node::getDVDest()
{
    string* temp = new string[RTSize];

    for (int i = 0; i < RTSize; ++i) {
        temp[i] = destination[i];
    }

    return temp;
}

string* node::getDVCost()
{
    string* temp = new string[RTSize];

    for (int i = 0; i < RTSize; ++i) {
        temp[i] = cost[i];
    }

    return temp;
}

bool node::updateRTDV(string* destDV, string* costDV, string nextHopDV, int dvSize, string nCost)
{
    bool newEntry = true;
    bool triggeredUpdate = false;

    for (int i = 0; i < dvSize; ++i) {
        newEntry = true;
        for (int j = 0; j < RTSize; ++j) {
            //if DV destination entry is the destination node, skip entry
            if (destDV[i] == getNodeName())
            {
                newEntry = false;
                break;
            }
            if (destination[j] == destDV[i])
            {


                //convert costs to ints so they can determine if a shorter path was found
                int costNum = stoi(cost[j]);
                int costDVNum = stoi(costDV[i]);
                int costDVSrcNum = stoi(nCost);
                costDVNum += costDVSrcNum;
                if (costNum > costDVNum)
                {
                    cost[j] = to_string(costDVNum);
                    nextHop[j] = nextHopDV;
                    newEntry = false;
                    triggeredUpdate = true;
                    break;
                }
                else
                {
                    newEntry = false;
                    break;
                }
            }
        }

        if (newEntry)
        {
            triggeredUpdate = true;
            int costDVN = stoi(costDV[i]);
            int costTotal = stoi(nCost);
            costTotal += costDVN;

            //no allocation needed
            if (RTSize < maxRTSize)
            {
                destination[RTSize] = destDV[i];
                cost[RTSize] = to_string(costTotal);
                nextHop[RTSize] = nextHopDV;
                ++RTSize;
            }
            //allocation needed
            else
            {
                allocateRoutingTable();

                destination[RTSize] = destDV[i];
                cost[RTSize] = to_string(costTotal);
                nextHop[RTSize] = nextHopDV;
                ++RTSize;
            }
        }
    }
    return triggeredUpdate;
}


void node::allocateRoutingTable()
{
    maxRTSize += 30;

    string *tempDest = new string[maxRTSize];
    string *tempCost = new string[maxRTSize];
    string *tempNextHop = new string[maxRTSize];

    for (int i = 0; i < RTSize; ++i) {
        tempDest[i] = destination[i];
        tempCost[i] = cost[i];
        tempNextHop[i] = nextHop[i];
    }

    delete [] destination;
    delete [] cost;
    delete [] nextHop;

    destination = tempDest;
    cost = tempCost;
    nextHop = tempNextHop;
}



