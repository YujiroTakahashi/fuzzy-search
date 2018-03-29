CXX = c++
CXXFLAGS = -std=c++14 -g -O0
LDFLAGS =
SRCDIR = src/
OBJ = trie.o main.o
OBJDIR = obj/
INCLUDES = -Iinclude
EXEC = sample

OBJS = $(addprefix $(OBJDIR), $(OBJ))
DEPS = $(wildcard include/*.h) Makefile


$(EXEC): $(OBJS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(DEPS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

all:
	$(EXEC)

clean:
	rm -rf obj/*.o $(EXEC)
