
// Single-lin
#include<stdio.h>
#include"demoheader.h"
#define PI 3.14
#define MAX 100
#define SUM(a,b) ((a) + (b))
#define SQUARE(x) \
((x) * (x))

/* Multiline comment
   Testing comment removal
*/

int main()
{
    int x = 10;
    int y = 20;

    float radius = 5.0;
    float area = PI * radius * radius;

    int limit = MAX;

    int result1 = SUM(x, y);
    int result2 = SQUARE(5);

    int result3 = SUM(10, 20);

    char ch = 'A';
    char *str = "Hello // World";

    /* This should disappear */

    printf("Area = %f\n", area);
    printf("Limit = %d\n", limit);
    printf("Sum = %d\n", result1);
    printf("Square = %d\n", result2);
    printf("Sum = %d\n", result3);

    return 0;
}

