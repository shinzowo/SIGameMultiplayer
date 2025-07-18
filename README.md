Для сборки необходимы CMake, g++

Установка cmake
Скачайте с https://cmake.org/download/
При установке отметьте: "Add CMake to system PATH"

Установка MinGW-w64
Скачайте с https://www.mingw-w64.org/
Добавьте путь к bin директории в переменную окружения PATH

Для сборки текущей версии из корневой директории в командной строке выполните:

``
cmake -S . -B build -DCMAKE_CXX_COMPILER=g++
``
``
cmake --build build
``

Исполняемый файл будет находиться в `./build`

