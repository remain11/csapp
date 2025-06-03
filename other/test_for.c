#include<stdio.h>

void test() {

}

void test1() {
    int i = 0;
    i++;
}

int main(){
    for(;;) {
        test();
        test1();
    }
    return 0;
}