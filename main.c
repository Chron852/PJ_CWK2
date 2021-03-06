#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<SDL2/SDL.h>
#include<ui.h>
#include "CUnit/Basic.h"

// the struct data is to load the edges of map
typedef struct data
{
    int start;// the start point
    int end;// the end point
    long double length;// the length of edges
}data;

// the struct node is to load the node of map
typedef struct node{
    int ID_origin;//ID_origin is to store nodes that have not been reassigned
    int ID_normal;// ID_normal is to store nodes that have been reassigned
    long double lat;
    int ave_lat;// the lat that has been changed to fix for the map
    long double lon;
    int ave_lon;// the lon that has been changed to fix for the map
    int prev;// the node that the best path has passed before this node
}node;
int fl = 0;
long double **matrix;
data *Data;
node *Node;
long double *dist,**dist_floyd;
int **pass;// the passing node that the arithmetic Floyd calculated
int sum = 0,num = 0;//sum is the number of edges and num is the number of nodes
int endi,starti,endk = 0,startk = 0; // the starti and endi are the origin Node ID and startk and endk are the simplified Node ID
uiEntry *s,*e;// the entry box of start and end Node
uiLabel *best;// the label is to list the best path
char c_floyd[1000];// this array is to load the passing node of FLoyd


//this function is to load map files
void loadmap(char *filename){
    long double min_lat = 100;
    long double min_lon = 100;
    char a1[100],a2[100],a3[100],a4[100],a5[100],a6[100],a7[100],a8[100],a9[100],a10[100],a11[100],a12[100];
    FILE *f = fopen(filename,"r");
    if(f == NULL){
        printf("this file dose not exist!\n");
        exit(0);
    }
    char buf[1024];
    sum = 0;
    // this loop is to get the number of edges
    while(fgets(buf,1024,f)){
        if(strstr(buf,"link id")){
            sum++;
        }
    }
    fclose(f);
    Data = (data *)malloc((sum + 1) * sizeof (data));
    sum = 0;
    FILE *g = fopen(filename,"r");
    // this loop is to load the edges from map to data
    while(fgets(buf,1024,g)){
        if(strstr(buf,"link id")){
            sscanf(buf,"<link id=%s node=%s node=%s way=%s length=%s veg=%s arch=%s land=%s POI=%s;/link>",&a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9);
            sum++;
            Data[sum].start = atoi(a2);
            Data[sum].end = atoi(a3);
            Data[sum].length = atof(a5);
        }
    }
    fclose(g);
    FILE *h = fopen(filename,"r");
    // this loop is to get the number of edges
    while(fgets(buf,1024,h)){
        if(strstr(buf,"node id")){
            num++;
        }
    }
    fclose(h);
    Node = (node *)malloc((num + 1) * sizeof (node));
    dist = (long double*)malloc((num + 1) * sizeof (long double));
    num = 0;
    FILE *v = fopen(filename,"r");
    // this loop is to load node from map to Node
    while(fgets(buf,1024,v)){
        if(strstr(buf,"node id")){
            sscanf(buf,"<node id=%s lat=%s lon=%s /node>",&a10,&a11,&a12);
            num++;
            Node[num].ID_origin = atoi(a10);
            Node[num].lat = atof(a11);
            Node[num].lon = atof(a12);
            if(Node[num].lat < min_lat){
                min_lat = Node[num].lat;
            }
            if(Node[num].lon < min_lon){
                min_lon = Node[num].lon;
            }
        }
    }
    fclose(v);
    int tmp;
    // this loop is to sort the node ID to better rearrange the Node ID
    for(int i = 1;i<=num;i++){
        tmp = i;
        for(int j = i + 1;j<=num;j++){
            if(Node[j].ID_origin<Node[tmp].ID_origin){
                tmp = j;
            }
        }
        if(tmp != i){
            int temp = Node[i].ID_origin;
            Node[i].ID_origin = Node[tmp].ID_origin;
            Node[tmp].ID_origin = temp;
            long double tempp = Node[i].lat;
            Node[i].lat = Node[tmp].lat;
            Node[tmp].lat = tempp;
            tempp = Node[i].lon;
            Node[i].lon = Node[tmp].lon;
            Node[tmp].lon = tempp;
        }
    }
    // this loop is to simplify the lat and lon to better visualize the map
    for(int i = 1;i <= num;i++){
        Node[i].ID_normal = i;
        Node[i].ave_lon = (int)((Node[i].lon - min_lon) * 100000);
        Node[i].ave_lat = (int)((Node[i].lat - min_lat) * 100000);
    }
    // this loop is to change the Node ID from original into simplified in Data
    for(int i = 1;i <= sum;i++){
        int flag1 = 0,flag2 = 0;
        for(int j = 1;j <= num;j++){
            if(Data[i].start == Node[j].ID_origin && flag1 == 0){
                Data[i].start = Node[j].ID_normal;
                flag1 = 1;
            }
            if(Data[i].end == Node[j].ID_origin && flag2 == 0){
                Data[i].end = Node[j].ID_normal;
                flag2 = 1;
            }
        }
    }
    // to request memory
    matrix = (long double**)malloc((num + 1) * sizeof (long double*));
    pass = (int**)malloc((num + 1) * sizeof (int*));
    dist_floyd = (long double**)malloc((num + 1) * sizeof (long double*));
    for(int i = 1;i <= num;i++){
        matrix[i] = (long double*)malloc((num + 1)* sizeof (long double));
        pass[i] = (int*)malloc((num + 1)* sizeof (int));
        dist_floyd[i] = (long double*)malloc((num + 1) * sizeof (long double));
    }
}

long double dijkstra(int start,int end)
{
    // to initial the pointers
    for(int i = 1;i <= num;i++){
        for(int j = 1;j <= num;j++){
            matrix[i][j] = 100000;
        }
    }
    for(int i = 1;i<=sum;i++){
        matrix[Data[i].start][Data[i].end] = Data[i].length;
        matrix[Data[i].end][Data[i].start] = Data[i].length;
    }
    // if the node ID does not exist, exit the program
    if(start > num || end > num){
        printf("The Node does not exist!\n");
        exit(0);
    }
    int *flag;
    flag = malloc(num * sizeof (int));
    // initial the pointer
    for(int i = 1;i <= num; i++){
        dist[i] = 100000;
        flag[i] = 0;
        Node[i].prev = Node[i].ID_normal;
    }
    dist[start]=0;
    for(int i = 0; i <num; i++)
    {
        int t = -1;
        for(int j = 1; j <= num; j++) {
            if (flag[j] == 0 && (t == -1 || dist[t] > dist[j])) {
                t = j;
            }
        }
        flag[t] = 1;
        for(int j = 1; j <= num; j++) {
            if(dist[j] > (dist[t] + matrix[t][j])){
                dist[j] = dist[t] + matrix[t][j];
                Node[j].prev = t;
            }
        }
    }
    // if the route does not exist, return 10000000
    if(dist[end] == 100000) return 10000000;
    // return the best route
    return dist[end];
}

// this is another arithmetic to compute out the best path.
// However, it is too slow thus I just write down invoke code but annotate it.
// If you want to test it, you could free annotation.
// The running time is about 2 minutes.
void floyd() {
    // to initial the pointers
    for(int i = 1;i <= num;i++){
        for(int j = 1;j <= num;j++){
            matrix[i][j] = 100000;
        }
    }
    for(int i = 1;i<=sum;i++){
        matrix[Data[i].start][Data[i].end] = Data[i].length;
        matrix[Data[i].end][Data[i].start] = Data[i].length;
    }
    for(int i = 1;i <= num;i++){
        for(int j = 1;j <= num;j++){
            dist_floyd[i][j] = 100000;
            pass[i][j] = -1;
        }
    }
    for(int i = 1;i <= sum;i++){
        dist_floyd[Data[i].start][Data[i].end] = Data[i].length;
        dist_floyd[Data[i].end][Data[i].start] = Data[i].length;
        pass[Data[i].start][Data[i].end] = 0;
        pass[Data[i].end][Data[i].start] = 0;
    }
    for (int k = 1; k <= num; k++){
        for (int i = 1; i <= num; i++) {
            for (int j = 1; j <= num; j++) {
                if (dist_floyd[i][j] > dist_floyd[i][k] + dist_floyd[k][j]) {
                    dist_floyd[i][j] = dist_floyd[i][k] + dist_floyd[k][j]; // the length of the best path
                    pass[i][j] = k; // the passing node
                }
            }
        }
    }
}

// this function is to list out the beat path of the arithmetic of Floyd
void print(int i,int j)
{
    if(i > num || j > num){
        printf("The Node does not exist!\n");
        exit(0);
    }
    if(i==j) return;
    if(pass[i][j]==0){
        for(int k = 1;k <= num;k++){
            if(i == Node[k].ID_normal){
                i = Node[k].ID_origin;
                break;
            }
        }
        for(int k = 1;k <= num;k++){
            if(j == Node[k].ID_normal){
                j = Node[k].ID_origin;
                break;
            }
        }
        char rem[100];
        sprintf(rem,"%d",i);
        strcat(c_floyd,rem);
        strcat(c_floyd," ---> ");
        sprintf(rem,"%d",j);
        strcat(c_floyd,rem);
        strcat(c_floyd,"\n");
    }
    else{
        print(i,pass[i][j]);
        print(pass[i][j],j);
    }
}

void paint(int start,int end){
    if(start > num || end > num){
        printf("The Node does not exist!\n");
        exit(0);
    }
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    int quit = 0;
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
            "MAP",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            700,
            700,
            SDL_WINDOW_SHOWN
    );
    if(window == NULL){
        printf("Could not create window: %s",SDL_GetError());
        return;
    }
    renderer = SDL_CreateRenderer(window,-1,0);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    // paint all the Node
    for(int i = 1;i <= num;i++){
        SDL_Rect rect = {Node[i].ave_lon,Node[i].ave_lat,2,2};
        SDL_RenderFillRect(renderer,&rect);
    }
    int start2,end2;
    // paint all the edges
    for(int i = 1;i <= sum;i++){
        start2 = Data[i].start;
        end2 = Data[i].end;
        SDL_RenderDrawLine(renderer,Node[start2].ave_lon,Node[start2].ave_lat,Node[end2].ave_lon,Node[end2].ave_lat);
    }
    SDL_SetRenderDrawColor(renderer,255,0,0,255);
    int k = start;
    // paint the best path
    while(k != end){
        SDL_RenderDrawLine(renderer,Node[k].ave_lon,Node[k].ave_lat,Node[Node[k].prev].ave_lon,Node[Node[k].prev].ave_lat);
        k = Node[k].prev;
    }
    int x = 0,y = 0;
    int a = 2,b = 2;
    while(quit == 0){
        SDL_PollEvent(&event);
        switch(event.type){
            case SDL_QUIT:// the quit the map function
                quit = 1;
                break;
            case SDL_MOUSEMOTION:// the moving map function
                if(event.motion.state == SDL_PRESSED){
                    x += event.motion.xrel;
                    y += event.motion.yrel;
                    SDL_SetRenderDrawColor(renderer,0,0,0,255);
                    SDL_RenderClear(renderer);
                    SDL_SetRenderDrawColor(renderer,255,255,255,255);
                    for(int i = 1;i <= num;i++){
                        SDL_Rect rect = {(Node[i].ave_lon + x) * a / b,(Node[i].ave_lat + y) * a / b,2,2};
                        SDL_RenderFillRect(renderer,&rect);
                    }
                    for(int i = 1;i <= sum;i++){
                        start2 = Data[i].start;
                        end2 = Data[i].end;
                        SDL_RenderDrawLine(renderer,(Node[start2].ave_lon + x)* a / b,(Node[start2].ave_lat + y) * a / b,(Node[end2].ave_lon + x) * a / b,(Node[end2].ave_lat + y) * a / b);
                    }
                    SDL_SetRenderDrawColor(renderer,255,0,0,255);
                    k = start;
                    while(k != end){
                        SDL_RenderDrawLine(renderer,(Node[k].ave_lon + x) * a / b,(Node[k].ave_lat + y) * a / b,(Node[Node[k].prev].ave_lon + x) * a / b,(Node[Node[k].prev].ave_lat + y) * a / b);
                        k = Node[k].prev;
                    }
                }
                break;
            case SDL_MOUSEWHEEL:// the mitigation and amplification function
                if(event.wheel.y > 0){
                    a++;
                }
                else if(event.wheel.y < 0){
                    b++;
                }
                if(a == b){
                    a = 1;
                    b = 1;
                }
                SDL_SetRenderDrawColor(renderer,0,0,0,255);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer,255,255,255,255);
                for(int i = 1;i <= num;i++){
                    SDL_Rect rect = {(Node[i].ave_lon + x) * a / b,(Node[i].ave_lat + y) * a / b,2,2};
                    SDL_RenderFillRect(renderer,&rect);
                }
                for(int i = 1;i <= sum;i++){
                    start2 = Data[i].start;
                    end2 = Data[i].end;
                    SDL_RenderDrawLine(renderer,(Node[start2].ave_lon + x) * a / b,(Node[start2].ave_lat + y) * a / b,(Node[end2].ave_lon + x) * a / b,(Node[end2].ave_lat + y) * a / b);
                }
                SDL_SetRenderDrawColor(renderer,255,0,0,255);
                k = start;
                while(k != end){
                    SDL_RenderDrawLine(renderer,(Node[k].ave_lon + x) * a / b,(Node[k].ave_lat + y) * a / b,(Node[Node[k].prev].ave_lon + x) * a / b,(Node[Node[k].prev].ave_lat + y) * a / b);
                    k = Node[k].prev;
                }
                break;
        }
        SDL_RenderPresent(renderer);
    }
    if(renderer){
        SDL_DestroyRenderer(renderer);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}
int onClosing(uiWindow *w, void *data)
{
    uiQuit();
    return 1;
}

void onClicked_Dijkstra(uiButton *b, void *data)
{
    int i;
    fl = 0;
    char c[1000],ex1[100] = "The start node does not exist",ex2[100] = "The end node dose not exist";
    starti = atoi(uiEntryText(s));
    endi = atoi(uiEntryText(e));
    for(i = 1;i <= num;i++){
        if(Node[i].ID_origin == starti){
            startk = Node[i].ID_normal;
            break;
        }
    }
    if(i == num + 1){
        uiLabelSetText(best,ex1);
    }else{
        for(i = 1;i <= num;i++){
            if(Node[i].ID_origin == endi){
                endk = Node[i].ID_normal;
                break;
            }
        }
        if(i == num + 1){
            uiLabelSetText(best,ex2);
        }else{
            long double test = dijkstra(endk,startk);
            if(test != 10000000){
                int k = startk;
                sprintf(c,"%d",Node[startk].ID_origin);
                strcat(c," ---> \n");
                while(Node[k].prev != endk){
                    k = Node[k].prev;
                    char rem[100];
                    sprintf(rem,"%d",Node[k].ID_origin);
                    strcat(c,rem);
                    strcat(c," ---> \n");
                }
                char rem[100];
                sprintf(rem,"%d",endi);
                strcat(c,rem);
                strcat(c,":");
                sprintf(rem,"%Lf",test);
                strcat(c,rem);
                fl = 1;
            }
            else{
                char rem[100];
                sprintf(rem,"%d",starti);
                strcat(c,rem);
                strcat(c," ---> ");
                sprintf(rem,"%d",endi);
                strcat(c,rem);
                strcat(c,":No\n");
            }
            strcat(c,"\nPlease close the GUI to see the map");
            uiLabelSetText(best,c);
        }
    }
}

void onClicked_Floyd(uiButton *b, void *data)
{
    int i;
    fl = 0;
    strcpy(c_floyd,"");
    starti = atoi(uiEntryText(s));
    endi = atoi(uiEntryText(e));
    for(i = 1;i <= num;i++){
        if(Node[i].ID_origin == starti){
            startk = Node[i].ID_normal;
            break;
        }
    }
    if(i == num + 1){
        strcat(c_floyd,"The start node does not exist!");
    }else{
        for(i = 1;i <= num;i++){
            if(Node[i].ID_origin == endi){
                endk = Node[i].ID_normal;
                break;
            }
        }
        if(i == num + 1){
            strcat(c_floyd,"The end node does not exist!");
        }else{
            uiLabelSetText(best,"This arithmetic takes too much time! Please waiting patiently!\n");
            floyd();
            dijkstra(endk,startk);
            print(startk,endk);
            char rem[100];
            sprintf(rem,"%Lf",dist_floyd[startk][endk]);
            strcat(c_floyd,rem);
            strcat(c_floyd,"\nPlease close the GUI to see the map");
            fl = 1;
        }
    }
    uiLabelSetText(best,c_floyd);
}

void GUI(void)
{
    uiInitOptions o;
    const char *err;
    uiWindow *w;
    uiGrid *g;
    uiLabel *start,*end,*path;
    uiButton *d,*f;

    memset(&o, 0, sizeof(uiInitOptions));
    err = uiInit(&o);
    if (err != NULL) {
        puts(err);
        uiFreeInitError(err);
        return;
    }

    // create Window
    w = uiNewWindow("GUI", 500, 240, 0);
    uiWindowSetMargined(w, 1);

    // create Grid
    g = uiNewGrid();
    uiGridSetPadded(g, 1);
    uiWindowSetChild(w, uiControl(g));

    // create Label
    start = uiNewLabel("Start Node:");
    uiGridAppend(g, uiControl(start),
                 0, 2, 2, 1,
                 1, uiAlignCenter, 1, uiAlignFill);
    s = uiNewEntry();
    uiGridAppend(g, uiControl(s),
                 0, 3, 2, 1,
                 1, uiAlignCenter, 1, uiAlignFill);
    end = uiNewLabel("End Node:");
    uiGridAppend(g, uiControl(end),
                 0, 4, 1, 1,
                 1, uiAlignCenter, 1, uiAlignFill);
    e = uiNewEntry();
    uiGridAppend(g, uiControl(e),
                 0, 5, 2, 1,
                 1, uiAlignCenter, 1, uiAlignFill);
    path = uiNewLabel("Best Path:");
    uiGridAppend(g, uiControl(path),
                 0, 6, 2, 1,
                 1, uiAlignCenter, 1, uiAlignFill);
    best = uiNewLabel("");
    uiGridAppend(g, uiControl(best),
                 0, 7, 2, 1,
                 1, uiAlignCenter, 1, uiAlignFill);
    // create the calculate button
    d = uiNewButton("Calculate(Dijkstra)");
    uiButtonOnClicked(d, onClicked_Dijkstra, best);
    uiGridAppend(g, uiControl(d),
                 0, 10, 2, 1,
                 1, uiAlignCenter, 1, uiAlignEnd);
    f = uiNewButton("Calculate(Floyd)");
    uiButtonOnClicked(f, onClicked_Floyd, best);
    uiGridAppend(g, uiControl(f),
                 0, 11, 2, 1,
                 1, uiAlignCenter, 1, uiAlignEnd);
    uiWindowOnClosing(w, onClosing, NULL);
    uiControlShow(uiControl(w));
    uiMain();
}

int main(int argc,char **argv){
    char *file = "Final_Map.map";
    if(argc == 2){
        strcpy(file,argv[1]);
    }
    loadmap(file);
    GUI();
    if(fl == 1) paint(startk,endk);
}