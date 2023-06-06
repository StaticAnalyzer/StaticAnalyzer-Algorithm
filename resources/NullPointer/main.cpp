int input() {return 0;}

void test_basic() {
    int* p = nullptr;
    *p = 1;
    int *q;
    q = 0;
    q[2] = 1;
}

void test_branch() {
    int *p;
    if (input()) {
        p = nullptr;
    } else {
        p = new int;
    }
    *p = 1;

    int *q;
    if (input()) {
        q = new int;
    } else {
        q = p;
    }
    *q = 1;
}