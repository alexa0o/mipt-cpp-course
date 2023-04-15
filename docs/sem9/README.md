# Семинар 9. move - семантика, rvalue ссылки
## lvalue, rvalue

lvalue (locator value) представляет собой объект, который занимает идентифицируемое место в памяти (например, имеет адрес).  
rvalue определено путём исключения, говоря, что любое выражение является либо lvalue, либо rvalue.  

Примеры:
```c++
int a = 4; // lvalue, OK
4 = a; // rvalue, NOT OK
a + 2 = 6; // rvalue, NOT OK

int pa[10];
*(&pa[0] + 1) = 3; // lvalue, OK
&pa = 42; // rvalue, NOT OK

const std::string& s = std::string(); // OK
std::string& s = std::string(); // NOT OK
```

Мотивация - хотим передавать rvalue без лишних копирований. Например, конструктор перемещения:
```c++
struct Movable {
    int* Ptr;
    size_t Size;

    // конструктор перемещенния
    Movable(Movable&& other) : Ptr(other.Ptr), Size(other.Size) {
        other.Ptr = nullptr;
        other.Size = 0;
    }

    // конструктор копирования
    Movable(const Movable& other) : Ptr(new int[other.Size]), Size(other.Size) {
        std::memcpy(Ptr, other.Ptr, Size * sizeof(int));
    }
};
```

Рассмотрим еще один пример:
```c++
class Intvec {
public:
    explicit Intvec(size_t num = 0)
        : m_size(num), m_data(new int[m_size])
    {
        log("constructor");
    }

    ~Intvec()
    {
        log("destructor");
        if (m_data) {
            delete[] m_data;
            m_data = 0;
        }
    }

    Intvec(const Intvec& other)
        : m_size(other.m_size), m_data(new int[m_size])
    {
        log("copy constructor");
        for (size_t i = 0; i < m_size; ++i)
            m_data[i] = other.m_data[i];
    }

    Intvec& operator=(const Intvec& other)
    {
        log("copy assignment operator");
        Intvec tmp(other);
        std::swap(m_size, tmp.m_size);
        std::swap(m_data, tmp.m_data);
        return *this;
    }
private:
    void log(const char* msg)
    {
        cout << "[" << this << "] " << msg << "\n";
    }

    size_t m_size;
    int* m_data;
};
```

Запустим следующий код:
```c++
cout << "assigning rvalue...\n";
v2 = Intvec(33);
cout << "ended assigning rvalue...\n";
```

Вывод:
```c++
assigning rvalue...
[0x28ff08] constructor
[0x28fef8] copy assignment operator
[0x28fec8] copy constructor
[0x28fec8] destructor
[0x28ff08] destructor
ended assigning rvalue...
```

Получили дополнительные вызовы конструктора и деструктора. Давайте добавим в наш класс пермещащий оператор присваивания:
```c++
Intvec& operator=(Intvec&& other)
{
    log("move assignment operator");
    std::swap(m_size, other.m_size);
    std::swap(m_data, other.m_data);
    return *this;
}
```

Вывод:
```c++
assigning rvalue...
[0x28ff08] constructor
[0x28fef8] move assignment operator
[0x28ff08] destructor
ended assigning rvalue...
```

Деструктор удаляет пустой буффер в данном примере.


### std::move
Этот метод выполняет преобразование типа следующего вида `static_cast<typename std::remove_reference<T>::type&&>(t)`, при этом ничего не делает с самими объектом.
```c++
std::string s("Hello world");
std::move(s); // s все еще содержит строку "Hello world"
```

### Универсальная ссылка, std::forward
```c++
template <class T>
void doSmth(T&& value);
```

В примере выше в зависимости от типа Т будет выведен тип аргумента функции. Если Т - rvalue, то и метод doSmth будет принимать rvalue, иначе - lvalue. Теперь представим, что хотим прокинуть внутри функции doSmth value дальше, при этом не забывая о том, rvalue это или lvalue:
```c++
template <class T>
void doSmth(T&& value) {
    doSmthImpl(std::forward<T>(value));
}
```
Давайте попробуем реализовать функцию std::forward:
```c++
template <typename T>
T&& my_forward(T&& x) {
    return static_cast<T&&> (x);
}
```

Какие проблемы видите в такой функции?

Всегда можно вызвать ее без явной подстановки шаблона, а значит в случае с doSmth будет интерпретирован аргумент как lvalue. Нужно задавать тип явно. Чтобы запретить пользователю вызов без спецификации, можем написать так:
<details>
    <summary>Спойлер</summary>
```c++
template <class T> T&& forward(typename remove_reference<T>::type& t) noexcept;
template <class T> T&& forward(typename remove_reference<T>::type&& t) noexcept;
```
</details>

### Упражнение
Какой вывод у программы?
```c++
#include <iostream>
#include <utility>

int y(int &) { return 1; }
int y(int &&) { return 2; }

template <class T> int f(T &&x) { return y(x); }
template <class T> int g(T &&x) { return y(std::move(x)); }
template <class T> int h(T &&x) { return y(std::forward<T>(x)); }

int main() {
  int i = 10;
  std::cout << f(i) << f(20);
  std::cout << g(i) << g(20);
  std::cout << h(i) << h(20);
  return 0;
}
```

<details>
    <summary>Ответ</summary>
    112212
</details>

## Ложка дегтя в бочке меда (перегрузка для универсальных ссылок)
Хотим иметь метод, пишуший в лог имена. Код:
```c++
std::multiset<std::string> names;

void logName(const std::string& name) {
    names.emplace(name);
}
```

Пользоваться нашим методом можно по-разному:
```c++
std::string petName("Daria");
logName(petName);
logName(std::string("Maria"));
logName("Petr");
```

В одном из трех случаев решаем задачу оптимально. Можем починить проблему универсальными ссылками:
```c++
template <class T>
void logName(T&& name) {
    names.emplace(std::forward<T>(name));
}
```

Решаем задачу оптимально. Но теперь появляется новое требование, хотим некоторые имена доставать из кеша, то есть нужна перегрузка функции, которая по индексу определяет имя:
```c++
void logName(int idx) {
    names.empalce(nameByIdx(idx));
}
```

Снова все будет хорошо, но до тех пор, пока мы не напишем следующее:
```c++
short i = 3;
logName(i); // compilation error
```

В перегрузке с универсальной ссылкой получаем точное соответствие и оно побеждает в разрешении перегрузки расширение short до int. Особенно опасно это для конструкторов, потому что универсальная ссылка может перехватить копирующий или перемещающий конструктор.

### Упражнение
Написать оптимальным образом метод, добавляющий имена в вектор:
```c++
class Widget {
public:
    void addName(??? name) {
        ??? 
    }

private:
    std::vector<std::string> names;
};
```

Тут важно подумать, что аргумент может быть как rvalue, так и lvalue, а также об удобстве реализации (например, реализацию шаблона придется разместить в заголовочном файле).


