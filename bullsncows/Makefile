# Имя исполняемого файла
TARGET = bulls_and_cows

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -O2

# Внешние библиотеки
CROW_DIR = ./crow
ASIO_DIR = ./asio

# Проверяем установку Crow и Asio
.PHONY: all clean install_crow install_asio

all: check_crow check_asio $(TARGET)

# Проверяем, установлена ли библиотека Crow
check_crow:
	@if [ ! -d "$(CROW_DIR)" ]; then \
		$(MAKE) install_crow; \
	fi

# Проверяем, установлена ли библиотека Asio
check_asio:
	@if [ ! -d "$(ASIO_DIR)" ]; then \
		$(MAKE) install_asio; \
	fi

# Установка Crow (header-only)
install_crow:
	@echo "Installing Crow..."
	git clone https://github.com/CrowCpp/crow.git $(CROW_DIR)

# Установка Asio (header-only)
install_asio:
	@echo "Installing Asio..."
	git clone https://github.com/chriskohlhoff/asio.git $(ASIO_DIR)

# Компиляция проекта
$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -I$(CROW_DIR)/include -I$(ASIO_DIR)/asio/include main.cpp -o $(TARGET)

# Удаление временных файлов
clean:
	rm -f $(TARGET)
	rm -rf $(CROW_DIR)
	rm -rf $(ASIO_DIR)
