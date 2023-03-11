# Семинар 5. Стандартные контейнеры и адаптеры над ними.
## Адаптеры
### [std::stack](https://en.cppreference.com/w/cpp/container/stack)
Простейший адаптер над контейнером, поддерживающим методы: `back(), push_back(), pop_back()` и удовлетворяющем требованиям SequenceContainer, реализующий интерфейс стека - структуры данных, дающей доступ в первую очередь к последнему добавленному элементу (LIFO - last in, first out). Давайте посмотрим на объявление стека в стандартной библиотеке:
```c++
template<
    class T,
    class Container = std::deque<T>
> class stack;
```
Т - тип хранимых в стеке данных,  
Container - контейнер, на основе которого создан стек (стандартно - std::deque).  

Примеры контейнеров, удовлетворяющих требованиям для создания стека: std::vector (в том числе std::vector<bool>), std::deque, std::list.

### [std::queue](https://en.cppreference.com/w/cpp/container/queue)
Очередь. Контейнер в основе очереди должен поддерживать следющие методы: `back(), front(), push_back(), pop_front()` и удовлетворять SequenceContainer. Дает доступ до первого добавленного элемента (FIFO - first in, first out). 
```c++
template<
    class T,
    class Container = std::deque<T>
> class stack;
```

В качестве основного контейнера можно использовать std::deque или std::list.

### [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue)
Адаптер, который даает возможность за О(1) получить доступ к самому большому элементу в контейнере (стандартно) и добавление (извлечение) за О(log n).
```c++
template<
    class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>
> class priority_queue;
```
Появился новый шаблонный аргумент - Compare. Этот аргумент позволяет менять последовательность элементов в контейнере, например, можно получать всегда наименьший элемент, используя std::greater<T>.
Работа с priority_queue очень похожа на работу с кучей, однако, у нас отстутствует возможность случайно испортить структуру данных.  
Container должен иметь методы `front(), push_back(), pop_back()`, удовлетворять SequenceContainer и иметь итератор LegacyRandomAccessIterator. Этому удовлетворяют std::vector(<bool>), std::deque.  
Обратите внимание, что в priority_queue первым становится элемент, последний при сортировке со сравнениями Compare.

### Реализация std::priority_queue

## std::array
Семантически ничем не отличается от обычного статического массива T[N]. Все элементы изначально проинициализированы, в отличие от других контейнеров. Конструкторы вызываются слева направо, а деструкторы справа налево. Посмотрим на пример:
```c++
#include <array>
#include <iostream>

struct MyTriple {
    MyTriple() { std::cout << "Constructing at: " << this << std::endl; }
    ~MyTriple() { std::cout << "Destructing at: " << this << std::endl; }
    int32_t a;
    int64_t b;
    int32_t c;
};

int main() {
    std::array<MyTriple, 10> arr;
    std::cout << "inside main()" << std::endl;
}
```
```
Constructing at: 0x7ffeafc95390
Constructing at: 0x7ffeafc953a8
Constructing at: 0x7ffeafc953c0
Constructing at: 0x7ffeafc953d8
Constructing at: 0x7ffeafc953f0
Constructing at: 0x7ffeafc95408
Constructing at: 0x7ffeafc95420
Constructing at: 0x7ffeafc95438
Constructing at: 0x7ffeafc95450
Constructing at: 0x7ffeafc95468
inside main()
Destructing at: 0x7ffeafc95468
Destructing at: 0x7ffeafc95450
Destructing at: 0x7ffeafc95438
Destructing at: 0x7ffeafc95420
Destructing at: 0x7ffeafc95408
Destructing at: 0x7ffeafc953f0
Destructing at: 0x7ffeafc953d8
Destructing at: 0x7ffeafc953c0
Destructing at: 0x7ffeafc953a8
Destructing at: 0x7ffeafc95390
```

## std::bitset
Контейнер для манипуляций с битами. В отличие от std::vector<bool> хранится на стеке. Интересно реализован оператор [], он возвращает "легкий" объект, содержащий указатель на некоторое число и маску для вычисления определенного бита.
У этого объекта переопределен оператор operator=(bool), производящий запись в определенный бит.  
Обратите внимание, что неполучится использовать std::vector<bool> как обычный vector, а именно использовать указатели на элементы (нельзя указывать на отдельные биты). 

## LRU cache
[About](https://en.wikipedia.org/wiki/Cache_replacement_policies#LRU)
[Task](https://leetcode.com/problems/lru-cache/description/)
