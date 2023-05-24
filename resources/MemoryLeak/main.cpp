typedef unsigned int size_t;
void *malloc (size_t __size) {return 0;}
void free (void *__ptr) {}
int input() {return 0;}

void test_mismatch() {
    int *x = new int;
    int *t1 = new int;
    float *t2;
    t2 = new float[20];
    delete x;
}

void test_c() {
    int* arr1 = (int*)malloc(sizeof(int));
    free(arr1);
}

void test_shadow() {
    int *x = new int;
    if (input()) {
        x = new int;
    }
    delete x;
}

void test_branch() {
    int *x = new int;
    if (input()) {
        delete x;
    } else {
        *x = 1;
    }
}