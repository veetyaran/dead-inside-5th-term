PROJECT_NAME = MessageBoard

BUILD_DIR = build

CXX = g++

SOURCE_FILE = main.cpp

CROW_DIR = Crow
ASIO_DIR = asio-1.18.1

install_deps:
	sudo apt-get update
	sudo apt-get install -y libboost-all-dev cmake g++

# Скачивание и установка Crow
install_crow:
	@if [ ! -d "$(CROW_DIR)" ]; then \
		git clone https://github.com/CrowCpp/Crow.git $(CROW_DIR); \
	fi

# Сборка
build: install_deps install_crow
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		mkdir $(BUILD_DIR); \
	fi
	cd $(BUILD_DIR) && cmake .. && make

# Очистка
clean:
	rm -rf $(BUILD_DIR)

# Запуск
run: build
	./$(BUILD_DIR)/$(PROJECT_NAME)

# Удаление зависимостей
uninstall_deps:
	sudo apt-get remove --purge -y libboost-all-dev cmake g++
	sudo apt-get autoremove -y
	sudo apt-get clean

.PHONY: install_deps install_crow build clean run uninstall_deps
