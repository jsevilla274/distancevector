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
} dt3;

void printdt3(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}

/* students to write the following two routines, and maybe some others */

#define SELFID 3
int mincost3[4] = {7, 999, 2, 0};

void rtinit3() 
{
	init_disttable(SELFID, (int *)dt3.costs, mincost3);
	updateneighbors(SELFID, mincost3, (int *)dt3.costs);
}


void rtupdate3(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
	int mincost_updated = updatecosts(rcvdpkt, SELFID, mincost3, (int *)dt3.costs);
	printdt3(&dt3);	// print distance table

	if (mincost_updated)
	{
		updateneighbors(SELFID, mincost3, (int *)dt3.costs);
	}
}
