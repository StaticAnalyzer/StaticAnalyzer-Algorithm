/**
 * @class UnitVar
 * @brief Test case for unitialized variables.
 */
class UnitVar {
    int unitVarTest1() {
        int a, b, c = a * b + b;
        return c;
    }

    int unitVarTest2(int n) {
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

    int unitVarTest3(int a, int b, int c) {
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

    int unitVarTest4(int a, int b) {
        int c;
        while (a > b) {
            c = b;
            --a;
        }
        return c;
    }

};
