#include <stdio.h>
int main()
{
    int ch;
    int cnt = 0;
    printf("\"");
    while ((ch = getchar()) != EOF) {
        printf("\\x%02X", ch);
        if (++cnt % 10 == 0) printf("\"\n\"");
    }
    printf("\"\n");
    return 0;
}
