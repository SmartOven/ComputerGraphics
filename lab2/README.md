# Лабораторная работа 2: Изучение алгоритмов отрисовки растровых линий с применением сглаживания и гамма-коррекции
## **Цель работы**:  
Изучить алгоритмы и реализовать программу, рисующую линию на изображении в формате PGM (P5) с учетом гамма-коррекции sRGB
## **Описание**:  
Программа должна быть написана на C/C++ и не использовать внешние библиотеки.
## Аргументы передаются через командную строку:
`program.exe <имя_входного_файла> <имя_выходного_файла> <яркость_линии> <толщина_линии> <x_начальный> <y_начальный> <x_конечный> <y_конечный> <гамма>`, где:
* <яркость_линии>: целое число 0..255
* <толщина_линии>: положительное дробное число
* <x,y>: координаты внутри изображения, (0;0) соответствует левому верхнему углу, дробные числа (целые значения соответствуют центру пикселей)
* <гамма>: (optional) положительное вещественное число: гамма-коррекция с введенным значением в качестве гаммы. При его отсутствии используется sRGB
#### Программа должна быть написана на C/C++ и не использовать внешние библиотеки.
