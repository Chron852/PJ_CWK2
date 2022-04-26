#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct data
{
    int start;
    int end;
    float length;
}data;

typedef struct node{
    int ID_origin;//ID_origin is to store nodes that have not been reassigned
    int ID_normal;// ID_normal is to store nodes that have been reassigned
    float lat;
    float lon;
    int prev;
}node;

float matrix[5000][5000];
data Data[5000];
node Node[5000];
float dist[5000],dist_floyd[5000][5000];
int pass[5000][5000];
int sum = 0,num = 0;//sum is the number of edges and num is the number of nodes

void loadmap(char *filename){
    FILE *f = fopen(filename,"r");
    char buf[1024];
    char a1[100],a2[100],a3[100],a4[100],a5[100],a6[100],a7[100],a8[100],a9[100],a10[100],a11[100],a12[100];
    while(fgets(buf,1024,f)){
        if(strstr(buf,"link id")){
            sscanf(buf,"<link id=%s node=%s node=%s way=%s length=%s veg=%s arch=%s land=%s POI=%s;/link>",&a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9);
            sum++;
            Data[sum].start = atoi(a2);
            Data[sum].end = atoi(a3);
            Data[sum].length = atof(a5);
        }
    }
    fclose(f);
    f = fopen(filename,"r");
    while(fgets(buf,1024,f)){
        if(strstr(buf,"node id")){
            sscanf(buf,"<node id=%s lat=%s lon=%s /node>",&a10,&a11,&a12);
            num++;
            Node[num].ID_origin = atoi(a10);
            Node[num].lat = atof(a11);
            Node[num].lon = atof(a12);
        }
    }
    fclose(f);
    int tmp;
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
            float tempp = Node[i].lat;
            Node[i].lat = Node[tmp].lat;
            Node[tmp].lat = tempp;
            tempp = Node[i].lon;
            Node[i].lon = Node[tmp].lon;
            Node[tmp].lon = tempp;
        }
    }
    for(int i = 1;i <= num;i++){
        Node[i].ID_normal = i;
    }
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
    for(int i = 1;i < 5000;i++){
        for(int j = 1;j < 5000;j++){
            matrix[i][j] = 100000;
        }
    }
    for(int i = 1;i<=sum;i++){
        matrix[Data[i].start][Data[i].end] = Data[i].length;
        matrix[Data[i].end][Data[i].start] = Data[i].length;
    }
}

float dijkstra(int start,int end)
{
    int flag[5000];
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
    if(dist[end] == 100000) return 10000000;
    return dist[end];
}

// this is another arithmetic to compute out the best path.
// However, it is too slow thus I just write down invoke code but annotate it.
// If you want to test it, you could free annotation.
// The running time is about 2 minutes.
void floyd() {
    for(int i = 1;i < 5000;i++){
        for(int j = 1;j < 5000;j++){
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
                    dist_floyd[i][j] = dist_floyd[i][k] + dist_floyd[k][j];
                    pass[i][j] = k;
                }
            }
        }
    }
}

// this function is to list out the beat path of the arithmetic of Floyd
void print(int i,int j)
{
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
        printf("%d->%d\n",i,j);
    }
    else{
        print(i,pass[i][j]);
        print(pass[i][j],j);
    }
}

int main(void){
    loadmap("Final_Map.map");
    int end,start,end1 = 0,start1 = 0;
    printf("Please enter the start point:");
    scanf("%d",&start);
    printf("Please enter the end point:");
    scanf("%d",&end);
    start = 1615404345;
    for(int i = 1;i < 5000;i++){
        if(Node[i].ID_origin == start){
            start1 = Node[i].ID_normal;
            break;
        }
    }
    for(int i = 1;i < 5000;i++){
        if(Node[i].ID_origin == end){
            end1 = Node[i].ID_normal;
            break;
        }
    }
    double test = dijkstra(start1,end1);
    printf("Dijkstra:\n");
    if(test != 0){
        int k = end1;
        printf("%d <--- ",Node[end1].ID_origin);
        while(Node[k].prev != start1){
            k = Node[k].prev;
            printf("%d <--- ",Node[k].ID_origin);
        }
        printf("%d:%f\n",start,test);
    }
    else{
        printf("%d ---> %d:no\n",start,end);
    }
//    printf("Floyd:\n");
//    floyd();
//    printf("%f\n",dist_floyd[start1][end1]);
//    print(start1,end1);
}

