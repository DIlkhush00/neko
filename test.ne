var x = 10;
var y = x * 2 + 5;

function add(a, b) {
    return a + b;
}

if (x <= y) {
    var z = add(x, y);
    print z;
} else {
    x = 0;
}

while (x > 0) {
    print x;
    x = x - 1;
}

print "Done";
print null;
