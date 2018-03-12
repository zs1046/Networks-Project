/*
Author: Zach Sotak
Date: 11/10/2017

main.cpp

*/

#include <iostream>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <algorithm>
#include "node.h"
#include "queue.h"

using namespace std;


//Functions
void commandLineInterface(vector<node>&, int argc, const char * argv[], int& numOfnodes, int& maxNodes, string *& nodeNames, double& time, int& simDataScenario);
void printnodeNamesArray(const string * rNames, const int numnodes);
void printRoutingTables(const vector<node>&);
void printAllNeighbors(const vector<node>&);
void allocatenodeNames(string *& rNames, const int numOfnodes, int& maxNodes);
int nameToIndex(const string name, vector<node>&, const int numnodes);
//Events
void initializeEventQueue(vector<node>&, const int numOfnodes, queue&);
void periodicUpdate(vector<node>&, int &numOfnodes, queue&, int currTime);
char getNextEvent(queue&);
void sendDVPacket(vector<node>&, int& numOfnodes, queue &, double currTime);
void processDVPacket(vector<node>&, int numOfnodes, queue &, double& currTime);
void forwardDataPacket(vector<node>& n, int numOfnodes, queue & q, double currTime);

int main(int argc, const char * argv[]) {

    queue queue;
    vector<node> nodeVector;

    int numOfnodes = 0;
    int maxNodes = 0;
    string *nodeNames = NULL;
    string lastNode;
    string lastConvergingNode;
    double simulationTime = 0;
    double currTime = 0;
    int periodicSeconds = 1;
    bool networkConvergence = false;
    double convergenceTime = 0;

    int routingMessagesCount = 0;
    int finalRoutingMessagesCount = 0;
    int simDataScenario = 0;


    allocatenodeNames(nodeNames, numOfnodes, maxNodes);
    commandLineInterface(nodeVector, argc, argv, numOfnodes, maxNodes, nodeNames, simulationTime, simDataScenario);
    initializeEventQueue(nodeVector, numOfnodes, queue);

    //fill priority queue with events
    while (currTime <= simulationTime)
    {
        if (queue.isEmpty())
        {
            if (networkConvergence == false)
            {
                //Place Data Packet forwarding events after convergence (so convergence can be detected correctly)
                int s = nameToIndex("0", nodeVector, numOfnodes);
                if (simDataScenario == 1 && simulationTime > 10)
                {
                    int d = nameToIndex("3", nodeVector, numOfnodes);
                    queue.insert(10, 'f', nodeVector[s], nodeVector[d], NULL, NULL, 0);
                }
                else if (simDataScenario == 2 && simulationTime > 20)
                {
                    int d = nameToIndex("7", nodeVector, numOfnodes);
                    queue.insert(20, 'f', nodeVector[s], nodeVector[d], NULL, NULL, 0);
                }
                else if (simDataScenario == 3 && simulationTime > 30)
                {
                    int d = nameToIndex("23", nodeVector, numOfnodes);
                    queue.insert(30, 'f', nodeVector[s], nodeVector[d], NULL, NULL, 0);
                }

                convergenceTime = currTime;
                lastConvergingNode = lastNode;
                finalRoutingMessagesCount = routingMessagesCount;
                networkConvergence = true;
            }

            currTime = periodicSeconds;
        }
        else
        {
            //Update current time
            currTime = queue.getCurrTime();
        }

        //periodic update
        if (currTime >= periodicSeconds)
        {
            periodicUpdate(nodeVector, numOfnodes, queue, periodicSeconds);
            ++periodicSeconds;
        }


        /*
         *  Begin processing events
         *  s = send DV Packet
         *  f = forward DV Packet
         *  p = process DV Packet
         */

        switch (queue.getCurrentType()) {
            case 's': sendDVPacket(nodeVector, numOfnodes, queue, currTime);
                      ++routingMessagesCount;
                      break;
            case 'p': processDVPacket(nodeVector, numOfnodes, queue, currTime);
                      ++routingMessagesCount;
                      break;
            case 'f': forwardDataPacket(nodeVector, numOfnodes, queue, currTime);
                      break;
            default: cout << "Event Type '" << queue.getCurrentType() << "' not recognized...\n";
                     cout << "Terminating Program...\n";
                     exit(EXIT_FAILURE);
        }
        lastNode = queue.getCurrentDestName();
        queue.del();
    }

    printRoutingTables(nodeVector);

    cout << "Rounds until convergence " << currTime << endl;
    cout << "Last Node to converge was " << lastConvergingNode << endl;
    cout << "Total DV messages sent " << finalRoutingMessagesCount << endl;

    return 0;
}

void commandLineInterface(vector<node>& nodeVector, int argc, const char * argv[], int& numOfnodes, int& maxNodes, string *& nodeNames, double& time, int& simDataScenario)
{
    int maxLineLength = 100;
    char topologyLine[maxLineLength];
    char* src;
    char* dest;
    char* cost;
    bool newnode = true;
    bool newDestnode = true;

    //checking proper command line entries...
    if (argc == 3) {
        //Set simulation time
        time = atof(argv[2]);
        //Open topology file
        ifstream in_file;
        in_file.open(argv[1]);

        if (in_file.is_open()) {

            //set data packet forwarding scenario
            if (!strcmp(argv[1], "topology1.txt"))
            {
                simDataScenario = 1;
            }
            else if (!strcmp(argv[1], "topology2.txt"))
            {
                simDataScenario = 2;
            }
            else if (!strcmp(argv[1], "topology3.txt"))
            {
                simDataScenario = 3;
            }

            //scan file for number of node objects to make
            in_file.getline(topologyLine, maxLineLength);
            while (!in_file.eof()) {

                //Parse line into string tokens
                src = strtok(topologyLine, "\t");
                dest = strtok(NULL, "\t");
                cost = strtok(NULL, "\t");

                newnode = true;
                newDestnode = true;
                for (int i = 0; i < numOfnodes; ++i)
                {
                    if (src == nodeNames[i]) {
                        //If the source number is a nodeName...
                        //Give to object of correct name
                        nodeVector[i].updateRT(*dest, *cost, *dest);
                        nodeVector[i].updateNeighbor(*dest, *cost);
                        newnode = false;
                    }


                    if (dest == nodeNames[i])
                    {
                        //If the destination number is a nodeName...
                        //Give to object of correct name
                        nodeVector[i].updateRT(*src, *cost, *src);
                        nodeVector[i].updateNeighbor(*src, *cost);
                        newDestnode = false;
                    }

                }
                if (newnode == true)
                {
                    //If it's a new node, create a new object
                    node n(*src, *dest, *cost);
                    nodeVector.push_back(n);

                    //nodeName allocation not needed
                    if (numOfnodes < maxNodes)
                    {
                        nodeNames[numOfnodes] = src;
                        ++numOfnodes;
                    }
                    //nodeName allocation needed
                    else
                    {
                        allocatenodeNames(nodeNames, numOfnodes, maxNodes);

                        nodeNames[numOfnodes] = src;
                        ++numOfnodes;
                    }
                }

                /*
                 */
                if (newDestnode == true)
                {
                    //If it's a new node, create a new object
                    node n(*dest, *src, *cost);
                    nodeVector
                    .push_back(n);

                    //nodeName allocation not needed
                    if (numOfnodes < maxNodes)
                    {
                        nodeNames[numOfnodes] = dest;
                        ++numOfnodes;
                    }
                    //nodeName allocation needed
                    else
                    {
                        allocatenodeNames(nodeNames, numOfnodes, maxNodes);

                        nodeNames[numOfnodes] = dest;
                        ++numOfnodes;
                    }
                }

                in_file.getline(topologyLine, maxLineLength);
            }
        }
        else {
            cout << "topology file did not open correctly.\n";
            cout << "Program terminated...\n";
            exit(EXIT_FAILURE);
        }
        in_file.close();
    }
    else {
        cout << "Incorrect command line input!\nArgument 0 = program, Argument 1 = topology file, Argument 2 = time\n";
        cout << "Program terminated...\n";
        exit(EXIT_FAILURE);
    }
}

void printnodeNamesArray(const string * rNames, const int numnodes)
{
    cout << "Total nodes = " << numnodes << "\n";
    cout << "nodeNames array: { ";
    for (int i = 0; i < numnodes; ++i) {
        cout << rNames[i] << ", ";
    }
    cout << "}\n";
}

void printRoutingTables(const vector<node>& n)
{
    unsigned long size = n.size();

    for (unsigned int i = 0; i < size; ++i) {
        n[i].printRT();
    }
}

void printAllNeighbors(const vector<node>& n)
{
    int size = n.size();


    // print all elements
    for (unsigned int i = 0; i < size; ++i) {
        n[i].printNeighbors();
    }

}

void allocatenodeNames(string *& nNames, const int numOfnodes, int& maxNodes)
{
    maxNodes += 2;

    string *tempnodeArrayPointer = new string[maxNodes];

    for (int i = 0; i < numOfnodes; ++i) {
        tempnodeArrayPointer[i] = nNames[i];
    }

    delete [] nNames;

    nNames = tempnodeArrayPointer;

    //Initialize new node slots to -1
    for (int j = numOfnodes; j < maxNodes; ++j) {
        nNames[j] = "-1";
    }
}

int nameToIndex(const string name, vector<node>& n, const int numnodes)
{
    for (int i = 0; i < numnodes; ++i) {
        if (name == n[i].getNodeName())
        {
            return i;
        }
    }
    return -1;
}

/*
 *  EVENTS
 */

void initializeEventQueue(vector<node>& n, const int numOfnodes, queue& q)
{
    int index;

    for (int i = 0; i < numOfnodes; ++i) {
        for (int j = 0; j < n[i].getNumOfNeighbors(); ++j) {
            index = nameToIndex(n[i].getNeighborName(j), n, numOfnodes);
            if (index == -1)
            {
                cout << "nameToIndex failed during event queue initialization...\nTerminating program.\n";
                exit(EXIT_FAILURE);
            }
            q.insert(0, 's', n[i], n[index], n[i].getDVDest(), n[i].getDVCost(), n[i].getRTSize());
        }
    }
}

void periodicUpdate(vector<node>& n, int &numOfnodes, queue& q, int currTime)
{
    int index;

    for (int i = 0; i < numOfnodes; ++i) {
        for (int j = 0; j < n[i].getNumOfNeighbors(); ++j) {
            index = nameToIndex(n[i].getNeighborName(j), n, numOfnodes);
            if (index == -1)
            {
                cout << "nameToIndex failed during event queue initialization...\nTerminating program.\n";
                exit(EXIT_FAILURE);
            }
            q.insert(currTime, 's', n[i], n[index], n[i].getDVDest(), n[i].getDVCost(), n[i].getRTSize());
        }
    }

}

void sendDVPacket(vector<node>& n, int& numOfnodes, queue & q, double currTime)
{
    int srcIndex = nameToIndex(q.getCurrentSrcName(), n, numOfnodes);
    int destIndex = nameToIndex(q.getCurrentDestName(), n, numOfnodes);
    double newTime;


    for (int i = 0; i < n[srcIndex].getNumOfNeighbors(); ++i) {
        if (n[srcIndex].getNeighborName(i) == n[destIndex].getNodeName())
        {
            newTime = currTime;
            q.insert(newTime, 'p', n[srcIndex], n[destIndex], q.getCurrentDVDest(), q.getCurrentDVCost(), q.getCurrentDVSize());    //Last 2 parameters need to be passed by value?
                                                                                                                                    //Otherwise values may be changed between this
                                                                                                                                    //event and the event where the packet is
                                                                                                                                    //processed.
            return;
        }
        else
        {
            /*
             *   Detect if node goes down here
             */
        }
    }
}

void processDVPacket(vector<node>& n, int numOfnodes, queue & q, double& currTime)
{
    int srcIndex = nameToIndex(q.getCurrentSrcName(), n, numOfnodes);
    int destIndex = nameToIndex(q.getCurrentDestName(), n, numOfnodes);
    int neighborIndex = -1;

    for (int i = 0; i < n[destIndex].getNumOfNeighbors(); ++i) {
        if (n[srcIndex].getNodeName() == n[destIndex].getNeighborName(i))
        {
            neighborIndex = i;
        }
    }

    if (n[destIndex].updateRTDV(q.getCurrentDVDest(), q.getCurrentDVCost(), q.getCurrentSrcName(), q.getCurrentDVSize(), n[destIndex].getNeighborCost(neighborIndex)))
    {
        //triggered update occurs
        for (int i = 0; i < n[destIndex].getNumOfNeighbors(); ++i) {
            //split horizon
            if (n[i].getNodeName() == n[srcIndex].getNodeName())
            {
                continue;
            }
            q.insert(currTime, 's', n[destIndex], n[i], n[destIndex].getDVDest(), n[destIndex].getDVCost(), n[destIndex].getRTSize());
        }
    }
    else
    {
        //nothing new in table, no triggered update
    }


}

void forwardDataPacket(vector<node>& n, int numOfnodes, queue & q, double currTime)
{
    int srcIndex = nameToIndex(q.getCurrentSrcName(), n, numOfnodes);
    int destIndex = nameToIndex(q.getCurrentDestName(), n, numOfnodes);
    double newTime;
    int newSrcIndex;


    for (int i = 0; i < n[srcIndex].getRTSize(); ++i) {
        if (n[srcIndex].getRTDest(i) == n[destIndex].getNodeName())
        {

            //Find new time to insert next forwarding event
            for (int j = 0; j < n[srcIndex].getNumOfNeighbors(); ++j) {
                if (n[srcIndex].getNeighborName(j) == n[srcIndex].getRTNextHop(i))
                {
                    newTime = currTime;
                    newSrcIndex = nameToIndex(n[srcIndex].getNeighborName(j), n, numOfnodes);
                    q.insert(newTime, 'f', n[newSrcIndex], n[destIndex], NULL, NULL, 0);
                    return;
                }
            }
        }
    }

}


