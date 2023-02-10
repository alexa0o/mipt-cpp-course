# Семинар 1. Исключения.
## Два подхода к обработке ошибок
1. Коды возврата
Пример простого клиента для записи строки в сокет. Вынуждены обрабатывать код возврата почти каждой функции. Код тяжело читается, легко пропустить ошибку не обработав определенный код.
```c++
int sock = 0, valread, client_fd;
struct sockaddr_in serv_addr;
char* hello = "Hello from client";
char buffer[1024] = { 0 };
if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
}

serv_addr.sin_family = AF_INET;
serv_addr.sin_port = htons(PORT);

// Convert IPv4 and IPv6 addresses from text to binary
// form
if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
    <= 0) {
    printf(
        "\nInvalid address/ Address not supported \n");
    return -1;
}

if ((client_fd
     = connect(sock, (struct sockaddr*)&serv_addr,
               sizeof(serv_addr)))
    < 0) {
    printf("\nConnection Failed \n");
    return -1;
}
send(sock, hello, strlen(hello), 0);
printf("Hello message sent\n");
valread = read(sock, buffer, 1024);
printf("%s\n", buffer);

// closing the connected socket
close(client_fd);
```
Основной плюс такого подхода - простота.

2. Исключения
Другим способом сообщить об обнаруженной ошибке являются исключения. С каждым сгенерированным исключением связан некоторый объект, который как-то описывает ошибку. Таким объектом может быть что угодно — даже целое число или строка. Но обычно для описания ошибки заводят специальный класс и генерируют объект этого класса:
```c++
#include <iostream>
 
struct WrongAgeException {
    int age;
};
 
int ReadAge() {
    int age;
    std::cin >> age;
    if (age < 0 || age >= 128) {
        throw WrongAgeException(age);
    }
    return age;
}
```
Здесь в случае ошибки оператор throw генерирует исключение, которое представлено временным объектом типа WrongAgeException. В этом объекте сохранён для контекста текущий неправильный возраст age. Функция досрочно завершает работу: у неё нет возможности обработать эту ошибку, и она должна сообщить о ней наружу. Поток управления возвращается в то место, откуда функция была вызвана. Там исключение может быть перехвачено и обработано.
```c++
int main() {
    try {
        age = ReadAge();  // может сгенерировать исключение
        // Работаем с возрастом age
    } catch (const WrongAgeException& ex) {  // ловим объект исключения
        std::cerr << "Age is not correct: " << ex.age << "\n";
        return 1;  // выходим из функции main с ненулевым кодом возврата
    }
    // ...
}
```
Так как мы заранее знаем, что функция `ReadAge()` может выбросить исключение типа `WrongAgeException`, то оборачиваем ее в `try-catch` блок. Если происходит исключение, то для него подбирается первый подходящий по типу обработчик и выполняется его код. Если ни один из обработчиков не подошел, то исключение пробрасывается дальше по стеку вызовов. Если обработчик не нашелся даже в функции `main`, то программа аварийно завершается.

Давайте добавим больше типов исключений в нашу функцию. Предположим, что поток ввода оборвался досрочно и мы не смогли прочитать число. СТандартное поведение в такой ситуации установит специально [флаг ошибки](https://en.cppreference.com/w/cpp/io/ios_base/iostate) у `std::cin`. Давайте включим генерацию исключений при ошибках такого вида:
```c++
int ReadAge() {
    std::cin.exceptions(std::istream::failbit);
    int age;
    std::cin >> age;
    if (age < 0 || age >= 128) {
        throw WrongAgeException(age);
    }
    return age;
}
```
Теперь ошибка такого рода приведет к генерации исключения `std::istream::failure`, которое будем обрабатывать в функции `main`:
```c++
int main() {
    try {
        age = ReadAge();  // может сгенерировать исключения разных типов
        // Работаем с возрастом age
    } catch (const WrongAgeException& ex) {
        std::cerr << "Age is not correct: " << ex.age << "\n";
        return 1;
    } catch (const std::istream::failure& ex) {
        std::cerr << "Failed to read age: " << ex.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Some other exception\n";
        return 1;
    }
    // ...
}
```
Функция `what()` имеется во всех исключениях стандартной библиотеки и возвращает текстовое описание ошибки.
`catch(...)` перехватывает любые исключения, которые не были обработаны ранее.

## Исключения в стандартной библиотеке
Исключения в stl представляют собой [иерархию наследования](https://en.cppreference.com/w/cpp/error/exception). Это позволяет писать обработчик ошибок сразу на несколько группу ошибок, например, `std::runtime_error`.
Исключение `std::invalid_argument` уместно, когда получили невалидный аргумент. Например, `std::bitset` имеет конструктор от строки, однако если в ней есть символы отличные от 0 и 1, то он выбросит соответствующее исключение.

Исключения в конструкторах - это нормально.

## Свертка стека (stack unwinding)
```c++
#include <exception>
#include <iostream>

class Logger {
    inline static int counter = 0;
    const int id;

    public:
    Logger(): id(++counter) {
        std::cout << "Logger(): " << id << "\n";
    }
 
    Logger(const Logger& other): id(++counter) {
        std::cout << "Logger(const Logger&): " << id << " " << other.id << "\n";
    }
 
    Logger& operator = (const Logger& other) {
        std::cout << "Logger& operator = (const Logger&) " << id << " " << other.id << "\n";
        return *this;
    }
 
    ~Logger() {
        std::cout << "~Logger() " << id << "\n";
    }
};

void f() {
    std::cout << "Welcome to f()\n";
    Logger x;
    // ...
    throw std::exception();
}

int main() {
    try {
        Logger y;
        f();
    } catch (const std::exception&) {
        std::cout << "Something happened...\n";
        return 1;
    }
}
```

Мы увидим такой вывод:
```
Logger(): 1
Welcome to f()
Logger(): 2
~Logger(): 2
~Logger(): 1
Something happened...
```
Так как мы вынуждены досрочно покинуть функцию f(), то начинает происходить свертка стека: уничтожаются все объекты внутри самой функции и try блока. Поэтому мы видим вызовы деструкторов 2 и 1.
Если же исключение будет выброшено из деструктора во время свертки стека, то программа аварийно завершится.

## Exception-safety
Есть 4 основных типа гарантии безопасности относительно исключений:
1. Отстутствие каких-либо гарантий. При возникновение исключения могут нарушаться инварианты в структуре данных и дальнейшее использование может быть невозможно.
2. Базовая гарантия безопасноти. После исключения внутреннее состояние объекта может измениться, но он останется согласованным. То есть дальшейшее использование возможно.
3. Строгая гарантия безопасности. При возникновении исключения внутреннее состояние не изменяется и все ссылки остаются валидными.
4. Гарантия отсутствия сбоев. Функции вообще не выбрасывают искючений. Примерами служат хорошо написанные деструкторы, константные функции вида `Size`.

Функции, имеющие гарантию отсутствия сбоев помечают как `noexcept`. Это помогает компилятору генерировать более эффективный код.

## Спецификации исключений
```c++
noexcept        (1)
noexcept(true)  (2)
noexcept(false) (3)
```
1 и 2 - эквивалентны.
Все функции в С++ либо потенциально могут выбросить исключение, либо не выбрасывают. 
Нельзя выполнять перезгрузку функции с `noexcept`:
```c++
void f() noexcept;
void f(); // error: different exception specification
void g() noexcept(false);
void g(); // ok, both declarations for g are potentially-throwing
```
Если виртуальная функция помечена как `noexcpet`, то все ее перегрузки тоже должны быть `noexcept`, за исключением удаления.
`noexcept` бывает условным:
```c++
template<class T>
T f() noexcept(sizeof(T) < 4);
```
Условие должно быть constexpr.

## `std::terminate`
Функция будет вызвана если:
1. Выброшено и нигде необработано ислкючение
2. В ходе сворачивания стека было выброшено исключение (например, из деструктора локально объекта)
3. Конструктор или деструктор статического объекта бросил исключение
4. `noexcept` спецификация нарушена
5. Другое

Будет вызван `std::terminate_handler`, который может быть задан пользователем через метод `std::set_terminate`:
```c++
std::terminate_handler set_terminate( std::terminate_handler f ) noexcept;
```
Пример:
```c++
#include <iostream>
#include <cstdlib>
#include <exception>
 
int main()
{
    std::set_terminate([](){
        std::cout << "Unhandled exception\n" << std::flush;
        std::abort();
    });
    throw 1;
}
```
Также можем получить `std::terminate_handler`:
```c++
std::terminate_handler get_terminate() noexcept;
```

## Полезные ссылки
* [Обработка исключений](https://academy.yandex.ru/handbook/cpp/article/exceptions)
* [Иерархия исключений](https://en.cppreference.com/w/cpp/error/exception)
* [Где хранятся исключения?](https://stackoverflow.com/questions/6611880/when-i-throw-something-where-is-it-stored-in-memory)
* [Спецификация исключений](https://en.cppreference.com/w/cpp/language/noexcept_spec)
