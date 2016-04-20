# Game Engine
This project is my attempt at a generalized framework for writing AI to play various
games. The framework generalizes the minimax algorithm such that one implementation can
be applied to any game that implements the requisite classes:
 - A Heuristic Class
 - A Board State Class
 - A Player Class
 - A Move Iterator Class

My goal is to apply this to chess, checkers, tic-tac-toe, and meta tic-tac-toe.

# Implementation
This framework makes extensive use of templating to achive high performance through inlining code
and doing as much work as possible at compile time. 

The AI algorithm provided by the framework is simply Minimax with Alpha Beta pruning. I intend to try 
implementing scout or a similar algorith eventually. I will also try to improve the efficiency by
adding move ordering to improve the order in which moves get explored

# Usage
Currently the framework comes with one demo game: chess. To try it out simply
```
mkdir bin; make; ./bin/program
```