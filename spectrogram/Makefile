# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread

# Библиотеки и пути
LIBS = -lsndfile -lsfml-graphics -lsfml-window -lsfml-system -lm

# Имя исполняемого файла
TARGET = spectrogram

# Исходные файлы
SRCS = spectrogram.cpp

# Объектные файлы
OBJS = $(SRCS:.cpp=.o)

# Правило по умолчанию - сборка всего проекта
all: check_libsndfile $(TARGET)

# Правило для сборки исполняемого файла
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Правило для сборки объектных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Проверка и установка библиотеки libsndfile
check_libsndfile:
	@if ! pkg-config --exists sndfile; then \
		echo "Installing libsndfile..."; \
		sudo apt-get update && sudo apt-get install -y libsndfile1-dev; \
	fi

# Правило для очистки скомпилированных файлов
clean:
	rm -f $(OBJS) $(TARGET)

# Правило для полной очистки, включая результат и изображение спектрограммы
clean_all: clean
	rm -f result.txt spectrogram.png

.PHONY: all clean clean_all check_libsndfile
