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
}node;

float matrix[5000][5000];
data Data[5000];
node Node[5000];
float dist[5000];
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
            }
        }
    }
    if(dist[end] == 100000) return 0;
    return dist[end];
}

int main(void){
    loadmap("Final_Map.map");
    int end,start,end1,start1;
    printf("Please enter the start point:");
    scanf("%i",&start);
    printf("Please enter the end point:");
    scanf("%i",&end);
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
    for(int j = 1;j<=5000;j++){
        double test = dijkstra(start1,end1);
        for(int i = 1;i < 5000;i++){
            if(start == Node[i].ID_normal){
                start = Node[i].ID_origin;
                break;
            }
        }
        for(int i = 1;i < 5000;i++){
            if(end == Node[i].ID_normal){
                end = Node[i].ID_origin;
                break;
            }
        }
        if(test != 0){
            printf("%d ---> %d:%f\n",start,end,test);
        }
        else{
            printf("%d ---> %d:no\n",start,end);
        }
    }
}

