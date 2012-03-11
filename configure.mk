# Use git to define VERSION: 'v2.0.1-81-g0c1619f'
# Otherwise see 'src/common.h'
use_git_describe = yes

# Optimize executable for speed
optimize = yes
use_asserts = no

# Search options
use_null_move_pruning = yes

# Debug options
use_profiler = no
use_gdb = no

# Supported compiler options: gcc, intel, clang
ifndef compiler
    compiler = gcc
endif
ifeq ($(compiler),gcc)
    CXX = g++
endif
ifeq ($(compiler),intel)
    CXX = icpc
endif
ifeq ($(compiler),clang)
    CXX = clang++
endif

CXXFLAGS = -std=c++0x -pipe
ifeq ($(compiler),intel)
    CXXFLAGS += -Wall -Wremarks -wd981 -wd2259
else
    CXXFLAGS += -Wall -pedantic-errors -Wcast-qual -Wshadow -Wextra
endif
ifeq ($(use_git_describe),yes)
    CXXFLAGS += -DVERSION=\"$(shell git describe HEAD)\"
endif
ifeq ($(optimize),yes)
    ifeq ($(compiler),intel)
        CXXFLAGS += -fast
    else
        CXXFLAGS += -O3 -march=native -mtune=native
    endif
endif
ifeq ($(use_gdb),yes)
    CXXFLAGS += -g
endif
ifeq ($(use_profiler),yes)
    CXXFLAGS += -pg -lprofiler
endif
ifeq ($(use_asserts),no)
    CXXFLAGS += -DNDEBUG
endif
ifeq ($(use_null_move_pruning),yes)
    CXXFLAGS += -DNMP
endif
