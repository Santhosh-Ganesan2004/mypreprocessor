
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define MAX_MACROS 100
#define MAX_NAME 100
#define MAX_VALUE 500
#define MAX_PARAMS 10

struct macro
{
    char name[MAX_NAME];
    char value[MAX_VALUE];

    char params[MAX_PARAMS][MAX_NAME];

    int param_count;
    int type;
};


int macrofind(char *name, struct macro macros[], int count)
{
    for(int i = 0; i < count; i++)
    {
        if(strcmp(name, macros[i].name) == 0)
            return i;
    }

    return -1;
}

int macrogetline(FILE *in, char *line, int size)
{
    char temp[4096];
    int len;

    if(fgets(line, size, in) == NULL)
        return 0;

    while(1)
    {
        len = strlen(line);

        if(len < 2)
            break;

        if(line[len-2] == '\\' && line[len-1] == '\n')
        {
            line[len-2] = '\0';

            if(fgets(temp, sizeof(temp), in) == NULL)
                return -1;

            if(strlen(line) + strlen(temp) >= size)
                return -1;

            strcat(line, temp);
        }
        else
            break;
    }

    return 1;
}

int macrodefine(char *line, struct macro macros[], int *count)
{
    int i = 0, j = 0;

    while(isspace((unsigned char)line[i]) && line[i] != '\n')
        i++;

    if(line[i] != '#')
        return 0;

    i++;

    while(isspace((unsigned char)line[i]) && line[i] != '\n')
        i++;

    if(strncmp(&line[i], "define", 6) != 0)
        return 0;

    i += 6;

    if(!isspace((unsigned char)line[i]))
        return 0;

    while(isspace((unsigned char)line[i]))
        i++;

    char name[MAX_NAME];
    char value[MAX_VALUE];
    char params[MAX_PARAMS][MAX_NAME];

    int type = 0;
    int param_count = 0;

    if(!isalpha((unsigned char)line[i]) && line[i] != '_')
        return -1;

    while(isalnum((unsigned char)line[i]) || line[i] == '_')
    {
        if(j >= MAX_NAME - 1)
            return -1;

        name[j] = line[i];
        j++;
        i++;
    }

    name[j] = '\0';

    if(line[i] == '(')
    {
        type = 1;
        i++;

        while(isspace((unsigned char)line[i]))
            i++;

        if(line[i] != ')')
        {
            while(1)
            {
                if(param_count >= MAX_PARAMS)
                    return -1;

                j = 0;

                if(!isalpha((unsigned char)line[i]) && line[i] != '_')
                    return -1;

                while(isalnum((unsigned char)line[i]) || line[i] == '_')
                {
                    if(j >= MAX_NAME - 1)
                        return -1;

                    params[param_count][j] = line[i];
                    j++;
                    i++;
                }

                params[param_count][j] = '\0';
                param_count++;

                while(isspace((unsigned char)line[i]))
                    i++;

                if(line[i] == ')')
                    break;

                if(line[i] != ',')
                    return -1;

                i++;

                while(isspace((unsigned char)line[i]))
                    i++;
            }
        }

        i++;
    }
    else
    {
        if(line[i] != '\0' && line[i] != '\n' &&
           !isspace((unsigned char)line[i]))
            return -1;
    }

    while(isspace((unsigned char)line[i]))
        i++;

    j = 0;

    while(line[i] != '\0' && line[i] != '\n' && line[i] != '\r')
    {
        if(j >= MAX_VALUE - 1)
            return -1;

        value[j] = line[i];
        j++;
        i++;
    }

    value[j] = '\0';

    int pos = macrofind(name, macros, *count);

    if(pos == -1)
    {
        if(*count >= MAX_MACROS)
            return -1;

        pos = *count;
        (*count)++;
    }

    strcpy(macros[pos].name, name);
    strcpy(macros[pos].value, value);

    macros[pos].type = type;
    macros[pos].param_count = param_count;

    for(int k = 0; k < param_count; k++)
        strcpy(macros[pos].params[k], params[k]);

    return 1;
}

int macrofunctionexpand(char *line, int *i, FILE *out,
                        struct macro macros[], int pos)
{
    int k = *i;
    int j = 0, ac = 0, depth = 0;
    int quote = 0;

    char args[MAX_PARAMS][MAX_VALUE];

    while(isspace((unsigned char)line[k]))
        k++;

    if(line[k] != '(')
        return 0;

    k++;

    while(isspace((unsigned char)line[k]))
        k++;

    if(line[k] == ')' && macros[pos].param_count == 0)
    {
        ac = 0;
    }
    else
    {
        while(1)
        {
            if(ac >= MAX_PARAMS)
                return -1;

            j = 0;
            depth = 0;
            quote = 0;

            while(line[k] != '\0')
            {
                if(quote != 0)
                {
                    if(j >= MAX_VALUE - 1)
                        return -1;

                    args[ac][j++] = line[k];

                    if(line[k] == '\\' && line[k+1] != '\0')
                    {
                        k++;

                        if(j >= MAX_VALUE - 1)
                            return -1;

                        args[ac][j++] = line[k];
                    }
                    else if(line[k] == quote)
                        quote = 0;

                    k++;
                    continue;
                }

                if(line[k] == '"' || line[k] == '\'')
                    quote = line[k];

                if(line[k] == '(')
                    depth++;

                if(line[k] == ')')
                {
                    if(depth == 0)
                        break;

                    depth--;
                }

                if(line[k] == ',' && depth == 0)
                    break;

                if(j >= MAX_VALUE - 1)
                    return -1;

                args[ac][j] = line[k];
                j++;
                k++;
            }

            args[ac][j] = '\0';
            ac++;

            if(line[k] == ')')
                break;

            if(line[k] == ',')
            {
                k++;
                continue;
            }

            return -1;
        }
    }

    if(line[k] != ')')
        return -1;

    k++;

    if(ac != macros[pos].param_count)
        return -1;

    int v = 0;

    while(macros[pos].value[v] != '\0')
    {
        if(macros[pos].value[v] == '"' ||
           macros[pos].value[v] == '\'')
        {
            char q = macros[pos].value[v];

            fputc(macros[pos].value[v], out);
            v++;

            while(macros[pos].value[v] != '\0')
            {
                fputc(macros[pos].value[v], out);

                if(macros[pos].value[v] == '\\' &&
                   macros[pos].value[v+1] != '\0')
                {
                    v++;
                    fputc(macros[pos].value[v], out);
                    v++;
                    continue;
                }

                if(macros[pos].value[v] == q)
                {
                    v++;
                    break;
                }

                v++;
            }

            continue;
        }

        if(isalpha((unsigned char)macros[pos].value[v]) ||
           macros[pos].value[v] == '_')
        {
            char token[MAX_NAME];
            int t = 0;

            while(isalnum((unsigned char)macros[pos].value[v]) ||
                  macros[pos].value[v] == '_')
            {
                if(t >= MAX_NAME - 1)
                    return -1;

                token[t] = macros[pos].value[v];
                t++;
                v++;
            }

            token[t] = '\0';

            int found = 0;

            for(int p = 0; p < macros[pos].param_count; p++)
            {
                if(strcmp(token, macros[pos].params[p]) == 0)
                {
                    fputs(args[p], out);
                    found = 1;
                    break;
                }
            }

            if(found == 0)
                fputs(token, out);
        }
        else
        {
            fputc(macros[pos].value[v], out);
            v++;
        }
    }

    *i = k;

    return 1;
}


int macroexpandline(char *line, FILE *out,
                    struct macro macros[], int count)
{
    int i = 0;

    while(line[i] != '\0')
    {

        if(line[i] == '"' || line[i] == '\'')
        {
            char quote = line[i];

            fputc(line[i], out);
            i++;

            while(line[i] != '\0')
            {
                fputc(line[i], out);

                if(line[i] == '\\' && line[i+1] != '\0')
                {
                    i++;
                    fputc(line[i], out);
                    i++;
                    continue;
                }

                if(line[i] == quote)
                {
                    i++;
                    break;
                }

                i++;
            }

            continue;
        }

        if(isalpha((unsigned char)line[i]) || line[i] == '_')
        {
            char token[1000];
            int j = 0;

            while(isalnum((unsigned char)line[i]) || line[i] == '_')
            {
                if(j >= 999)
                    return -1;

                token[j] = line[i];
                j++;
                i++;
            }

            token[j] = '\0';

            int pos = macrofind(token, macros, count);

            if(pos != -1)
            {
                if(macros[pos].type == 0)
                    fputs(macros[pos].value, out);

                else
                {
                    int sts = macrofunctionexpand(line, &i, out,
                                                  macros, pos);

                    if(sts == 0)
                        fputs(token, out);

                    if(sts == -1)
                        return -1;
                }
            }
            else
                fputs(token, out);
        }
        else
        {
            fputc(line[i], out);
            i++;
        }
    }

    return 0;
}

int macroexpand(char *input)
{
    FILE *in = fopen(input, "r");

    if(in == NULL)
    {
        printf("Macro input file opening failed!!\n\n");
        return -1;
    }

    char output[1000];

    if(strlen(input) >= sizeof(output) - 3)
    {
        printf("Input filename too long!!\n\n");
        fclose(in);
        return -1;
    }

    strcpy(output, input);

    char *dot = strrchr(output, '.');
    char *slash = strrchr(output, '/');

    if(dot != NULL && (slash == NULL || dot > slash))
        strcpy(dot, ".i");
    else
        strcat(output, ".i");

    FILE *out = fopen(output, "w");

    if(out == NULL)
    {
        printf("Macro output file creation failed!!\n\n");
        fclose(in);
        return -1;
    }

    struct macro macros[MAX_MACROS];

    int count = 0;
    int sts;

    char line[4096];

    while((sts = macrogetline(in, line, sizeof(line))) == 1)
    {
        int def = macrodefine(line, macros, &count);

        if(def == -1)
        {
            printf("Macro definition extraction failed!!\n\n");
            fclose(in);
            fclose(out);
            return -1;
        }

        if(def == 1)
            continue;

        int exp = macroexpandline(line, out, macros, count);

        if(exp == -1)
        {
            printf("Macro expansion failed!!\n\n");
            fclose(in);
            fclose(out);
            return -1;
        }
    }

    if(sts == -1)
    {
        printf("Multiline macro processing failed!!\n\n");
        fclose(in);
        fclose(out);
        return -1;
    }

    fclose(in);
    fclose(out);

    printf("Macro expansion completed successfully!!\n");
    printf("Output file: %s\n\n", output);

    return 0;
}

