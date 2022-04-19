#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <xmath.h>

typedef struct data
{
    int start;
    int end;
    double length;
}data;

double matrix[5000][5000];
data Data[5000];
int prev[5000];
double dist[5000];
int ID_origin[5000],ID_normal[5000];//ID_origin is to store nodes that have not been reassigned
// ID_normal is to store nodes that have been reassigned
int sum = 0,num = 0;//sum is the number of edges and num is the number of nodes

void loadmap(char *filename){
    FILE *f = fopen(filename,"r");
    char buf[1024];
    char a1[100],a2[100],a3[100],a4[100],a5[100],a6[100],a7[100],a8[100],a9[100],a10[100],a11[100],a12[100];
    while(fgets(buf,1024,f)){
        if(strstr(buf,"link id")){
            sscanf(buf,"<link id=%s node=%s node=%s way=%s length=%s veg=%s arch=%s land=%s POI=%s;/link>",&a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&a9);
            Data[sum].start = atoi(a2);
            Data[sum].end = atoi(a3);
            Data[sum].length = atof(a5);
            sum++;
        }
    }
    fclose(f);
    f = fopen(filename,"r");
    while(fgets(buf,1024,f)){
        if(strstr(buf,"node id")){
            sscanf(buf,"<node id=%s lat=%s lon=%s /node>",&a10,&a11,&a12);
            ID_origin[num] = atoi(a10);
            num++;
        }
    }
    fclose(f);
    int tmp;
    for(int i = 0;i<num;i++){
        tmp = i;
        for(int j = i + 1;j<num;j++){
            if(ID_origin[j]<ID_origin[tmp]){
                tmp = j;
            }
        }
        if(tmp != i){
            int temp = ID_origin[i];
            ID_origin[i] = ID_origin[tmp];
            ID_origin[tmp] = temp;
        }
    }
    for(int i = 0;i < num;i++){
        ID_normal[i] = i;
    }
    for(int i = 0;i < sum;i++){
        for(int j = 0;j < num;j++){
            if(Data[i].start == ID_origin[j]){
                Data[i].start = ID_normal[j];
            }
            if(Data[i].end == ID_origin[j]){
                Data[i].end = ID_normal[j];
            }
        }
    }
    for(int i = 0;i < 5000;i++){
        for(int j = 0;j < 5000;j++){
            matrix[i][j] = INF;
        }
    }
    for(int i = 0;i<sum;i++){
        matrix[Data[i].start][Data[i].end] = Data[i].length;
        matrix[Data[i].end][Data[i].start] = Data[i].length;
    }
    /*for(int i = 0;i < 5000;i++){
        for(int j = 0;j < 5000;j++){
            if(matrix[i][j] != 0) {
                printf("%d\t%d\t%f\n",i,j,matrix[i][j]);
            }
        }
    }*/
}


void dijkstra(int vs) {
    double min,tmp;
    int k = 0;
    int flag[5000];
     for (int i = 0; i < num; i++) {
        flag[i] = 0;
        prev[i] = 0;
        dist[i] = matrix[vs][i];
    }
    flag[vs] = 1;
    dist[vs] = 0;
    for (int i = 1; i < num; i++) {
        min = INF;
        for (int j = 0; j < num; j++) {
            if (flag[j] == 0 && dist[j] < min) {
                min = dist[j];
                k = j;
            }
        }
        flag[k] = 1;
        for (int j = 0; j < num; j++) {
            if(matrix[k][j] == 0){
                tmp = INF;
            } else{
                tmp = min + matrix[k][j];
            }
            if (flag[j] == 0 && (tmp < dist[j])) {
                dist[j] = tmp;
                prev[j] = k;
            }
        }
    }
}

int main(void){
    loadmap("Final_Map.map");
    printf("dijkstra(%d): \n", ID_normal[0]);
    dijkstra(0);
    for (int i = 0; i < num; i++){
        if(dist[i] != INF) {
            printf("  shortest(%d, %d)=%f\n", ID_normal[0], ID_normal[i], dist[i]);
        }
    }
}

