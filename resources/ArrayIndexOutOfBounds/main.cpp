int test() {
    int a[10][10][10];
    int i = 10, j = 2, k = 1;
    a[i][i + j][k] = 1;
    return a[i][k][k - j];
}
