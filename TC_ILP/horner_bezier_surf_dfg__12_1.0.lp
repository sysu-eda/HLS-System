Minimize
M1 + M2
Subject To
x0,10 = 1
x1,9 = 1
x2,7 = 1
x3,6 = 1
x4,5 = 1
x5,3 = 1
x6,5 + x6,6 + x6,7 + x6,8 + x6,9 + x6,10 = 1
x7,7 + x7,8 + x7,9 + x7,10 = 1
x8,6 + x8,7 + x8,8 + x8,9 = 1
x9,5 + x9,6 + x9,7 + x9,8 = 1
x10,3 + x10,4 + x10,5 + x10,6 = 1
x11,2 = 1
x12,6 + x12,7 + x12,8 + x12,9 + x12,10 = 1
x13,5 + x13,6 + x13,7 + x13,8 + x13,9 = 1
x14,3 + x14,4 + x14,5 + x14,6 + x14,7 = 1
x15,2 + x15,3 + x15,4 + x15,5 + x15,6 = 1
x16,1 = 1
x17,1 + x17,2 + x17,3 + x17,4 + x17,5 + x17,6 + x17,7 + x17,8 + x17,9 + x17,10 + x17,11 = 1
x16,1 + x17,1 - M2 <= 0
x11,2 + x15,2 + x17,2 - M2 <= 0
x15,3 + x17,3 - M2 <= 0
x5,3 + x5,2 + x10,3 + x10,2 + x14,3 + x14,2 - M1 <= 0
x15,4 + x17,4 - M2 <= 0
x5,4 + x5,3 + x10,4 + x10,3 + x14,4 + x14,3 - M1 <= 0
x4,5 + x9,5 + x13,5 + x15,5 + x17,5 - M2 <= 0
x6,5 + x6,4 + x10,5 + x10,4 + x14,5 + x14,4 - M1 <= 0
x3,6 + x8,6 + x9,6 + x13,6 + x15,6 + x17,6 - M2 <= 0
x6,6 + x6,5 + x10,6 + x10,5 + x12,6 + x12,5 + x14,6 + x14,5 - M1 <= 0
x8,7 + x9,7 + x13,7 + x17,7 - M2 <= 0
x2,7 + x2,6 + x6,7 + x6,6 + x7,7 + x7,6 + x10,7 + x10,6 + x12,7 + x12,6 + x14,7 + x14,6 - M1 <= 0
x8,8 + x9,8 + x13,8 + x17,8 - M2 <= 0
x2,8 + x2,7 + x6,8 + x6,7 + x7,8 + x7,7 + x12,8 + x12,7 + x14,8 + x14,7 - M1 <= 0
x1,9 + x8,9 + x13,9 + x17,9 - M2 <= 0
x6,9 + x6,8 + x7,9 + x7,8 + x12,9 + x12,8 - M1 <= 0
x0,10 + x0,9 + x6,10 + x6,9 + x7,10 + x7,9 + x12,10 + x12,9 - M1 <= 0
x0,11 + x0,10 + x6,11 + x6,10 + x7,11 + x7,10 + x12,11 + x12,10 - M1 <= 0
9 x1,9 - 10 x0,10 <= -1
7 x2,7 - 9 x1,9 <= -2
6 x3,6 - 7 x2,7 <= -1
5 x4,5 - 6 x3,6 <= -1
3 x5,3 - 5 x4,5 <= -2
6 x8,6 + 7 x8,7 + 8 x8,8 + 9 x8,9 - 7 x7,7 - 8 x7,8 - 9 x7,9 - 10 x7,10 <= -1
5 x9,5 + 6 x9,6 + 7 x9,7 + 8 x9,8 - 6 x8,6 - 7 x8,7 - 8 x8,8 - 9 x8,9 <= -1
3 x10,3 + 4 x10,4 + 5 x10,5 + 6 x10,6 - 5 x6,5 - 6 x6,6 - 7 x6,7 - 8 x6,8 - 9 x6,9 - 10 x6,10 <= -2
3 x10,3 + 4 x10,4 + 5 x10,5 + 6 x10,6 - 5 x9,5 - 6 x9,6 - 7 x9,7 - 8 x9,8 <= -2
2 x11,2 - 3 x5,3 <= -1
2 x11,2 - 3 x10,3 - 4 x10,4 - 5 x10,5 - 6 x10,6 <= -1
5 x13,5 + 6 x13,6 + 7 x13,7 + 8 x13,8 + 9 x13,9 - 6 x12,6 - 7 x12,7 - 8 x12,8 - 9 x12,9 - 10 x12,10 <= -1
3 x14,3 + 4 x14,4 + 5 x14,5 + 6 x14,6 + 7 x14,7 - 5 x13,5 - 6 x13,6 - 7 x13,7 - 8 x13,8 - 9 x13,9 <= -2
2 x15,2 + 3 x15,3 + 4 x15,4 + 5 x15,5 + 6 x15,6 - 3 x14,3 - 4 x14,4 - 5 x14,5 - 6 x14,6 - 7 x14,7 <= -1
1 x16,1 - 2 x11,2 <= -1
1 x16,1 - 2 x15,2 - 3 x15,3 - 4 x15,4 - 5 x15,5 - 6 x15,6 <= -1
Bounds
0 <= x0,10 <= 1
0 <= x1,9 <= 1
0 <= x2,7 <= 1
0 <= x3,6 <= 1
0 <= x4,5 <= 1
0 <= x5,3 <= 1
0 <= x6,5 <= 1
0 <= x6,6 <= 1
0 <= x6,7 <= 1
0 <= x6,8 <= 1
0 <= x6,9 <= 1
0 <= x6,10 <= 1
0 <= x7,7 <= 1
0 <= x7,8 <= 1
0 <= x7,9 <= 1
0 <= x7,10 <= 1
0 <= x8,6 <= 1
0 <= x8,7 <= 1
0 <= x8,8 <= 1
0 <= x8,9 <= 1
0 <= x9,5 <= 1
0 <= x9,6 <= 1
0 <= x9,7 <= 1
0 <= x9,8 <= 1
0 <= x10,3 <= 1
0 <= x10,4 <= 1
0 <= x10,5 <= 1
0 <= x10,6 <= 1
0 <= x11,2 <= 1
0 <= x12,6 <= 1
0 <= x12,7 <= 1
0 <= x12,8 <= 1
0 <= x12,9 <= 1
0 <= x12,10 <= 1
0 <= x13,5 <= 1
0 <= x13,6 <= 1
0 <= x13,7 <= 1
0 <= x13,8 <= 1
0 <= x13,9 <= 1
0 <= x14,3 <= 1
0 <= x14,4 <= 1
0 <= x14,5 <= 1
0 <= x14,6 <= 1
0 <= x14,7 <= 1
0 <= x15,2 <= 1
0 <= x15,3 <= 1
0 <= x15,4 <= 1
0 <= x15,5 <= 1
0 <= x15,6 <= 1
0 <= x16,1 <= 1
0 <= x17,1 <= 1
0 <= x17,2 <= 1
0 <= x17,3 <= 1
0 <= x17,4 <= 1
0 <= x17,5 <= 1
0 <= x17,6 <= 1
0 <= x17,7 <= 1
0 <= x17,8 <= 1
0 <= x17,9 <= 1
0 <= x17,10 <= 1
0 <= x17,11 <= 1
M1 >= 1
M2 >= 1
Generals
x0,10
x1,9
x2,7
x3,6
x4,5
x5,3
x6,5
x6,6
x6,7
x6,8
x6,9
x6,10
x7,7
x7,8
x7,9
x7,10
x8,6
x8,7
x8,8
x8,9
x9,5
x9,6
x9,7
x9,8
x10,3
x10,4
x10,5
x10,6
x11,2
x12,6
x12,7
x12,8
x12,9
x12,10
x13,5
x13,6
x13,7
x13,8
x13,9
x14,3
x14,4
x14,5
x14,6
x14,7
x15,2
x15,3
x15,4
x15,5
x15,6
x16,1
x17,1
x17,2
x17,3
x17,4
x17,5
x17,6
x17,7
x17,8
x17,9
x17,10
x17,11
M1
M2
End
