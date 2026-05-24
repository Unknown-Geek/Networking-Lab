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

#include <stdio.h>

int main() {
    int dist[100][100], next[100][100];
    int nodes;
    int flag;

    printf("Enter the number of nodes : ");
    scanf("%d", &nodes);

    printf("\nEnter the distance matrix : \n");
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            scanf("%d", &dist[i][j]);
            next[i][j] = j;

            if (i == j)
                dist[i][j] = 0;
        }
    }

    flag = 1;
    while (flag) {
        flag = 0;
        for (int i = 0; i < nodes; i++) {
            for (int j = 0; j < nodes; j++) {
                for (int k = 0; k < nodes; k++) {
                    if (dist[i][j] > dist[i][k] + dist[k][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        next[i][j] = next[i][k];
                        flag = 1;
                    }
                }
            }
        }
    }

    for (int i = 0; i < nodes; i++) {
        printf("\nRouter info for %c :\n", i + 65);
        printf("%-10s %-10s %-10s\n", "Dest", "NextHop", "Distance");
        for (int j = 0; j < nodes; j++) {
            if (i == j)
                continue;
            else
                printf("%-10c %-10c %-10d\n", j + 65, next[i][j] + 65, dist[i][j]);
        }
        printf("\n");
    }
}