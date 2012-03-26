Installation
============

For compiling Purple Haze:

    $ make


For compiling with the Intel(R) C++ Compiler, here on the IA-32 architecture
using Intel(R) C++ Compiler XE version 12.0.4 (2011/04/27) on GNU/Linux:

    $ source /opt/intel/composerxe/bin/compilervars.sh ia32
    $ make compiler=intel


For using Purple Haze:

    $ ./purplehaze


For running Purple Haze unit tests:

    $ cd test/unit
    $ make
    $ ./test_purplehaze --gtest_color=1
