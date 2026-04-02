#include<stdio.h>
#include<stdbool.h>

#define MAX_NODES 10
#define INF 99999

int numNodes ;
int graph [ MAX_NODES ][ MAX_NODES ];
bool visited [ MAX_NODES ];

void dijkstra(int startNode) {
    int distance[MAX_NODES];
    int previous[MAX_NODES];

    for (int i = 0; i < numNodes; i++) {
        distance[i] = INF;
        visited[i] = false;
        previous[i] = -1;
    }

    distance[startNode] = 0;

    for (int count = 0; count < numNodes - 1; count++) {

        int minDistance = INF;
        int minNode = -1;

        for (int i = 0; i < numNodes; i++) {
            if (!visited[i] && distance[i] < minDistance) {
                minDistance = distance[i];
                minNode = i;
            }
        }

        if (minNode == -1)
            break;

        visited[minNode] = true;

        for (int i = 0; i < numNodes; i++) {
            if (!visited[i] &&
                graph[minNode][i] != INF &&
                distance[minNode] + graph[minNode][i] < distance[i]) {

                distance[i] = distance[minNode] + graph[minNode][i];
                previous[i] = minNode;
            }
        }
    }

    printf("\nRouting Table for Node %d:\n", startNode);
    printf("Destination\tNext Hop\tCost\n");

    for (int i = 0; i < numNodes; i++) {
        if (i != startNode) {
            printf("%d\t\t%d\t\t%d\n",
                   i, previous[i], distance[i]);
        }
    }
}


int main () {
	printf ( "Enter the number of nodes : " ) ;
	scanf ("%d" , &numNodes ) ;
// Initialize the graph
	for ( int i = 0; i < numNodes ; i ++) {
		for ( int j = 0; j < numNodes ; j ++) {
			if ( i == j )
				graph [ i ][ j ] = 0;
			else
				graph [ i ][ j ] = INF ;
		}
	}

	int numLinks ;
	printf ( "Enter the number of links : " ) ;
	scanf ( "%d" , &numLinks ) ;

// Add links to the graph
    printf ( "Enter source , destination , and cost of links: \n") ;
	for ( int i = 0; i < numLinks ; i ++) {
		int src , dest , cost ;
		scanf ( "%d %d %d" , &src , &dest , &cost ) ;
		graph [ src ][ dest ] = cost ;
		graph [ dest ][ src ] = cost ;
	}

// Perform Link State Routing for each node
	for ( int i = 0; i < numNodes ; i ++) {
		dijkstra ( i ) ;
	}
	return 0;
}
