/*
 * Link State Routing (Dijkstra's Algorithm)
 * -------------------------------------------
 * Input: num_nodes, links (src, dest, cost) → fill graph[][]
 *   └─ dijkstra(start) for each node:
 *       └─ init: distance[all]=INF, distance[start]=0
 *       └─ repeat num_nodes-1 times:
 *           ├─ pick min_node: unvisited with smallest distance
 *           ├─ mark visited
 *           └─ relax neighbors: if dist[min]+graph[min][i] < dist[i] → update
 *       └─ print routing table (destination, next hop, cost)
 *
 * Key arrays: distance[] = shortest cost, previous[] = next hop
 */

#include <stdbool.h>
#include <stdio.h>

#define MAX 25
#define INF 99999

int nodes;
int graph[MAX][MAX];

void dijkstra(int src) {
    int dist[MAX];
    int prev[MAX];
    int visited[MAX];

    for (int i = 0; i < nodes; i++) {
        dist[i] = INF;
        prev[i] = -1;
        visited[i] = false;
    }

    dist[src] = 0;

    while (true) {
        int u = -1;

        for (int i = 0; i < nodes; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }

        if (u == -1) {
            break; // No more nodes to visit
        }

        visited[u] = true;

        for (int v = 0; v < nodes; v++) {
            if (!visited[v] && graph[u][v] != INF) {
                int newdist = dist[u] + graph[u][v];
                if (newdist < dist[v]) {
                    dist[v] = newdist;
                    prev[v] = u;
                }
            }
        }
    }

    printf("Routing table for node %c:\n", src + 65);
    printf("%-10s %-10s %-10s\n", "Dest", "Next", "Distance");

    for (int i = 0; i < nodes; i++) {
        if (src == i) {
            continue;
        }

        int nexthop = i;
        while (prev[nexthop] != src && prev[nexthop] != -1) {
            nexthop = prev[nexthop];
        }

        printf("%-10c %-10c %-10d\n", i + 65, nexthop + 65, dist[i]);
    }

    printf("\n");
}

int main(void) {
    int links;
    int src;
    int dest;
    int cost;

    printf("Enter the number of nodes: ");
    scanf("%d", &nodes);

    // Init graph
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            if (i == j)
                graph[i][j] = 0;
            else
                graph[i][j] = INF;
        }
    }

    printf("Enter the number of links: ");
    scanf("%d", &links);

    printf("\nEnter the src, dest, cost:\n");
    for (int i = 0; i < links; i++) {
        scanf("%d %d %d", &src, &dest, &cost);
        graph[src][dest] = cost;
        graph[dest][src] = cost;
    }

    printf("\n");

    for (int i = 0; i < nodes; i++) {
        dijkstra(i);
    }

    return 0;
}
