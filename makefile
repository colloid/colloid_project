.PHONY: all clean depend

CXX = /home/yekka/bin/gcc-4.6.2/bin/g++
CXXFLAGS = -ansi -pedantic -pedantic-errors -Wall -Wunused-macros -Wundef --std=c++0x
CXXDBG = -g -static -D__DEBUG__
DFLAGS = -MMD
LD = $(CXX)
LDFLAGS =
RM = rm -f
SRCDIR=.

OFILES = debug.o globals.o main.o
TARGET = main
# MYLIB = mylib.a
# $(MYLIB): $(MYLIB)(debug.o) $(MYLIB)(globals.o)
# $(TARGET): main.o $(MYLIB)
# 	$(LD) $(LDFLAGS) $+ -o $@

all: $(TARGET)

$(TARGET): $(OFILES)
	$(LD) $(LDFLAGS) $(OFILES) -o $(TARGET)
#	@echo '### LD $(TARGET)'

-include $(OFILES:.o=.d)

%.o : %.cpp
	$(CXX) -c $(DFLAGS) $(CXXFLAGS) $(CXXDBG) $< -o $@
#	@echo '### CXX $<'

depend: 
	$(RM) *.d && $(CXX) -E -MMD $(OFILES:.o=.cpp) > /dev/null

clean:
	$(RM) *.o *.d *.bak *~
