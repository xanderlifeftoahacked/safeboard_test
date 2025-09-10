
# Простейший кроссплатформенный "антивирус" 

## Запуск (linux): 
```
git clone git@github.com:xanderlifeftoahacked/safeboard_test.git
cd safeboard_test
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

./build/antivirus --base <file.csv> --log <file.log> --path <dir>
```

### Зависимости:
* CMake
* OpenSSL
  
### Тесты:
* Большая часть функционала покрыта тестами
* Запуск тестов:
```
./build/tests/antivirus_tests
```

## Запуск (windows)
### Собралось с помощью Visual Studio 2022 (другие попытки сражения с виндой не увенчались успехом)  
