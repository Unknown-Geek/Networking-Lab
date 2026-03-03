#include <stdio.h>
#include <stdbool.h>

#define MAX_NODES 10
#define INF 99999

int num_nodes;
int graph[MAX_NODES][MAX_NODES];
bool visited[MAX_NODES];

void dijkstra(int start) {
        int distance[MAX_NODES];
        int previous[MAX_NODES];

        for (int i = 0; i < num_nodes; i++) {
                distance[i] = INF;
                visited[i] = false;
                previous[i] = -1;
        }

        distance[start] = 0;

        for (int count = 0; count < num_nodes - 1; count++) {
                int min_dist = INF, min_node = -1;
                for (int i = 0; i < num_nodes; i++) {
                        if (!visited[i] && distance[i] < min_dist) {
                                min_dist = distance[i];
                                min_node = i;
                        }
                }

                if (min_node == -1) break;
                visited[min_node] = true;

                for (int i = 0; i < num_nodes; i++) {
                        if (!visited[i] && graph[min_node][i] != INF &&
                            distance[min_node] + graph[min_node][i] < distance[i]) {
                                distance[i] = distance[min_node] + graph[min_node][i];
                                previous[i] = min_node;
                        }
                }
        }

        printf("\nRouting table for node %d:\n", start);
        printf("Destination\tNext Hop\tCost\n");
        for (int i = 0; i < num_nodes; i++) {
                if (i != start)
                        printf("%d\t\t%d\t\t%d\n", i, previous[i], distance[i]);
        }
}

int main() {
        printf("Enter number of nodes: ");
        scanf("%d", &num_nodes);

        for (int i = 0; i < num_nodes; i++)
                for (int j = 0; j < num_nodes; j++)
                        graph[i][j] = (i == j) ? 0 : INF;

        int num_links;
        printf("Enter number of links: ");
        scanf("%d", &num_links);

        printf("Enter source, destination, and cost of each link:\n");
        for (int i = 0; i < num_links; i++) {
                int src, dest, cost;
                scanf("%d %d %d", &src, &dest, &cost);
                graph[src][dest] = cost;
                graph[dest][src] = cost;
        }

        for (int i = 0; i < num_nodes; i++)
                dijkstra(i);

        return 0;
}
