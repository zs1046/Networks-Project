# Networks-Project
-An Overview
In this project, you are asked to simulate Distance Vector routing for a given network. The main goal of
this project is to study the impact of different factors on the convergence times to optimal routes. You will
be provided with multiple files that represent different network topologies. Your simulator would need build
routing tables and then forward data packets until they reach their destinations based on the routing tables
built.

-The Network Topology Files
Each network topology file consists of a number of rows, each row represents a single edge in the network.
There are three entries per row. The first entry is the node ID at one end, the second is the node ID at the
other end, and the third entry is the cost of the link between the nodes which will be used in computing
optimal routes. For example, a row with these values: 2 12 23 means that there is a link between node 2
and node 12 and that link has a cost 23. Here are three topologies to use:

• Toplogy1.txt (5 nodes, 7 edges)

• Toplogy2.txt (10 nodes, 20 edges)

• Toplogy3.txt (30 nodes, 60 edges)

-Initially, every node is only aware of its immediate neighbors (and thus would not have a complete picture
of the topology). To build routing tables, nodes will proceed in “Rounds”. At the beginning of every round,
each node will prepare a DV packet that it would send to its immediate neighbors. DV packets include the
source node and the list of nodes-costs pairs for what this node knows about the network. In every round,
a pair of nodes that are connected will exchange their DV packets and update their tables.

-The above topologies were generated using BRITE. The grader may test/grade your code on different
topologies than the ones provided above, so make sure nothing is hard-coded (even the number of nodes!).
Your simulator should take as a line argument, which topology file to use along with the duration to run
your simulation (e.g., how many rounds needs to be simulated).

-Distance Vector
Recall that in a distance vector-based routing algorithm, each node would tell its neighbors, what it knows
about the whole network. As nodes exchange information with their neighbors in every round, they update
their routing tables if better routes are discovered. Each node would maintain a routing table that consists
of ¡destination, cost, next hop¿. The DV packet each node sends is simply the destination and cost pairs (no
need to send the next hop).

When a node receives a data packet, it consults its routing table and forwards the packet to the next
hop, which would do the same until the packet reaches the destination.
