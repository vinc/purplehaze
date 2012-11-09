Installation
============

For obtaining Purple Haze and compiling the source with the default compiler:

    $ git clone git://github.com/vinc/purplehaze.git
    $ cd purplehaze
    $ make


For compiling the source with the Clang C++ compiler:

    $ env CXX=clang++ make


For compiling with the Intel C++ compiler, on the IA-32 architecture using
Intel C++ Compiler XE on GNU/Linux:
 
    $ source /opt/intel/composerxe/bin/compilervars.sh ia32
    $ env CXX=icpc make


For running the unit tests:

    $ make test


And finally for executing Purple Haze:

    $ ./purplehaze

