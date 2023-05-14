# Задания
## Подготовка
Для сдачи заданий необходимо создать репозиторий на Github, добавить меня в коллабораторы.
## Правила сдачи
Решение каждой задачи размещаете в отдельной ветке и папке в корне репозитория, после чего создаете Pull Request в master и присылаете ссылку на него мне в личные сообщения tg.  

Ветку и папку называете taskN, где N - номер задачи. Для названий и описаний коммитов и Pull Request'ов ограничений нет.  

Для каждой задачи указан жесткий и мягкий дедлайн. Сдача после мягкого дедлайна возможна со штрафом в 50%, после жесткого - 70%.  

Задача считается сданной после отправки ссылки в tg. Баллы за задачу вы получаете после того как я замержу ее в master. Прохождение всех тестов не гарантирует получение баллов, но является необходимым условием для этого.

## Тесты
### Сборка и запуск
```
mkdir build && cd build
cmake <path-to-tasks-folder> -DTASK=<task-number>
cmake --build .
ctest --verbose
```

## Задачи
[Задача 1. Тайное становится явным.](https://github.com/alexa0o/mipt-cpp-course/tree/main/tasks/task1)  
[Задача 2. TransformIf safe and in-place.](https://github.com/alexa0o/mipt-cpp-course/tree/main/tasks/task2)  
[Задача 3. IndexedIterator.](https://github.com/alexa0o/mipt-cpp-course/tree/main/tasks/task3)  
[Задача 4. Аллокатор 1.](https://github.com/alexa0o/mipt-cpp-course/tree/main/tasks/task4)  
