#include <stdio.h>
#include <string.h>
int main(){
    char a1[32], a2[32], b1[32], b2[32], b3[32], c[32];
    strcpy(a1, "0723");
    strcpy(a2, "00");
    strcpy(b1, "0x231");
    strcpy(b2, "0X231");
    strcpy(b3, "0x0");
    strcpy(c, "1.05e3");
    int v1, v2, v3, v4, v5;
    float v6;

    sscanf(a1, "%o", &v1);
    sscanf(a2, "%o", &v2);
    sscanf(b1, "%x", &v3);
    sscanf(b2, "%x", &v4);
    sscanf(b3, "%x", &v5);
    sscanf(c, "%f", &v6);

    printf("%s has the value: %d \n", a1, v1);
    printf("%s has the value: %d \n", a2, v2);
    printf("%s has the value: %d \n", b1, v3);
    printf("%s has the value: %d \n", b2, v4);
    printf("%s has the value: %d \n", b3, v5);
    printf("%s has the value: %f \n", c, v6);
    return 0;
}