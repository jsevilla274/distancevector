void init_disttable(int selfid, int *disttable, int *mincost);
void updateneighbors(int selfid, int *mincost, int *disttable);
int updatecosts(struct rtpkt *rcvdpkt, int selfid, int *mincost, int *disttable);
void linkhandler(int linkid, int newcost, int selfid, int *mincost, int *disttable);

extern const int NODECOUNT; // number of total nodes in graph
extern const int INFINITY; // convention
