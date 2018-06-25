DEBUG = 0

TARGET = lindenmaker

CXX = g++
LD = $(CXX)
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-sign-compare
LDFLAGS =

ifeq ($(DEBUG), 1)
CXXFLAGS += -g
else
CXXFLAGS += -O2 -DNDEBUG
endif

UNAME = $(shell uname -s)
ifeq ($(UNAME), Darwin)
    GL_LDFLAGS = -framework OpenGL
else
    GL_LDFLAGS = -lGL
endif
CXXFLAGS += -fPIC $(shell pkg-config --cflags glfw3 glm)
LDFLAGS += -lm -ldl $(GL_LDFLAGS) $(shell pkg-config --libs glfw3)

SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp, obj/%.o, $(SRCS))
DEPS = $(wildcard .d/*.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(LD) -o $@ $^ $(LDFLAGS)

obj/%.o: src/%.cpp
	@mkdir -p $(@D) .d/
	$(CXX) $(CXXFLAGS) -MMD -MF .d/$*.d -c -o $@ $<

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

clean:
	$(RM) bin/* obj/*  .d/lib/* .d/demo/*
