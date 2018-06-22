#include <stdio.h>
#include <stdlib.h>
// test program:
// unsigned
// simple loop
// function call -> top
// if constraint with two constraints
// switch
// switch with case 6: bottom
// switch with case 12: fallthrough
// switch with default
// modulo on top


int main(int argc, char const *argv[]) {
    unsigned a = 0;
    unsigned b = 12;

    while (a < b) {
        a+=4;
        b-=2;
    }

    unsigned c = rand();
    if(a>6 && b<6){
        switch (a) {
            case 6:
                b = 99;
                break;
            case 12:
            case 13:
                b = a*2;
                break;
            default:
                c = c%18;
        }
    } else {
        a = 88;
    }

    printf("%d\n", a);
    printf("%d\n", b);
    printf("%d\n", c);
}