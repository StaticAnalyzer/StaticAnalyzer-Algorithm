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
    int g = foo(1);
    int h = foo(1 / (g & 0x0));
    int i = foo(1 / (g ^ g));
    return 0;
}