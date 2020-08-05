#include <stdio.h>
#include "nodeutil.h"

extern struct rtpkt {
int sourceid;       /* id of sending router sending this pkt */
int destid;         /* id of router to which pkt being sent 
					(must be an immediate neighbor) */
int mincost[4];    /* min cost to node 0 ... 3 */
};

extern int TRACE;
extern int YES;
extern int NO;

struct distance_table 
{
	int costs[4][4];
} dt0;

void printdt0(dtptr)
	struct distance_table *dtptr;
{
	printf("                via     \n");
	printf("   D0 |    1     2    3 \n");
	printf("  ----|-----------------\n");
	printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
	dtptr->costs[1][2],dtptr->costs[1][3]);
	printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
	dtptr->costs[2][2],dtptr->costs[2][3]);
	printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
	dtptr->costs[3][2],dtptr->costs[3][3]);
}

/* students to write the following two routines, and maybe some others */

#define SELFID 0
int mincost0[4] =  {0, 1, 3, 7}; /* contains least-costs to all other nodes
									indexed by id, initialized with costs to
									directly attached neighbors, or INFINITY 
									otherwise */	
									 
/* NOTE: An important implication of the array "type" is that it is both a
memory allocation and pointer assignment in the stack(?)*/

/* Called at the beginning, it should initialize distance table to reflect
costs to its neighbors. After initializing the necessary data structures, it 
sends its distance vector to its neighbors via tolayer2()*/
void rtinit0() 
{
	init_disttable(SELFID, (int *)dt0.costs, mincost0);
	updateneighbors(SELFID, mincost0, (int *)dt0.costs);
}

/* Called when node receives a routing packet by one of its neighbors. Note that
if a minimum cost is changed as a consequence of this update, this node (receiver)
will send its distance vector to its neighbors via tolayer2()*/
void rtupdate0(rcvdpkt)
	struct rtpkt *rcvdpkt;
{
	int mincost_updated = updatecosts(rcvdpkt, SELFID, mincost0, (int *)dt0.costs);
	printdt0(&dt0);	// print distance table

	if (mincost_updated)
	{
		updateneighbors(SELFID, mincost0, (int *)dt0.costs);
	}
}

/* called when cost from 0 to linkid changes from current value to newcost
You can leave this routine empty if you're an undergrad. If you want
to use this routine, you'll need to change the value of the LINKCHANGE
constant definition in prog3.c from 0 to 1 */
void linkhandler0(linkid, newcost)   
	int linkid, newcost;
{
	linkhandler(linkid, newcost, SELFID, mincost0, (int *)dt0.costs);
	printdt0(&dt0);	// print distance table
}
