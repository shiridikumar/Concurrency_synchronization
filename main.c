#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
struct courses{
    char course_name[200];
    float interest;
    int max_slots;
    int num_labs;
    int lab_ids[1000];
};
int split(char *c, char **arr)
{
    int coun = 0;
    int prev = 0;
    for (int i = 0; i < strlen(c); i++)
    {
        if (c[i] == ' ')
        {
            strncpy(arr[coun], c + prev, i - prev);
            prev = i + 1;
            coun++;
        }
    }
    strncpy(arr[coun], c + prev, strlen(c) - prev);
    coun++;
    return coun;
}
int main()
{
    char *inp;
    inp = (char *)malloc(1000 * sizeof(char));
    int num_students,num_labs,num_courses;
    char* startline;
    startline=(char *)malloc(1000*sizeof(char));
    gets(startline);
    char **arr2;
    arr2=(char **)malloc(1000*sizeof(char *));
    for(int i=0;i<1000;i++){
        arr2[i]=(char *)malloc(1000*sizeof(char));
    }
    int args=split(startline,arr2);
    sscanf(arr2[0],"%d",&num_students);
    sscanf(arr2[1],"%d",&num_labs);
    sscanf(arr2[2],"%d",&num_courses);
    struct courses cs[num_courses];
    for(int i=0;i<num_courses;i++){
        scanf("%s %f %d %d",cs[i].course_name,&cs[i].interest,&cs[i].max_slots,&cs[i].num_labs);
        for(int j=0;j<cs[i].num_labs;j++){
            scanf("%d",&cs[i].lab_ids[j]);
        }
        
    }
    for(int i=0;i<num_courses;i++){
        printf("%s %f %d %d ",cs[i].course_name,cs[i].interest,cs[i].max_slots,cs[i].num_labs);
        for(int j=0;j<cs[i].num_labs;j++){
            printf("%d ",cs[i].lab_ids[j]);
        }
        printf("\n");
        
    }
}