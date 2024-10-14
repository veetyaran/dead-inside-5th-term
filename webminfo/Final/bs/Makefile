CXX = g++
CXXFLAGS = -Wall -std=c++11
LIBS = -lebml -lmatroska

TARGET = remove_audio
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)

install_dependencies:
	@echo "Installing dependencies..."
	@sudo apt-get update && sudo apt-get install -y libebml-dev libmatroska-dev

.PHONY: all clean install_dependencies