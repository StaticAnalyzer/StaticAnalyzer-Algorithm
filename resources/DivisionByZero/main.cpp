int foo(int n) {
    return n;
}

int main() {
    int a = 17, b = 7;
    int zero = a - (a / b * b) - (a % b);
    int c = a / zero;
    int d = foo(4 % zero);
    int e = 5;
    e /= zero;
    int f[12];
    f[6 / zero] = 1;
    return 0;
}