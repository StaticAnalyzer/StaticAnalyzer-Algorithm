/**
 * @class UseBeforeDef
 * @brief Test case for use before define variables.
 */
class UseBeforeDefTest {
    int UseBeforeDefTest1() {
        int a, b, c = a * b + b;
        return c;
    }

    int UseBeforeDefTest2(int n) {
        int x, y;
        if (n > 0) {
            x = 1;
            y = 2;
        } else {
            x = 1 + y;
            y = 2;
        }
        return x + y;
    }

    int UseBeforeDefTest3(int a, int b) {
        int c;
        while (a > b) {
            c = b;
            --a;
        }
        return c;
    }

    int UseBeforeDefTest4(int a, int b, int c) {
        int x;
        if (a > 0) {
            x = a;
        } else {
            x = b;
        }
        int y = x;
        x = c;
        return x;
    }

    static void set1(int &a) {
        a = 1;
    }

    int UseBeforeDefTest5() {
        int a;
        set1(a);
        return a;
    }

    static void valSet1(int a) {
        a = 1;
    }

    int UseBeforeDefTest6() {
        int a;
        valSet1(a);
        return a;
    }

};
