int returnInt() {return 0;}

int main() {
    int constVal = ((3 * 9) / (1 + 2)) % 4;
    int a = 0;
    while ((a + (1 + returnInt())) * 128 > 0) {
        if (a / 16) {
            int b = returnInt() % 8;
            int c = 256 * b;
        }
        a--;
    }
}