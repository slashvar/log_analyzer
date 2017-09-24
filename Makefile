# Makefile

ifdef NO_POSIX_ISATTY
  NO_POSIX = -DNO_ISATTY -D_POSIX_SOURCE
else
  NO_POSIX =
endif


CXX = clang++
CPPFLAGS = -MMD ${NO_POSIX}
CXXFLAGS = -Wall -Wextra -std=c++14 -O3
LDFLAGS =
LDLIBS =

OBJ = log_analyzer.o
DEP = ${OBJ:.o=.d}

all: log_analyzer

log_analyzer: ${OBJ}
	$(LINK.cc) $^ $(LDLIBS) -o $@

clean:
	${RM} ${OBJ} ${DEP}
	${RM} log_analyzer

-include ${DEP}

# END
