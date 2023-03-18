# Семинар 6. Ranges.
## Мотивация
Сколько строк выведет код ниже?
```c++
#include <iostream>

int main() {
    const int days = 3;   // количество дней с играми
    const int games = 2;  // количество игр с питомцем в день

    for (int i = 0; i < days; i++) {
        std::cout << "День " << i << std::endl;
        for (int j = 0; j < games; i++) {
            std::cout << "  Игра " << j << std::endl;
        }
    }
}
```

Допустить такую ошибку при стандартном подходе к написанию циклов легко. Хочется уметь писать в таких ситуациях range-based for, как в питоне:
```python
days = 3   # количество дней с играми
games = 2  # количество игр с питомцем в день

for i in range(days):
    print("День %d" % i)
    for j in range(games):
        print("  Игра %d" % j)
```

Код при работе с итераторами часто получается громоздким:
```c++
std::merge(
  collection_holder.get_left_collection().begin(), 
  collection_holder.get_left_collection().end(), 
  collection_holder.get_right_collection().begin(), 
  collection_holder.get_right_collection().end(),
  std::back_inserter(
    collection_holder.get_destination_collection()
  )
);
```

А еще у нас есть сложности с реализацией нестандартных алгоритмов. Давайте попробуем написать transform_if с использованием stl и итераторов.

## std::ranges::
Начиная с C++20
```c++
#include <iostream>
#include <ranges>

namespace rng = std::ranges;
namespace view = rng::views;

int main() {
    const int days = 3;   // количество дней с играми
    const int games = 2;  // количество игр с питомцем в день

    for (int i : view::iota(0, days)) {
        std::cout << "День " << i << std::endl;
        for (int j : view::iota(0, games)) {
            std::cout << "  Игра " << j << std::endl;
        }
    }
}
```
Помимо этого можем реализовать transform_if
```c++
#include <iostream>
#include <ranges>
#include <vector>

namespace rng = std::ranges;
namespace view = rng::views;

int main() {
    auto even = [](int i) { return i % 2 == 0; };
    auto half = [](int i) { return i / 2; };
    
    std::vector<int> numbers_in = {100, 55, 80, 2, -1};
    auto numbers_out = numbers_in | view::filter(even) |  // <-- вся магия
                 view::transform(half);
    
    for (auto i : numbers_out) {
        std::cout << i << std::endl; // 50, 40, 1
    }
}
```

В библиотеке ranges есть реализация для большинства стандартных алгоритмов, можно теперь писать
```c++
std::vector v = { 4, 1, 7, 2, 3, 8 };
rng::sort(v);
```

При сортировке сложных объектов часто нужно сравнивать их по одному полю, чтобы не писать длинных компараторов, добавили "проекции" - это функции, которые будут вычислены от объектов перед сравнением. Пример:
```c++
struct Lecture {
    int course;
    int local_idx;
    int complexity;
};
std::vector<Lecture> ReadLectures();

namespace rng = std::ranges;

int main() {
    std::vector<Lecture> lectures = ReadLectures();

    // как теперь
    rng::sort(lectures, std::less<>{}, [](const Lecture& x) {
        return x.complexity;
    });

    return 0;
}
```
Проекции имеют тип Invocable и могут принимать указатели на члены. Например:
```c++
struct Account {
    std::string owner;
    double value();
    double base();
};
std::vector<Account> acc = get_accounts();
// member
std::ranges::sort(acc,{},&Account::owner);
// member function
std::ranges::sort(acc,{},&Account::value);
```

### Зависание итератора
В чем потенциальная проблема этого кода?
```c++
auto good = "1234567890";
auto sep1 = std::ranges::find(std::string_view(good), '0');
std::cout << *sep1 << "\n";
auto bad = 1234567890;
auto sep2 = std::ranges::find(std::to_string(bad), '0');
std::cout << *sep2 << "\n";
```

В варианте с диапазонами он не скомпилируется. Тут явно проверяется, что диапазон временный, и для него возвращается специальный итератор `std::ranges::dangling`. Обратите внимание, что проблема есть только во втором случае, пример с string_view полностью валидный, так как string_view не владеет объектом. Чтобы сделать ваш диапазон временным, необходимо специализировать константу:
```c++
template<typename T>
inline constexpr bool 
    std::ranges::enable_borrowed_range<MyView<T>> = true;
```

## std::ranges::views
Есть два способа работы с представлениями. Напрямую через создание объектов и перегрузку оператора `operator()`:
```c++
namespace rv = std::ranges::views;
std::vector<int> dt = {1, 2, 3, 4, 5, 6, 7};
for (int v : rv::reverse(rv::take(rv::reverse(dt),3))) {
    std::cout << v << ", ";
}
std::cout << "\n";
```
А также можно комбинировать представления используя оператор `operator|`:
```c++
namespace rv = std::ranges::views;
std::vector<int> dt = {1, 2, 3, 4, 5, 6, 7};
auto odd = [](std::integral auto v) { return v % 2 == 1; };
for (auto& v : dt | rv::filter(odd)) {
    v *= 2;
}
```
## Ссылки
[Реализация ranges v3](https://github.com/ericniebler/range-v3)
