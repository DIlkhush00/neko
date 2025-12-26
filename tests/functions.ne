function square(n) {
    return n * n;
}

function add_three(a, b, c) {
    return a + b + c;
}

var s = square(5);
print s;

var sum = add_three(1, 2, 3);
print sum;

print square(add_three(1, 1, 1));
