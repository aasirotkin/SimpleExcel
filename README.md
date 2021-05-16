### Рекомендации по сборке проекта

1. Установите Java SE Runtime Environment 8 https://www.oracle.com/java/technologies/javase-jre8-downloads.html
2. Установите ANTLR (ANother Tool for Language Recognition) https://www.antlr.org/, выполнив все пункты в меню Quick Start
3. Проверте название файла antlr-4.9.2-complete.jar в файлах FindANTLR.cmake и CMakeLists.txt, если оно отличается, то поправьете название.
4. Создайте папку antlr4_formula, скопируйте в неё файл Formula.g4, перейдите в неё и выполните команду:

```cpp
antlr4 -Dlanguage=Cpp Formula.g4
```

5. Создайте папку antlr4_runtime и скачайте в неё файлы из https://github.com/antlr/antlr4/tree/master/runtime/Cpp.
6. Всё остальное сделает CMakeLists.txt!