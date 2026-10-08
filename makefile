CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++17

TARGET = main

SRC = main.cpp parser.cpp builtins.cpp process.cpp features.cpp history.cpp pipe.cpp autocomplete.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)