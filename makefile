CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lSDL2 -lSDL2_image  # 根据你的实际需要添加链接库

SRCDIR = src
BINDIR = bin

SOURCES = caster.cpp framebuffer.cpp map.cpp textures.cpp utils.cpp
OBJECTS = $(patsubst %.cpp,$(BINDIR)/%.o,$(SOURCES))

EXECUTABLE = $(BINDIR)/caster

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BINDIR)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

.PHONY: all clean run
