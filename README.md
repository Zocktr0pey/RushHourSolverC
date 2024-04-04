## A~n incomprehensible~ program to solve RushHour puzzles, written in c.

+ written in C99
+ put puzzle in format:
``` 
++++++++
+      +
+      +
+ XXA  #
+   A  +
+      +
+      +
++++++++
```
+ '+' the walls, should always be the same
+ '#' the exit, should always on the right
+ 'X' the red car, goal of the game is to get this car out the exit, has to be in the same row as the Exit and should be two tiles long
+ 'A' or 'B' or any Other char not previously used is a car, should be two or three tiles in length
+ the solution is a collection of moves in the Format CDN C: char of Car (e.g 'X','A',..), D: Direction U(p), D(own), L(eft), R(ight), N: Number of tiles being moved (1-4)
+ The solution for the example above would be 'AD1 XR3': A one down, X three to the right
