int fib(int n) {
    int i = 0, j = 1;
    while (n > 0) {
        int tmp = i;
        i = j;
        j = i + tmp;
        n--;
    }
    return i;
}

int main() {
    int x;
    for (int i = 0; i < 10; i++) {
        x = fib(i);
    }
    return 0;
}
