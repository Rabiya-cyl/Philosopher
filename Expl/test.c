#include <stdio.h>


typedef struct s_student
{
    char  name[20];   
    int   age;        
    float grade;      
}   t_student;

int main(void)
{
    t_student s1; 
    strcpy(s1.name, "Rabiya");
    s1.age = 19;
    s1.grade = 2.5

    printf("Le nom est %s, Son age est de %i, la note de %f", s1.name, s1.age, s1.grade);

    return 0;
}