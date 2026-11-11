#include<stdio.h>
#include<string.h>

int headeradd(char *, FILE *);
int sysheaderadd(char *, FILE *);
int headerfind(char *);
int predefinedheader(char *);


int predefinedheader(char *name)
{
    char *headers[] = {
        "stdio.h",
        "stdlib.h",
        "string.h",
        "math.h",
        "ctype.h",
        "stdbool.h",
        "limits.h",
        "float.h",
        "time.h",
    };

    for(int i = 0; i < 9; i++)
    {
        if(strcmp(name, headers[i]) == 0)
            return 1;
    }

    return 0;
}


int sysheaderadd(char *line, FILE *out)
{
    int f = 0, j = 0;
    char s[100];
    char path[500];

    for(int i = 0; line[i] != '\0'; i++)
    {
        if(line[i] == '<' || f == 1)
        {
            if(line[i] != '<' && line[i] != '>')
            {
                if(j >= 99)
                    return -1;

                s[j] = line[i];
                j++;
            }

            if(line[i] == '<')
                f = 1;

            if(line[i] == '>')
            {
                f = 0;
                break;
            }
        }
    }

    s[j] = '\0';

    if(f == 1 || j == 0)
    {
        printf("System header name is not proper!!\n\n");
        return -1;
    }

    char *dirs[] = {
        "/usr/lib/gcc/x86_64-linux-gnu/15/include",
        "/usr/local/include",
        "/usr/include/x86_64-linux-gnu",
        "/usr/include"
    };

    FILE *header = NULL;

    for(int i = 0; i < 4; i++)
    {
        strcpy(path, dirs[i]);
        strcat(path, "/");
        strcat(path, s);

        

        header = fopen(path, "r");

        if(header != NULL)
            break;
    }

    if(header == NULL)
    {
        printf("System header file not found!!\n\n");
        return -1;
    }

    char line2[1000];

    while(fgets(line2, sizeof(line2), header) != NULL)
    {
        fputs(line2, out);
    }

    fclose(header);

    return 0;
}


int headeradd(char *line, FILE *out)
{
    int f = 0, j = 0;
    char s[100];

    for(int i = 0; line[i] != '\0'; i++)
    {
        if(line[i] == '"' || f == 1)
        {
            if(line[i] != '"')
            {
                if(j >= 99)
                    return -1;

                s[j] = line[i];
                j++;
            }

            if(line[i] == '"')
                f ^= 1;
        }
    }

    s[j] = '\0';

    if(f == 1 || j == 0)
    {
        printf("Header file name is not proper!! terminating process!\n\n");
        return -1;
    }

    FILE *header = fopen(s, "r");

    if(header == NULL)
    {
        printf("File opening crashed!!\n\n");
        return -1;
    }

    char line2[1000];

    while(fgets(line2, sizeof(line2), header) != NULL)
    {
        if(strstr(line2, "#include"))
        {
            int stuts;

            if(strchr(line2, '<'))
                stuts = sysheaderadd(line2, out);
            else
                stuts = headeradd(line2, out);

            if(stuts == -1)
            {
                printf("Header Extraction failure!!\n\n");
                fclose(header);
                return -1;
            }
        }
        else
        {
            fputs(line2, out);
        }
    }

    fclose(header);

    return 0;
}


int headerfind(char *input)
{
    FILE *in = fopen(input, "r");
    FILE *out = fopen("stage1.c.tmp", "w");

    if(in == NULL)
    {
        printf("Stage1 file not opened successfully\n\n");
        return -1;
    }

    if(out == NULL)
    {
        printf("Output file creation failed\n\n");
        fclose(in);
        return -1;
    }

    char line[1000];

    while(fgets(line, sizeof(line), in) != NULL)
    {
        if(strstr(line, "#include"))
        {
            int sts;

            if(strchr(line, '<'))
                sts = sysheaderadd(line, out);
            else
                sts = headeradd(line, out);

            if(sts == -1)
            {
                printf("Header file expansion failed!!\n\n");
                fclose(in);
                fclose(out);
                return -1;
            }
        }
        else
        {
            fputs(line, out);
        }
    }

    fclose(in);
    fclose(out);

    printf("Quoted headers added successfully!!\n\n");

    return 0;
}

