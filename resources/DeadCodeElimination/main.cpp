int f() {return 0;}
int g() {return 0;}

int test () {
    int x = 0;
    int key = 0;
    switch (key) {
    case 0:
        x = 1;
        break;
    case 1:
        key = x + 1;
        break;
    }
    x = f();
    x = g();

    return key + x;
}