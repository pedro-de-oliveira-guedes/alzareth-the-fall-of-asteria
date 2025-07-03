CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I /opt/SDL2/include/SDL2/
LDFLAGS := -L/opt/SDL2/lib -lSDL2 -L/opt/SDL2/lib -lSDL2_image -L/opt/SDL2/lib -lSDL2_ttf -L/opt/SDL2/lib -lSDL2_mixer

SRCS := $(shell find Source -name "*.cpp")
OBJS := $(patsubst Source/%.cpp, object/%.o, $(SRCS))

OUT := alzareth-the-fall-of-asteria

all: $(OUT)

# Regra principal para linkar os objetos
$(OUT): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Regra para compilar arquivos .cpp em object/
object/%.o: Source/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(OUT)
	cd Source && ../$(OUT)

clean:
	rm -rf object $(OUT)
