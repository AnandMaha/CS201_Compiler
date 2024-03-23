#include <stdio.h>
void example_function(int x) {
    if (x > 0) {
        printf("Positive\n");
    } else if (x < 0) {
        printf("Negative\n");
    } else {
        printf("Zero\n");
    }
}
int main() {
    int a = 5;
    int b = -2;
    if (a > b) {
        example_function(a);
    } else {
        example_function(b);
    }
    return 0;
}
