#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <time.h>

#define MAX_VERTEICSES 30

char city[MAX_VERTEICSES][128];

typedef struct GraphNode{
    int vertex;
    float weight;
    struct GraphNode *next;
}GraphNode;

typedef struct{
    int n; 
    GraphNode *adj_list[MAX_VERTEICSES];
}Graph;

typedef struct{
    int vertex;
    float distance;
}Queue;

int Extract_Min(Queue *q, int N){

    float min = q[0].distance;
    int min_idx = 0;

    for(int i = 1; i <= N; i++){
        if(min > q[i].distance){
            min = q[i].distance;
            min_idx = i;
        }
    }

    Queue temp;
    temp = q[min_idx];
    q[min_idx] = q[N];
    q[N] = temp;

    return min_idx;
}

void Relax(Queue u, Queue *v, int w){
    if(v->distance > u.distance + w)
        v->distance = u.distance + w;
}

void Dijkstra(Graph graph, int source, int distance[][MAX_VERTEICSES]){

    Queue s[graph.n];
    Queue q[graph.n];
    int v[graph.n];

    for(int i = 0; i < graph.n; i++){
        q[i].vertex = i;
        v[i] = i;
        if(i == source)
            q[i].distance = 0;
        else
            q[i].distance = INFINITY;
    }

    int N = graph.n-1;
    int idx = 0;
    int min_idx;
    Queue u;

    while(N >= 0){

        // do u ← EXTRACT-MIN(Q)
        min_idx = Extract_Min(q, N);
        u = q[N--];

        // S ← S U {u}
        s[u.vertex] = u;
        idx++;

        // for each vertex v ∈ Adj[u]
        // do RELAX(u, v, w)
        GraphNode *temp = graph.adj_list[u.vertex];
        temp = temp->next;
        while(1){
            if(temp == NULL)
                break;

            int i;
            for(i = 0; i < graph.n; i++){
                if(q[i].vertex == temp->vertex)
                    break;
            }

            Relax(u, &q[i], temp->weight);

            temp = temp->next;
        }

    }

    for(int i = 0; i < idx; i++)
        distance[source][i] = (int)s[i].distance;

}

void Floyd(float weight[][MAX_VERTEICSES], int N, int distance[][MAX_VERTEICSES]){
    
    float d[N][N];

    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            d[i][j] = weight[i][j];

    for(int k = 0; k < N; k++){
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(d[i][k] + d[k][j] < d[i][j])
                    d[i][j] = d[i][k] + d[k][j];
                else
                    d[i][j] = d[i][j];
            }
        }
    }

    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
           distance[i][j] = (int)d[i][j];

}

int main(){

    FILE *fp = fopen("hw7.data", "rt");

    // if the file does not exist or there is an error in the file, exit the program
    if(fp == NULL){
        printf("Can't Open!:(\n");
        exit(1);
    }
    int size = 128;
    int s = 0;
    char buffer[size];
    char first[size];

    while(!feof(fp)){
        fscanf(fp, "%s", buffer);
        if((buffer[0] >= 'A' && buffer[0] <= 'Z') || (buffer[0] >= 'a' && buffer[0] <= 'z')){
            if((strcmp(first, buffer) == 0))
                break;
            
            strcpy(city[s++], buffer);

            if(s == 1)
                strcpy(first, buffer);

        }
    }

    Graph graph;
    graph.n = s;
    // Assume number of nodes in your graph is less than or equal to 30.
    if(graph.n > MAX_VERTEICSES){
            printf("Maxinum number of node = 30, try again!:)\n");
            exit(1);
    }

    int num = 0;
    int w = 0;
    int index = 0;
    float weight[graph.n][MAX_VERTEICSES];

    while(!feof(fp)){
        fscanf(fp, "%s", buffer);
        
        if(atoi(buffer) || strcmp(buffer, "INF") == 0 || buffer[0] == 48){

            if(num == 0){
                if(atoi(buffer))
                    weight[index][num] = (float)atoi(buffer);
                else if(buffer[0] == 48)
                    weight[index][num] = 0;
                else   
                    weight[index][num]= INFINITY;

                GraphNode *node = (GraphNode*)malloc(sizeof(GraphNode));
                node->vertex = index;
                node->weight = 0;
                node->next = NULL;
                graph.adj_list[index++] = node;
            }
            else{
                if(atoi(buffer))
                    weight[index-1][num] = (float)atoi(buffer);
                else if(buffer[0] == 48)
                    weight[index-1][num] = 0;
                else   
                    weight[index-1][num] = INFINITY;
            }

            if(atoi(buffer)){
                w = atoi(buffer);
                if(w != 0){
                    
                    GraphNode *temp = graph.adj_list[index-1];
                    while(1){
                        if(temp->next == NULL){
                            GraphNode *node = (GraphNode*)malloc(sizeof(GraphNode));
                            node->vertex = num;
                            node->weight = w;
                            node->next = NULL;
                            temp->next = node;
                            break;
                        }
                        temp = temp->next;
                    }
                }
            }

            num++;
            if(num == s)
                num = 0;
            
            
        }
    }

    int distance[MAX_VERTEICSES][MAX_VERTEICSES];
    clock_t start, end;
    float t;

    start = clock();
    for(int i = 0; i < graph.n; i++)
        Dijkstra(graph, i, distance);
    end = clock();
    t = (float)(end-start)/CLOCKS_PER_SEC;

    printf("\nIt took %f seconds to compute shortest path between cities with Dijkstra's algorithm as follows.\n\n", t);
    printf("%10s ", " ");
    for(int i = 0; i < graph.n; i++)
        printf("%8s ", city[i]);
    printf("\n");

    for(int i = 0; i < graph.n; i++){
        printf("%10s ", city[i]);
        for(int j = 0; j < graph.n; j++){
            printf("%8d ", distance[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    start = clock();
    Floyd(weight, graph.n, distance);
    end = clock();
    t = (float)(end-start)/CLOCKS_PER_SEC;

    printf("\nIt took %f seconds to compute shortest path between cities with Dijkstra's algorithm as follows.\n\n", t);
    printf("%10s ", " ");
    for(int i = 0; i < graph.n; i++)
        printf("%8s ", city[i]);
    printf("\n");

    for(int i = 0; i < graph.n; i++){
        printf("%10s ", city[i]);
        for(int j = 0; j < graph.n; j++){
            printf("%8d ", distance[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    fclose(fp);

    return 0;
}
