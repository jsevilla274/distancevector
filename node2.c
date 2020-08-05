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
} dt2;

void printdt2(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
	 dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
	 dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
	 dtptr->costs[3][1],dtptr->costs[3][3]);
}

/* students to write the following two routines, and maybe some others */

#define SELFID 2
int mincost2[4] = {3, 1, 0, 2};

void rtinit2() 
{
	init_disttable(SELFID, (int *)dt2.costs, mincost2);
	updateneighbors(SELFID, mincost2, (int *)dt2.costs);
}


void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
	int mincost_updated = updatecosts(rcvdpkt, SELFID, mincost2, (int *)dt2.costs);
	printdt2(&dt2);	// print distance table

	if (mincost_updated)
	{
		updateneighbors(SELFID, mincost2, (int *)dt2.costs);
	}
}
