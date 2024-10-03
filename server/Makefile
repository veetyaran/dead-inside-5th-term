# Имя проекта
PROJECT_NAME = MessageBoard

# Путь к директории для сборки
BUILD_DIR = build

# Компилятор
CXX = g++

# Путь к файлу исходного кода
SOURCE_FILE = main.cpp

# Путь к Crow и Asio библиотекам
CROW_DIR = Crow
ASIO_DIR = asio-1.18.1

# Установка необходимых библиотек (для Debian-based систем)
install_deps:
	sudo apt-get update
	sudo apt-get install -y libboost-all-dev cmake g++

# Скачивание и установка Crow (если не существует)
install_crow:
	@if [ ! -d "$(CROW_DIR)" ]; then \
		git clone https://github.com/CrowCpp/Crow.git $(CROW_DIR); \
	fi

# Скачивание и установка Asio (если не существует)
install_asio:
	@if [ ! -d "$(ASIO_DIR)" ]; then \
		wget https://sourceforge.net/projects/asio/files/asio/1.18.1%20%28Stable%29/asio-1.18.1.tar.bz2; \
		tar -xvf asio-1.18.1.tar.bz2; \
		rm asio-1.18.1.tar.bz2; \
	fi

# Сборка проекта
build: install_deps install_crow install_asio
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		mkdir $(BUILD_DIR); \
	fi
	cd $(BUILD_DIR) && cmake .. && make

# Очистка сборочной директории
clean:
	rm -rf $(BUILD_DIR)

# Запуск проекта
run: build
	./$(BUILD_DIR)/$(PROJECT_NAME)

# Удаление всех зависимостей
uninstall_deps:
	sudo apt-get remove --purge -y libboost-all-dev cmake g++
	sudo apt-get autoremove -y
	sudo apt-get clean

.PHONY: install_deps install_crow install_asio build clean run uninstall_deps
