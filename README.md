Purple Haze
===========


Do Chess Engines Dream of Electric Guitar?
------------------------------------------

Purple Haze is a free chess engine compatible with the Xboard protocol. It is
written by Vincent Ollivier and distributed under the terms of the GNU General 
Public License version 3.

The development of Purple Haze started in the winter of 2006/2007 as a project
to learn the Java programming language by a student in Computer Science.
It was terribly slow and rapidly put aside for two years before benefiting from 
a regained interest after much scientific papers' reading on the subject. A new
version was written in C++ between the summer of 2009 and the beginning of 2010
but it was very bugged and difficult to maintain so the second major 
rewriting started during the winter of 2011. This time it is only slightly
faster but a lot more organized (comparatively) and follow a test-driven 
development. The goal is to end up with something reasonably good and free of
bugs.

Most of its concepts or algorithms are inspired from scientific papers and 
Open Source engines, notably TSCP, CPW, Fruit, Crafty and Stockfish. A lot of 
the ideas used for the implementation of theses concepts also comes from 
reading the [Computer Chess Club Forums](http://talkchess.com/forum/) and the 
[Chess Programming Wiki](http://chessprogramming.wikispaces.com/). Without 
them Purple Haze would not be able to do more than moving the pieces around 
the board.


Implemented features
--------------------

* Search:
    * Principal Variation Search
    * Iterative Deepening
    * Transposition Table
    * Null Move Pruning (still counterproductive for the moment)

* Move Ordering:
    * Best move from Iterative Deepening or Transposition Table
    * Most Valuable Victim / Least Valuable Aggressor

* Quiescence Search:
    * Standing pat
    * Delta Pruning
    * Limitation to good captures only (via MVV/LVA)

* Debuging
    * Perft
    * Divide

* Miscellaneous
    * SAN output
    * FEN parsing
    * Zobrist hashing
    * Xboard protocol
    * Time Management


Installation
------------

See INSTALL for installation instructions.


Usage
-----

You are free to read it, run it and modify it. If you choose to do so, drop me
an email at <contact@vincentollivier.com>, it is still a very young project 
written by a beginner in chess programming so any feedback would be really 
appreciated!
