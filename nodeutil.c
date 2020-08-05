#include <stdio.h>
/* ******************************************************************
Contains procedures and constants used by nodes in the distance vector 
simulation working under the constraint that no network metadata is "shared"
between nodes. An exception is made for NODECOUNT and INFINITY for the
purpose of convenience.

**********************************************************************/

extern struct rtpkt {
int sourceid;       /* id of sending router sending this pkt */
int destid;         /* id of router to which pkt being sent 
					(must be an immediate neighbor) */
int mincost[4];    /* min cost to node 0 ... 3 */
};

const int NODECOUNT = 4; // number of total nodes in graph
const int INFINITY = 999; // convention

void tolayer2(struct rtpkt packet);

// Set distance table distances to infinity 
//
// Note: We make a subtle assumption in that the "column" in the disttable
// for selfid will will always have the exact link costs to the directly attached 
// neighbors. This works because disttable is defined: disttable[destination][via]
// and updatecosts() will never receive an update where via == selfid
void init_disttable(int selfid, int *disttable, int *mincost)
{
	printf("Initializing Node %d\n", selfid);
	for (int id_i = 0; id_i < NODECOUNT; id_i++)
	{
		for (int id_j = 0; id_j < NODECOUNT; id_j++)
		{
			/* Note: using pointer arithmetic & assumption that "multidimensional"
			arrays are allocated contiguously to index into disttable */

			if (id_j == selfid)
			{
				// "column" for selfid will always have link cost to *possible* neighbor id_i
				*((disttable + id_i * NODECOUNT) + id_j) = mincost[id_i];
			}
			else
			{
				*((disttable + id_i * NODECOUNT) + id_j) = INFINITY;
			}
		}
	}
}

// Transmit vector update to neighbors
void updateneighbors(int selfid, int *mincost, int *disttable)
{
	printf("Node %d sends its updated least cost vector { %d, %d, %d, %d } to its neighbors\n", 
		selfid, mincost[0], mincost[1], mincost[2], mincost[3]);

	// Create routingpacket
	struct rtpkt packet;
	packet.sourceid = selfid;

	// Populate packet with sender's least cost vector
	for (int id = 0; id < NODECOUNT; id++)
	{
		packet.mincost[id] = mincost[id];
	}

	// Send updates to neighbors through network
	int neighbor_cost;
	for (int id = 0; id < NODECOUNT; id++)
	{
		// cost to directly attached neighbor (id) is disttable[id][selfid]
		neighbor_cost = *((disttable + id * NODECOUNT) + selfid);

		// infer if neighbor through fact that costs range between (0, 999)
		if (neighbor_cost > 0 && neighbor_cost < INFINITY)
		{
			packet.destid = id;
			tolayer2(packet); /* NOTE: reassigning destid and sending the
								same packet shouldn't affect subsequent
								transmissions as passing structs by
								value result in "deep" copies */
		}

	}
}

// Updates the node's distance table with neighbor's new values and
// checks if neighbor's update will modify node's least-cost distance vector
// 
// Returns 1 if node's least cost vector is modified, 0 if not
// Note: Routing loops do occur, but resolve with enough iterations across network.
int updatecosts(struct rtpkt *rcvdpkt, int selfid, int *mincost, int *disttable)
{
	int senderid = rcvdpkt->sourceid;

	// get cost to sender in disttable[senderid][selfid]
	int cost_to_sender = *((disttable + senderid * NODECOUNT) + selfid);
	int *sendermincosts = rcvdpkt->mincost;
	int mincost_updated = 0;
	int new_sendercost, prev_sendercost;

	// update distance table's costs for sender
	for (int id = 0; id < NODECOUNT; id++)
	{
		prev_sendercost = *((disttable + id * NODECOUNT) + senderid); // cost via sender to id
		new_sendercost = cost_to_sender + sendermincosts[id];

		// keep infs as 999 for illustrative purposes
		if (new_sendercost > INFINITY) new_sendercost = INFINITY; 

		// if cost to node (id) was updated
		if (new_sendercost != prev_sendercost)
		{
			// update disttable[id][senderid] with new cost
			*((disttable + id * NODECOUNT) + senderid) = new_sendercost;

			// if lower least-cost to node (id) found via sender
			if (new_sendercost < mincost[id])
			{
				printf("Node %d finds a lower cost path to Node %d via Node %d! cost: %d->%d\n",
					selfid, id, senderid, mincost[id], new_sendercost);

				// no need to iterate over costs for this id since this is the smallest
				mincost[id] = new_sendercost;
				mincost_updated = 1;
			}
			// if modified cost was least-cost to node (id)
			else if (prev_sendercost == mincost[id])
			{
				// find least-cost neighbor to id
				// Remember: table defined as disttable[destination][via]
				int min = INFINITY; 
				int min_id, cost_via_idj;
				for (int id_j = 0; id_j < NODECOUNT; id_j++)
				{
					// cost_via_idj = disttable[id][id_j]
					cost_via_idj = *((disttable + id * NODECOUNT) + id_j);

					if (cost_via_idj < min)
					{
						min = cost_via_idj;
						min_id = id_j;
					}
				}

				printf("Node %d's least-cost path to Node %d recalculated due to link cost change. cost: %d->%d\n",
					selfid, id, mincost[id], min);
				mincost[id] = min;
				mincost_updated = 1;
			} 
			// else cost to node via sender changed but does not impact mincost
		}
	}
	return mincost_updated;
}

// Called when cost of link to directly attached neighbor changes
void linkhandler(int linkid, int newcost, int selfid, int *mincost, int *disttable)
{
	int old_linkcost = *((disttable + linkid * NODECOUNT) + selfid);	// disttable[linkid][selfid]
	printf("\nNode %d's link to Node %d experiences a cost change! cost: %d->%d\n", 
		selfid, linkid, old_linkcost, newcost);

	// make a fake update packet from neighbor across link to recalculate costs
	struct rtpkt linkupdate;
	linkupdate.sourceid = linkid;
	linkupdate.destid = selfid;

	// fill mincost with neighbor's most recent least-cost values
	for (int id = 0; id < NODECOUNT; id++)
	{
		// mincost{linkid}[id] == disttable[id][linkid] - old_linkcost
		linkupdate.mincost[id] = *((disttable + id * NODECOUNT) + linkid) - old_linkcost;
	}

	// update link cost to neighbor via self ( disttable[linkid][selfid] )
	*((disttable + linkid * NODECOUNT) + selfid) = newcost;

	// "send" the update
	int mincost_updated = updatecosts(&linkupdate, selfid, mincost, disttable);
	if (mincost_updated)
	{
		updateneighbors(selfid, mincost, disttable);
	}
}