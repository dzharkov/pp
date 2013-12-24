srcdir=src
bindir=bin
objdir=obj
exec=pp

full_exec = $(bindir)/$(exec)

VPATH = $(srcdir) $(bindir) $(objdir) 
CXX=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic -Wextra -Werror


objects=$(patsubst $(srcdir)/%.cc, %.o,$(wildcard $(addsuffix /*.cc, $(srcdir))))

all: $(bindir) $(objdir) $(exec)

$(bindir): 
	mkdir -p $(bindir)
$(objdir):
	mkdir -p $(objdir)

-include Makefile.dep
Makefile.dep: $(srcdir)/*.cc
	$(CXX) $(CXXFLAGS) -MM  $(srcdir)/*.cc | sed -e 's/^\(\S.*\)/\1/' > $@

$(exec): $(objects) 
	$(CXX) $(CXXFLAGS) $(addprefix $(objdir)/, $(objects)) -o $(bindir)/$@

%.o: %.cc 
	$(CXX) $(CXXFLAGS) -c $< -o $(objdir)/$@	

test: all 
	$(full_exec) ab.pp

.PHONY: clean

clean:
	rm -rf bin/
	rm -rf obj/
	rm -rf Makefile.dep
