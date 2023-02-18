# Семинар 2. Итераторы
## Про методы  `begin()` и `end()`
У стандартных контейнеров есть методы `begin()` и `end()`, которые позволяют нам "пройти" по контейнеру ничего не зная о его внутреннем устройстве. Посмотрим на примеры:
```c++
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v{ 1, 2, 3, 4, 5 };
    for (auto it = v.begin(); it != b.end(); ++it) {
        std::cout << *it << std::end;
    }
}
```
Аналогично выведем все элементы std::map:
```c++
#include <map>
#include <iostream>

int main() {
    std::map<int, float> num_map;
    num_map[3] = 3.14f;
    num_map[4] = 4.5f;
    num_map[5] = 5.55f;
    num_map[1] = 1.1f;

    for (auto it = num_map.begin(); it != num_map.end(); ++it) {
        std::cout << it->first << ", " << it->second << std::endl;
    }
}
```

Заметим, что вывод элементов контейнера практически не изменяется. При этом в первом случае мы имели дело с последовательным массивом чисел, а во втором с красно-черным деревом под капотом std::map.
Так как value_type у std::map - это std::pair<>, то мы обращались к его first и second членам. Узнать value_type контейнера можно на cppreference.

## Range-based for
Для стандартных контейнеров можем писать такой код:
```c++
void PrintVector(const std::vector<int>& v) {
    for (auto x : v) {
        std::cout << x << std::endl;
    }
}
```
Главное требование чтобы это было возможно - наличие у контейнера методов begin и end, возвращающих валидные итераторы.  
Написание range-based for эквивалетнтно следующему:
```c++
// until c++17
{
    auto && __range = range-expression ;
    for (auto __begin = begin-expr, __end = end-expr; __begin != __end; ++__begin)
    {
        range-declaration = *__begin;
        loop-statement
    }
}
```
```c++
// since c++17, until c++20
{
    auto && __range = range-expression ;
    auto __begin = begin-expr ;
    auto __end = end-expr ;
    for ( ; __begin != __end; ++__begin)
    {
        range-declaration = *__begin;
        loop-statement
    }
}
```
```c++
// since c++20
{
    init-statement
    auto && __range = range-expression ;
    auto __begin = begin-expr ;
    auto __end = end-expr ;
    for ( ; __begin != __end; ++__begin)
    {
        range-declaration = *__begin;
        loop-statement
    }
}
```
Чтобы избежать лишних копирований при работе с range-based циклами можно пользоватться следующей конструкцией:
```c++
struct cow_string { /* ... */ }; // a copy-on-write string
cow_string str = /* ... */;
 
// for (auto x : str) { /* ... */ } // may cause deep copy
 
for (auto x : std::as_const(str)) { /* ... */ }
```

Давайте напишем свой класс, поддерживающий такие циклы:
```c++
#include <iostream>
#include <cstring>
#include <stdexcept>

class FiveValuesHolder {
public:
    FiveValuesHolder(int values[], size_t size) {
        if (size < five_) {
            throw std::invalid_argument("Too small values array");
        }
        std::memcpy(values_, values, five_ * sizeof(int));
    }

    int* begin() {
        return values_;
    }

    int* end() {
        return values_ + five_;
    }
private:
    constexpr static int five_ = 5;

    int values_[five_];
};

int main() {
    int values[5] = { 1, 2, 3, 4, 5 };
    FiveValuesHolder fvh(values, 5);

    for (auto value : fvh) {
        std::cout << value << std::endl;
    }
}
```

## Иерархия итераторов (идейно)
1. Самый простой итератор - LegacyInputIterator. После того как мы инкрементируем этот итератор, все его прежние копии перестают быть валидными.
2. LegacyForwardIterator - остается валидным после инкремента копии. При равенстве двух итераторов гарантируется что они указывают на один и тот же объект. 
3. LegacyBidirectionalIterator - появляется возможность декремента итератора. Тут пример, когда мы не можем реализовать такой итератор - однозсвязный список. Замечание: --container.begin() - это UB.
4. LegacyRandomAccessIterator - появилась возможность перемещаться к любому элементу за константное время. Как пример - прибавлять к указателю любой сдвиг (в рамках контейнера, иначе - UB). Почему только тут появляется операция сравнения итераторов?
5. LegacyContiguousIterator - помимо прочего гарантируется, что смежные логически объекты смежны и в памяти.

## Стандартные алгоритмы на итераторах
### Простые алгоритмы
`find, find_if, count, count_if, all_of, any_of, for_each`
### Чуть более сложные
`remove, remove_if, replace, replace_if`
Часто применяется идиома remove-erase при работе с контейнерами. Пример:
```c++
#include <algorithm>  // remove and remove_if
#include <iostream>
#include <vector>  // the general-purpose vector container

bool IsOdd(int i) { return i & 1; }

void Print(const std::vector<int>& vec) {
  for (const auto& i : vec) {
    std::cout << i << ' ';
  }
  std::cout << '\n';
}

int main() {
  // Initializes a vector that holds numbers from 0-9.
  std::vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Print(v);

  // Removes all elements with the value 5.
  v.erase(std::remove(v.begin(), v.end(), 5), v.end());
  Print(v);

  // Removes all odd numbers.
  v.erase(std::remove_if(v.begin(), v.end(), IsOdd), v.end());
  Print(v);
}
```
### Еще более сложные
`std::sort, std::stable_sort, std::partition`
Почему нам требуется RandomAccessIterator?
Какой итератор подойдет для сортировки пузырьком?

## Посмотрим еще на итераторы
### Какие итераторы нужны и почему?
std::is_permutation, std::next_permutation
std::is_heap, std::make_heap
std::rotate, std::reverse
### Практика
Давайте реализуем rotate:
```c++
// TODO:
template<class ForwardIt>
constexpr // since C++20
ForwardIt // void until C++11
rotate(ForwardIt first, ForwardIt n_first, ForwardIt last)
{

}
```

Каких итераторов достаточно для решения задач?
* https://leetcode.com/problems/best-time-to-buy-and-sell-stock/
* https://leetcode.com/problems/best-time-to-buy-and-sell-stock-ii/
* https://leetcode.com/problems/majority-element/
* https://leetcode.com/problems/container-with-most-water/
* https://leetcode.com/problems/trapping-rain-water/
