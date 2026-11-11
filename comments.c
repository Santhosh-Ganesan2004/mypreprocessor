#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

int cmr(char * input)
{
    FILE * in = fopen(input , "r");
    FILE * out= fopen("nocomments.c.tmp", "w");
    if(in == NULL){
        printf("Input file does not exist!!");
        return -1;
    }
    if(out  == NULL)
    {
        printf("Outputfile creation failed!! ");
        fclose(in);
        return -1;
    }
    int ch, flag =0, str =0, single=0;

    while ((ch = fgetc(in)) != EOF)
    {
        int demo = ch;
        if(demo == '"' ||  str ==1)
        {
            if(demo == '"')
            str ^= 1;
            fputc(ch,out);
            continue;

        }
        else if(demo == '\'' || single == 1)
        {
            if(demo == '\'')
            single ^=1;
            fputc(ch,out);
            continue;
        }
        else{
        if(ch == '/')
        {
            int next = fgetc(in);
            
            if( next == '/')
            {
                while((ch = fgetc(in))!= '\n' && ch != EOF);
                if(ch == '\n')
                fputc('\n',out);
            }
            else if (next == '*')
            {
                int last = 0;
                while((ch=fgetc(in))!= EOF){
                if(last == '*' && ch == '/')
                    break;
                last =ch;

                }
                if(ch==EOF)
                    flag = -1;
                fputc('\n',out);
            }
            else
            {
                if(next != EOF)
                    ungetc(next,in);
                
                fputc(ch,out);
            }
        }
        else
        {
            fputc(ch,out);
        }
        }

    }
    fclose(in);
    fclose(out);
    if(flag == -1)
        return -1;
    return 0;

}

int spcrml(char  * input)
{

    FILE * in = fopen(input , "r");
    FILE * out= fopen("stage2.c.tmp", "w");
    if(in == NULL){
        printf("Input file does not exist!!");
        return -1;
    }
    if(out  == NULL)
    {
        printf("Outputfile creation failed!! ");
        fclose(in);
        return -1;
    }

    char line[1000];
    int flag =0;
    while(fgets(line, sizeof(line), in) != NULL)
    {
        flag =0;
        for(int i=0; line[i]; i++)
        {
            if(!isspace(line[i]))
            {
                flag =1;
                break;
            }
        }
        if(flag ==1){
            fputs(line,out);
       } 
    }

    fclose(in);
    fclose(out);
    remove("nocomments.c.tmp");
    return 0;

}
