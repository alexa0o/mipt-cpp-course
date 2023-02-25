# Семинар 3. Реализация и использование итераторов.
## Итератор для std::vector
Точно не стоит использовать указатели на элементы вектора при реализации. Вспомним, что при досижении максимального размера вектор удваивается в размере и указатели могут инвалидироваться. Вместо указателей будем сохранять индексы, они не будут меняться со временем. Если конечно кто-то не станет удалять элементы из вектора.  
Давайте попробуем написать простую реализацию итератора для вектора.
```c++
#include <vector>

template <class T>
class VectorIterator {
public:
    VectorIterator(std::vector<T>& v, size_t indx = 0)
    : v_(v), indx_(indx) { }

    VectorIterator<T> operator+(size_t value) {
        return VectorIterator<T>{v_, indx_ + value};
    }

    VectorIterator<T>& operator+=(size_t value) {
        indx_ += value;
        return *this;
    }

private:
    std::vector<T>& v_;
    size_t indx_;
};
```

Умеем создавать итератор и прибавлять к нему константу. Давайте добавим возможность получать доступ до элемента:
```c++
    T& operator*() {
        return v_[indx_];
    }

    T* operator->() {
        return &v_[indx_];
    }
```
Пришло время подумать о константах. Чтобы реализовать константный итератор добавим шаблонный параметр к итератору IsConst и воспользуемся `std::conditional_t` (чуть поже поговорим о его реализации).
```c++
template <class T, bool IsConst = false>
class VectorIterator {
public:
    using Type = std::conditional_t<IsConst, const T, T>;

    VectorIterator(std::vector<T>& v, size_t indx = 0)
    : v_(v), indx_(indx) { }

    VectorIterator<T, IsConst> operator+(size_t value) {
        return VectorIterator<T, IsConst>{v_, indx_ + value};
    }

    VectorIterator<T, IsConst>& operator+=(size_t value) {
        indx_ += value;
        return *this;
    }

    Type& operator*() {
        return v_[indx_];
    }

    Type* operator->() {
        return &v_[indx_];
    }

private:
    std::vector<T>& v_;
    size_t indx_;
};
```

Что же такое `std::conditional_t`? Давайте посмотрим:
```c++
template<bool B, class T, class F>
struct conditional { using type = T; };
 
template<class T, class F>
struct conditional<false, T, F> { using type = F; };

template< bool B, class T, class F >
using conditional_t = typename conditional<B,T,F>::type;
```
Следующий шаг - поддержать возможность кастовать обычный итератор к константному и запретить обратное преобразование.
```c++
    template <bool IsFromConst>
    VectorIterator(VectorIterator<T, IsFromConst>& vi)
    : v_(vi.v_), indx_(vi.indx_) {
        static_assert(IsConst || IsFromConst == false, "Create non-const iterator from const");
    }
```
Обратите внимание, что VectorIterator<T, true> и VectorIterator<T, false> - это разные классы, поэтому для того чтобы иметь доступы из них друг в друга нужно сделать их друзьями.
```c++
    friend VectorIterator<T, !IsConst>;
```
Для упрощения дальнейшей работы с итератором добавим методы GetBegin, GetCBegin, GetEnd, GetCEnd:
```c++
template <class T>
VectorIterator<T> GetBegin(std::vector<T>& v) {
    return VectorIterator<T>(v, 0);
}

template <class T>
VectorIterator<T> GetEnd(std::vector<T>& v) {
    return VectorIterator<T>(v, v.size());
}

template <class T>
VectorIterator<T, true> GetCBegin(const std::vector<T>& v) {
    return VectorIterator<T, true>(v, 0);
}

template <class T>
VectorIterator<T, true> GetCEnd(const std::vector<T>& v) {
    return VectorIterator<T, true>(v, v.size());
}
```
Для корректной работы константного итератора не хватает возможности хранить константный вектор, исправим это:
```c++
    using VectorType = std::conditional_t<IsConst, const std::vector<T>, std::vector<T>>;

    VectorIterator(VectorType& v, size_t indx = 0)
    : v_(v), indx_(indx) { }

private:
    VectorType& v_;
    size_t indx_;
```

## reverse - итератор
Хотим уметь обходить контейнер от конца к началу, но при этом не хотим делать копипасту для уже реализованного итератора. На помощь приходит `template std::reverse_iterator<class Iter>;`, меняющий местами операторы инкремента и декремента. Посмотри на пример реализации с cppreference:
```c++
template<class It>
class reverse_iterator
{
protected:
    It current = It();
public:
    reverse_iterator() = default;
    constexpr explicit reverse_iterator(It itr) : current(itr) {}
    constexpr explicit reverse_iterator(const U& other) : current(other.base()) {}
 
    constexpr reverse_iterator& operator++() { --current; return *this; }
    constexpr reverse_iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
 
    constexpr reverse_iterator& operator--() { ++current; return *this; }
    constexpr reverse_iterator operator--(int) { auto tmp = *this; --(*this); return tmp; }
 
    constexpr It base() const { return current; }
 
    // Other member functions, friend functions, and member typedefs are not shown here.
};
```
## output - итераторы
Рассмотри следующий пример:
```c++
std::list<int> l = {1, 2, 3, 4};
std::vector<int> v;
std::copy(l.begin(), l.end(), v.begin());
```
В чем проблема? Получаем UB, нужен более умный итератор для такого копирования.
Воспользуемся std::back_insert_iterator:
```c++
std::list<int> l = {1, 2, 3, 4};
std::vector<int> v;
std::copy(l.begin(), l.end(), std::back_inserter(v));
```
std::backk_inserter - это удобная функция для создания итератора.  
Посмотри на реализацию:
```c++
template <typename Container>
class back_insert_iterator {
 public:
  back_insert_iterator(Container& container): container(container) {}

  back_insert_iterator<Container>& operator++() {
    return *this;
  }

  back_insert_iterator<Container>& operator*() {
    return *this;
  }

  back_insert_iterator<Container>& operator=(const typename container::value_type& value) {
    container.push_back(value);
    return *this;
  }
 private:
  Container& container;
}

template <typename Container>
back_insert_iterator<Container< back_inserter(Container& container) {
  return back_insert_iterator<Container>(container);
}
```
Напомним тут же и реализацию std::copy:
```c++
template<class InputIt, class OutputIt>
OutputIt copy(InputIt first, InputIt last,
              OutputIt d_first)
{
    for (; first != last; (void)++first, (void)++d_first)
        *d_first = *first;
 
    return d_first;
}
```

Как можно построить пересечение двух отсотрированных массивов? А двух `std::set`?
```c++
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
 
int main()
{
    std::vector<int> v1{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> v2{            5,    7,    9, 10};
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
 
    std::vector<int> v_intersection;
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(),
                          std::back_inserter(v_intersection));
 
    for (int n : v_intersection)
        std::cout << n << ' ';
}
```

## std::advance и std::distance
std::advance - очень удобная функция для работы с InputIt, которая инкрементирует (декрементирует) итератор n раз:
```c++
template<class It, class Distance>
constexpr void advance(It& it, Distance n)
{
    using category = typename std::iterator_traits<It>::iterator_category;
    static_assert(std::is_base_of_v<std::input_iterator_tag, category>);
 
    auto dist = typename std::iterator_traits<It>::difference_type(n);
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, category>)
        it += dist;
    else
    {
        while (dist > 0)
        {
            --dist;
            ++it;
        }
        if constexpr (std::is_base_of_v<std::bidirectional_iterator_tag, category>)
        {
            while (dist < 0)
            {
                ++dist;
                --it;
            }
        }
    }
}
```
Обратим внимание на `std::iterator_traits<It>::iterator_category`, эта штука позволяет нам узнать к какому типу принадлежит итератор. Соответственно для работы с пользовательскими итераторами, неоходимо перегрузить `std::iterator_traits`.

std::distance - функция, отвечающая на вопрос: сколько шагов между двумя итераторами? Обратим внимание, что для разных типов итераторов она имеет разное время работы. Есть два способа поддержать это:
1. С помощью перегрузок и диспетчеризации по тегу:
```c++
namespace detail {
 
template<class It>
typename std::iterator_traits<It>::difference_type 
    do_distance(It first, It last, std::input_iterator_tag)
{
    typename std::iterator_traits<It>::difference_type result = 0;
    while (first != last) {
        ++first;
        ++result;
    }
    return result;
}
 
template<class It>
typename std::iterator_traits<It>::difference_type 
    do_distance(It first, It last, std::random_access_iterator_tag)
{
    return last - first;
}
 
} // namespace detail
 
template<class It>
typename std::iterator_traits<It>::difference_type 
    distance(It first, It last)
{
    return detail::do_distance(first, last,
                               typename std::iterator_traits<It>::iterator_category());
}
```
2. constexpr
```c++
template<class It>
constexpr typename std::iterator_traits<It>::difference_type
    distance(It first, It last)
{
    using category = typename std::iterator_traits<It>::iterator_category;
    static_assert(std::is_base_of_v<std::input_iterator_tag, category>);
 
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, category>)
        return last - first;
    else {
        typename std::iterator_traits<It>::difference_type result = 0;
        while (first != last) {
            ++first;
            ++result;
        }
        return result;
    }
}
```
Что будет если второй итератор недостижим из первого? UB.

## Практика
1. Давайте напишем std::inserter
2. Перегрузить iterator_traits для пользовательского итератора
3. Реализовать std::prev
4. Написать reverse_iterator для RandomAccessIterator'a
5. Можно ли написать итератор для std::cin? Давайте попробуем!
