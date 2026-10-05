CXX = g++
CXXFLAGS = -std=c++17 -O3 -pthread -Iinclude
SRC = src/main.cpp src/PricingEngine.cpp src/Backtester.cpp src/Logger.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = OptionEngine

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) -pthread

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(EXEC)