#include "headers.h"
int num_students,num_labs,num_courses;
pthread_mutex_t student_mutex;
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

int student_filled(int i){
    printf("student %d has filled in preferences for course registration\n",i);
    int pref_1=st[i].pref_1;
    
}

int fill_slots(){
    pthread_t stu[num_students];
    for(int i=0;i<num_students;i++){
        pthread_create(&stu[i],NULL,student_filled,(i));

    }

    for(int i=0;i<num_students;i++){
        pthread_join(stu[i],NULL);
    }


}

int main()
{
    char *inp;
    inp = (char *)malloc(1000 * sizeof(char));
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
    cs=(courses *)malloc(num_courses*sizeof(courses));
    st=(students *)malloc(num_students*sizeof(students));
    lb=(labs *)malloc(num_labs*sizeof(labs));
    


    // courses input

    for(int i=0;i<num_courses;i++){
        scanf("%s %f %d %d",cs[i].course_name,&cs[i].interest,&cs[i].max_slots,&cs[i].num_labs);
        for(int j=0;j<cs[i].num_labs;j++){
            scanf("%d",&cs[i].lab_ids[j]);
        }
        
    }

    //students input
    for(int i=0;i<num_students;i++){
        scanf("%f %d %d %d %f",&st[i].calibre,&st[i].pref_1,&st[i].pref_2,&st[i].pref_3,&st[i].time);
    }

    //labs input
    for(int i=0;i<num_labs;i++){
        scanf("%s %d %d",lb[i].lab_name,&lb[i].num_ta,&lb[i].max_num);
    }
    fill_slots();

}