#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int cmr(char *);
int spcrml(char *);
int headeradd(char * , FILE *);
int headerfind(char *);
int macroexpand(char *);
int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        printf("Usage : ./a.out fnamwe");
        return -1;
    }

    int head = headerfind(argv[1]);
    if(head != 0)
    {
        printf("Header Extraction Failed!!\n\n");
        return -1;
    }
    int comments = cmr("stage1.c.tmp");
    if(comments != 0)
    {
        printf("comment removal failed!!\n\n");
        return -1;

    }
    int space = spcrml("nocomments.c.tmp");
    if(space != 0)
    {
        printf("Empty space removal failed!!\n\n");
        return -1;
    }
    int macro = macroexpand("stage2.c.tmp");
    if(macro != 0)
    {
        printf("Macros Expansion failed!!\n\n");
        return -1;
    }


    if(space == 0 && comments ==0 && head ==0 && macro == 0)
    {
        remove("stage1.c.tmp");
        remove("nocomments.c.tmp");
        remove("stage2.c.tmp");
    }
    printf("Preprocessing was successful !!\n\n");
    return 0;

}
