# Use git to define VERSION: 'v2.0.1-81-g0c1619f'
# Otherwise see 'src/common.h'
use_git_describe = yes

# Optimize executable for speed
optimize = yes
debug = no

# Search options
use_insertion_sort = no
use_check_extension = yes
use_null_move_pruning = yes
use_internal_iterative_deepening = yes
use_futility_pruning = yes
use_late_move_reduction = yes

# Debug options
use_gcov = no
use_gdb = no
use_profiler = no

CXXFLAGS = -std=c++0x -pthread -pipe
ifeq ($(CXX),icpc) # Intel Compiler
    CXXFLAGS += -Wall -Wremarks -wd981 -wd2259
else
    CXXFLAGS += -Wall -pedantic-errors -Wcast-qual -Wshadow -Wextra
endif
ifeq ($(use_git_describe),yes)
    CXXFLAGS += -DVERSION=\"$(shell git describe HEAD)\"
endif
ifeq ($(optimize),yes)
    ifeq ($(CXX),icpc) # Intel Compiler
        CXXFLAGS += -fast
    else
        CXXFLAGS += -O3 -march=native -mtune=native
    endif
endif
ifeq ($(use_gcov),yes)
    CXXFLAGS += -fprofile-arcs -ftest-coverage
endif
ifeq ($(use_gdb),yes)
    CXXFLAGS += -g
endif
ifeq ($(use_profiler),yes)
    CXXFLAGS += -pg -lprofiler
endif
ifeq ($(debug),no)
    CXXFLAGS += -DNDEBUG
endif
ifeq ($(use_insertion_sort),no)
    CXXFLAGS += -DNIS
endif
ifeq ($(use_check_extension),no)
    CXXFLAGS += -DNCE
endif
ifeq ($(use_null_move_pruning),no)
    CXXFLAGS += -DNNMP
endif
ifeq ($(use_internal_iterative_deepening),no)
    CXXFLAGS += -DNIID
endif
ifeq ($(use_futility_pruning),no)
    CXXFLAGS += -DNFP
endif
ifeq ($(use_late_move_reduction),no)
    CXXFLAGS += -DNLMR
endif
