Purple Haze
===========


Do Chess Engines Dream of Electric Guitar?
------------------------------------------

Purple Haze is a free chess engine compatible with the Xboard protocol. It is
written by Vincent Ollivier and distributed under the terms of the GNU General
Public License version 3.

The development of Purple Haze started in the winter of 2006-2007 as a pet
project to learn the Java programming language by a student in Computer
Science. It was terribly slow and rapidly put aside for two years before
benefiting from a regained interest after much research and reading of papers
on the subject.

A new version was written in C++11 between the summer of 2009 and the beginning
of 2010 but it was full of bugs and not well engineered so a second major
rewriting started during the winter of 2010-2011. This time it was only
slightly faster but it would prove to be a lot easier to maintain and the
overall code quality was enhanced by using a test-driven development practice.
The goal is to end up with something reasonably good and free of bugs.

Most of its concepts or algorithms are inspired from scientific papers and
Open Source engines, notably TSCP, CPW, Fruit, Crafty and Stockfish. A lot of
the ideas used for the implementation of theses concepts also comes from
reading the [Computer Chess Club Forums](http://talkchess.com/forum/) and the
[Chess Programming Wiki](http://chessprogramming.wikispaces.com/). Without
them Purple Haze would not be able to do more than moving the pieces around
the board.


Implemented features
--------------------

Data Structures:
* 0x88 Board Representation
* Piece-Lists

Moves:
* Lazy Move Generation
* Best Move from Iterative Deepening or Transposition Table
* Most Valuable Victim / Least Valuable Aggressor
* Killer Moves

Search:
* Principal Variation Search
* Iterative Deepening
* Transposition Table
* Mate Pruning
* Check Extension
* Adaptive Null Move Pruning
* Extended Futility Pruning
* Late Move Reduction

Quiescence Search:
* Standing Pat
* Delta Pruning
* Move Ordering (via MVV/LVA)

Evaluation:
* Lazy Evaluation
* Material Hash Table
* Opening and Ending Piece-Square Tables
* Tapered Evaluation

Debugging:
* Perft
* Divide
* EPD Test Positions

Miscellaneous:
* SAN output
* FEN parsing
* Zobrist hashing
* Xboard protocol
* Time Management


Installation
------------

See INSTALL for detailed installation instructions or just type the following
commands in a terminal:

    $ git clone git://github.com/vinc/purplehaze.git
    $ cd purplehaze
    $ make


Usage
-----

You are free to read it, run it and modify it. If you choose to do so, drop me
an email at <contact@vincentollivier.com>, it is not yet a mature project so
any feedback will be really appreciated.
