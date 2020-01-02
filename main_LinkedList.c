#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>


// A structure to represent an adjacency list node
struct AdjListNode
{
    int dest;
    struct AdjListNode* next;
};

// A structure to represent an adjacency list
struct AdjList
{
    struct AdjListNode *head;
};

// A structure to represent a graph. A graph
// is an array of adjacency lists.
// Size of array will be V (number of vertices
// in graph)
struct Graph
{
    int V;
    struct AdjList* array;
};

// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest)
{
    struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}

// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V)
{
    struct Graph* graph =
            (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;

    // Create an array of adjacency lists. Size of
    // array will be V
    graph->array =
            (struct AdjList*) malloc(V * sizeof(struct AdjList));

    // Initialize each adjacency list as empty by
    // making head as NULL
    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;

    return graph;
}

// Adds an edge to an undirected graph
void addEdge(struct Graph* graph, int src, int dest)
{
    // Add an edge from src to dest. A new node is
    // added to the adjacency list of src. The node
    // is added at the begining
    struct AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
}

// A utility function to print the adjacency list
// representation of graph
void printGraph(struct Graph* graph)
{
    int v;
    for (v = 0; v < graph->V; ++v)
    {
        struct AdjListNode* temp = graph->array[v].head;
        printf("\n Adjacency list of vertex %d\n head ", v);
        while (temp)
        {
            printf("-> %d", temp->dest);
            temp = temp->next;
        }
        printf("\n");
    }
}



//The functionality  same as DFS_Visit in the book. We only account for finish time and the color not the discovery time.
int DFSVisit(struct Graph* graph, int visited[], int fin_time[], int *time, int v, int cycle, FILE * fpA,
             FILE * fpB, FILE * fpC, int vertices[], size_t rows, size_t columns, int mat[rows][columns]){

    //charging vertex v for the following operation
    vertices[v]+=3;

    *time +=1;
    visited[v] = 1;
    struct AdjListNode* temp = graph->array[v].head;

    while (temp){

        //charging the edge from v to its adjacent node temp->dest for the following operation
        mat[v][temp->dest]+=1;
        if(visited[temp->dest]==0) {
            if (DFSVisit(graph, visited, fin_time, time, temp->dest, 0, fpA, fpB, fpC, vertices, graph->V,graph->V, mat))
                return 1;
        }
        else {
            //charging the edge from v to its adjacent node temp->dest for the following operation
            //This is to detect a potential cycle:
            mat[v][temp->dest]+=1;
            if (fin_time[temp->dest] == 0) {
                return 1;
            }
        }
        //charging the edge from v to its adjacent node temp->dest for the following operation
        mat[v][temp->dest]+=1;
        temp = temp->next;

    }

    fprintf (fpA, "%d ",v+1);
   // printf("%d", v);
    printf("\n");

    //charging vertex v for the following operation
    vertices[v]+=3;

    visited[v]=2;
    *time+=1;
    fin_time[v] = *time;

    return 0;
}

//DFS process as in the book. The only difference is that due to using linked list we used a while loop to iterate through
// the adj list of a node using an auxiliary variable and structure.
int DFS(struct Graph* graph, int visited[], int fin_time[], int *time, FILE * fpA, FILE * fpB, FILE * fpC,
        int vertices[], size_t rows, size_t columns, int mat[rows][columns]){

    for (int v=0; v< graph->V; v++){

        //charging vertex v for the following operation.
        vertices[v]+=1;

        if (visited[v]==0) {
            if(DFSVisit(graph, visited, fin_time, time, v, 0, fpA, fpB, fpC, vertices, graph->V,graph->V, mat))
                return 0;

        }
    }

    return 1;
}

//This is to write into the file for B, if you wish the usage of this file can be ommitted.
// this is not taken in the time complexity
void populateB(FILE * fpB, int vertices[], size_t col, size_t row, int mat[col][row]){
    int max1=0, max2=0, sum =0;
    for (int i =0; i<row; i++){
        if (vertices[i]>= max1)
            max1 = vertices[i];
        sum+= vertices[i];
        fprintf (fpB, "Vertex %d: Total %d operations charged.\n",i+1, vertices[i]);
        for (int j =0; j<col; j++){
            if(mat[i][j]>=max2)
                max2 = mat[i][j];
            sum+= mat[i][j];
        }
    }
    //int counter = 0;
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            if(mat[i][j]> 0) {
                fprintf(fpB, "edge [%d][%d]: Total %d operations charged.\n", i + 1, j + 1, mat[i][j]);
                //counter++;
            }
        }
    }
    //printf("Counter %d", counter);
    fprintf (fpB, "Maximum number of operation charged to any single vertex is: %d\n",max1);
    fprintf (fpB, "Maximum number of operation charged to any single edge is: %d\n", max2);
    fprintf (fpB, "Total number of operations is: %d\n", sum);
    fclose(fpB);
}

void populateC(FILE * fpC, int vertices[], size_t col, size_t row, int mat[col][row]){
    int max1=0, max2=0, sum =0;
    for (int i =0; i<row; i++){
        if (vertices[i]>= max1)
            max1 = vertices[i];
        sum+= vertices[i];
        for (int j =0; j<col; j++){
            if(mat[i][j]>=max2)
                max2 = mat[i][j];
            sum+= mat[i][j];
        }
    }
    fprintf (fpC, "Maximum number of operation charged to any single vertex is: %d\n",max1);
    fprintf (fpC, "Maximum number of operation charged to any single edge is: %d\n", max2);
    fprintf (fpC, "Total number of operations is: %d\n", sum);
    fclose(fpC);
}

int main(int argc, char *argv[]) {

/////////////////////////////////////////////First part: Parsing the data and Initializing the Adjacency Lists.
    FILE *ifp;
    if (argc < 2)
    {
        printf("too few arguments !");
        return 0;
    }
    clock_t begin = clock();

    // read input from file
    ifp = fopen(argv[1], "r");
    int num_v = 0;
    char l[256];


    while (fgets(l, sizeof l, ifp) != NULL) num_v++;

    // Only for finding the charges on each vertex and each edge.//Not included in calculations related to time complexity.
    //If you only want to see the effect of the second part please consider commenting this block and uncomment the other
    // one at the beginning of the second part:
//
    int mat [num_v][num_v];
    int vertices[num_v];
    for (int i =0; i<num_v; i++){
        //Vertices start with one to account for the previous operations.
        vertices[i]=1;
        for(int j=0; j<num_v; j++)
            mat[i][j]=0;
    }
//

    rewind(ifp);
    struct Graph* graph = createGraph(num_v);
    char line[1024];

//9 Constant time operations up to here.
    //In each iteration of the following loop 6 operations happen before the next while. (aggregate analysis.)
    while (fgets(line, sizeof line, ifp) != NULL)
    {


        int src_vertex, dst_vertex;
        char * pch;
        pch = strtok(line, " \n\r");

        sscanf(pch, "%d", &src_vertex);

        pch = strtok(NULL, " \n\r");
        //charging vertex v for the previous operations.
        vertices[src_vertex-1]+=5;
        //In each iteration of the following loop 5 operations occur.
        while (pch != NULL)
        {

            // there is an edge from src_vertex to dst_vertex
            sscanf(pch, "%d", &dst_vertex);

            //Each call to addEdge will have 3 operations included.
            //charging edge [src_vertex][dst_vertex] for the following operation.
            mat[src_vertex-1][dst_vertex-1]+=4;
            addEdge(graph, src_vertex-1, dst_vertex-1);
            //charging edge [src_vertex][dst_vertex] for the following operation.
            mat[src_vertex-1][dst_vertex-1]+=1;
            pch = strtok(NULL, " \n\r");
        }
    }



    fclose(ifp);



/////////////////////////////////////////////////// Second part: Running DFS.
//Defining the files and the initialization for DFS.
    FILE * fpA, * fpB, * fpC;
    fpA = fopen ("A.txt","w");
    fpB = fopen ("B.txt","w");
    fpC = fopen ("C.txt","w");
    int visited [graph->V];
    int fin_time [graph->V];
    int time =0;
    for (int i=0; i<graph->V; i++) {
        visited[i] = 0;
        fin_time[i] = 0;
    }

    //Up to here 3 constant time operations together with 2V.
    //printGraph(graph);
//    int mat [num_v][num_v];
//    int vertices[num_v];
//    for (int i =0; i<num_v; i++){
//        //Vertices start with one to account for the previous operations.
//        vertices[i]=0;
//        for(int j=0; j<num_v; j++)
//            mat[i][j]=0;
//    }

// The result will be a flag of int 0 or 1. When the flag is 1 it means that the topological order is found
//While when it is 0, the graph has cycle.
    int result = DFS(graph, visited, fin_time, &time, fpA, fpB, fpC, vertices, num_v, num_v, mat);

    //This part is optional and is only for the grader to verify the code is actually running correctly.
    if(result){
        printf("Finish Time Array:\t");
        for (int i =0; i< graph->V; i++)
            printf("%d\t", fin_time[i]);
        printf("\nVisited Array:\t\t");
        for (int i =0; i< graph->V; i++)
            printf("%d\t", visited[i]);
        printf("\n");

        printf("Vertex Charge Array:\t");
        for (int i =0; i< graph->V; i++)
            printf("%d\t", vertices[i]);
        printf("\nEdge Charge Array:\t\t");

        for (int i =0; i< graph->V; i++) {
            printf("\n");
            for (int j = 0; j < graph->V; j++)
                printf("%d\t", mat[i][j]);
        }
        printf("\n");
    }
    else{
        //printf("Blink\n");
        freopen("A.txt","w+",fpA);
        fprintf (fpA, "The graph has a cycle!");
    }
    fclose (fpA);
    //fclose (fpB);
    //fclose (fpC);


    //These following operations are not considered while finding the time complexity.

    populateB(fpB, vertices, num_v, num_v, mat);
    populateC(fpC, vertices, num_v, num_v, mat);
    clock_t end = clock();
    printf("Time taken = %lf\n", ((double)end-begin)/CLOCKS_PER_SEC);
    return 0;
}

