# Задача 2. Transform safe and in-place
## Формулировка
Реализуйте функцию `TransformIf`, которая должна для каждого элемента из последовательности между `begin` и `end` проверить предикат `p` и в случае если он выпоняется преобразовать элемент функцией `f`. Метод `TransformIf` в случае если внутри было выброшено исключение должен пробросить его дальше, а последоватлеьность оставить не измененнной. То есть вам нужно сохранять элементы перед преобразованием и в случае необходимости вернуть их в исходную последоватлеьность. Гарантируется что у класс `T` есть конструктор копирования и оператор присваивания. 

Если точнее, функция должна гарантировать следующее:
* Если p и/или f бросает исключение, то функция должна оставить последовательность в первоначальном виде (и бросить то же исключение).
* Если при копировании T возникает исключение, а p и f исключений не бросают, то функция должна отрабатывать полностью
(и не бросать исключений).
* Если исключения возникают и при копировании T, и в p и/или f, функция оставляет последовательность в неопределенном состояниии бросает одно из исключений.

**DEADLINE: 5 МАРТА 23:59**
