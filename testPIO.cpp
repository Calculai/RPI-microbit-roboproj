#include <microbit.h>

MicroBit uBit;

int main() {
    uBit.init();
    while (true) {
        uBit.display.scroll("Hello, World!");
        uBit.sleep(1000);
    }
}