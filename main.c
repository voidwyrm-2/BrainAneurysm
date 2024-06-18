#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int Get(int key, int list[][2], int *err)
{
    for (int i = 0; i < (int)sizeof(list); i++)
    {
        if (list[i][0] == key)
        {
            return list[i][1];
        }
    }
    *err = 1;
    return 0;
}

int GetNextOpen(int list[][2])
{
    const int empty[] = {0, 0};
    for (int i = 0; i < (int)sizeof(list); i++)
    {
        if (list[i] == empty)
        {
            return list[i][1];
        }
    }
    return 0;
}

// const int maximumFileLength = 1000;

void interpreter(char text[])
{
    int bytes[30000];
    int pointer = 0;

    int jumpstack[1000];
    int jumpnest = 0;
    int jumps[2000][2];
    // int openingtemp;
    int brackErr = 0;

    for (int i = 0; i < (int)strlen(text); i++)
    {
        if (text[i] == '[')
        {
            jumpnest++;
            jumpstack[jumpnest] = i;
        }
        else if (text[i] == ']' && jumpnest != 0)
        {
            jumpnest--;
            int openingtemp = jumpstack[jumpnest];
            int open = GetNextOpen(jumps);
            jumps[open][0] = openingtemp;
            jumps[open][1] = i;
            if (open + 1 > (int)sizeof(jumps))
            {
                printf("error: jumps overflow\n");
                return;
            }
            jumps[open + 1][0] = i;
            jumps[open + 1][1] = openingtemp;
            jumpstack[jumpnest] = 0;
        }
        else if (text[i] == ']' && jumpnest == 0)
        {
            printf("error: unopened ']'(jumpnest is %d)\n", jumpnest);
            return;
        }
    }

    if (jumpnest != 0)
    {
        printf("error: unclosed '['(jumpnest is %d)\n", jumpnest);
        return;
    }

    int col = 0;
    int ln = 0;
    while (col < (int)strlen(text))
    {
        printf("%d('%c'), ptr: %d, b1: %d, b2: %d, b3: %d\n", col, text[col], pointer, bytes[0], bytes[1], bytes[2]);

        switch (text[col])
        {
        case '+':
            if (bytes[pointer] + 1 > 255)
            {
                bytes[pointer] = 0;
            }
            else
            {
                bytes[pointer]++;
            }
            col++;
            break;
        case '-':
            if (bytes[pointer] - 1 < 0)
            {
                bytes[pointer] = 255;
            }
            else
            {
                bytes[pointer]--;
            }
            col++;
            break;
        case '>':
            if (pointer + 1 > 29999)
            {
                pointer = 0;
            }
            else
            {
                pointer++;
            }
            col++;
            break;
        case '<':
            if (pointer - 1 < 0)
            {
                pointer = 29999;
            }
            else
            {
                pointer--;
            }
            col++;
            break;
        case '.':
            printf("%d(%d)\n", pointer, bytes[pointer]);
            col++;
            break;
        case ',':
            printf("please input a number between 0-255, inclusive\n");
            int *input;
            input = (int *)malloc(sizeof(int));
            if (input == NULL)
            {
                printf("Memory for input was not allocated.\n");
                exit(0);
            }
            scanf("%d", input);
            if (*input > 255)
            {
                bytes[pointer] = 255;
            }
            else if (*input < 0)
            {
                bytes[pointer] = 0;
            }
            else
            {
                bytes[pointer] = *input;
            }

            free(input);

            col++;
            break;
        case '[':
            if (bytes[pointer] == 0)
            {

                int gottenpos = Get(col, jumps, &brackErr);
                if (brackErr != 0)
                {
                    printf("error: missing closing for '[' on line %d, col %d\n", ln, col);
                    return;
                }
                col = gottenpos;
            }
            col++;
            break;
        case ']':
            if (bytes[pointer] != 0)
            {

                int gottenpos = Get(col, jumps, &brackErr);
                if (brackErr != 0)
                {
                    printf("error: missing opening for ']' on line %d, col %d\n", ln, col);
                    return;
                }
                col = gottenpos;
            }
            col++;
            break;
        case '\n':
            ln++;
        default:
            col++;
            break;
        }
    }
}

int main()
{
    char code[200];
    printf("Input code here(max 200 characters)\n");
    fgets(code, 200, stdin);
    interpreter(code);
    return 0;
}
