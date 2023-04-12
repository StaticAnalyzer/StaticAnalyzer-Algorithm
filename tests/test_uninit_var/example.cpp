int foo(int n) {
    return n;
}

int main() {
    // Unititialized variable
    int x;
    foo(x);

    // Unititialized variable

    for(int i = 0; i < 5; i ++) {
        int y;
        foo(y);
    }

    int z;
    z = 1;
    foo(z);

    int u;
    if(z < 0)
        u = 1;
    else
        u = 0;
    foo(u);

    // Unititialized variable, but not detected

    int v;
    if(z < 0)
        v = 1;
    foo(v);

    return 0;
}
