#include <stdlib.h>
#include <stdio.h>

int main() {
    int *p = (int *) mymalloc(sizeof(int));
    *p = 5;
    printf("p is %p and *p is %d\n", p, *p);
    myfree(p);
}
