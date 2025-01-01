//
//  main.swift
//  myswift
//
//  Created by 李昕阳 on 31/12/2024.
//

import Foundation

var a = 10
var b:Int = 20

//aa = 30
if a < 20 {
    print("a < 20")
}else {
    print("a >= 20")
}
switch a {
case 10: print("a == 10")
case 20: print("a == 20")
default: print("others")
}
// 不用写break
print("Hello, World!", a, b)

var list = [1, 2, 3, 4]

for i in list {
    print(i)
}

for i in 25..<100 {
    print(i)
}

var loop = 0

while loop < 10 {
    print(loop)
    loop = loop + 1
}


func myfunc(a:Int) -> Int {
    print("the arg is (a)");
    return a;
}

var aa = myfunc(a: 10)
