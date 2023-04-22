# Семинар 10. Вывод типов.
## Структурное связывание (structured binding)
В C++17 появилась возможность писать так:
```c++
for (const auto& [key, value] : map) {
    std::cout << key << ": " << value << std::endl;
}
```

Можем использовать для ассоциативных контейнеров, обычных контейнеров, массивов и даже пользовательских структур и классов! 

Посмотрим на пример:
```c++
const volatile auto && [a,b,c] = Foo{};
```
Ожидаем, что a, b, c будут иметь все указанные спецификаторы, но тут все не так однозначно. Компилятор "заменяет" список переменных на имя для переменной, в которую поместим тип вычесленного выражения:
```c++
const volatile auto && e = Foo{};
```

А вот a, b, c будут неявно ссылаться на поля `e`. При этом `decltype(a)` не будет ссылочным (только если сами поля не являются ссылками). 
```c++
std::tuple<int, float> t(1, 2.f);
auto& [a, b] = t; // decltype(a) — int, decltype(b) — float
++a; // изменяет, как «по ссылке», первый элемент t
std::cout << std::get<0>(t); // выведет 2
```

Структурное связывание работает с:
 1. Массивами.  
 2. tuple-like типами. 
 3. Структурами.  

Поговорим подробнее про 2 случай:
Тип Е должен поддерживать интерфейс кортежей:
```c++
std::tuple_size<E>
std::tuple_element<i, E>
get<i>({e}); // или {e}.get<i>()
```
В таком случае типом переменной будет `std::tuple_element_t<i, E>`.

Подробнее про пункт 3:
Так как разобрать внутреннюю структуру класса - сложная задача даже для компилятора, то далеко не каждую структуру можно разобрать. Чтобы иметь возможность разбора структуры, необходимо чтобы выполнялось следующее условие:
Все внутренние нестатические поля класса должны быть из одного базового класса, и они должны быть доступны на момент использования.

```c++
// Примеры «простых» классов
struct A { int a; }; 
struct B : A {}; 
struct C : A { int c; }; 
class D { int d; };

auto [a] = A{}; // работает (a -> A::a) 
auto [a] = B{}; // работает (a -> B::A::a)
auto [a, c] = C{}; // ошибка: a и c из разных классов
auto [d] = D{}; // ошибка: d — private

void D::foo()
{
    auto [d] = *this; // работает (d доступен внутри класса)
}
```

Реализуем поддержку интерфейса кортежей:
```c++
// Небольшой класс, который должен возвращать ссылку на int при связывании

class Foo;

template<>
struct std::tuple_size<Foo> : std::integral_constant<std::size_t, 1> {};

template<>
struct std::tuple_element<0, Foo>
{
    using type = int&;
};

class Foo
{
public:
    template<std::size_t i>
    std::tuple_element_t<i, Foo> const& get() const;

    template<std::size_t i>
    std::tuple_element_t<i, Foo> & get();

private:
    int _foo = 0;
    int& _bar = _foo;
};

template<>
std::tuple_element_t<0, Foo> const& Foo::get<0>() const
{
    return _bar;
}

template<>
std::tuple_element_t<0, Foo> & Foo::get<0>()
{
    return _bar;
}
```

Теперь связываем:
```c++
Foo foo;
const auto& [f1] = foo;
const auto  [f2] = foo;
auto& [f3] = foo;
auto  [f4] = foo;
```

Магия:
```c++
decltype(f1); // int&
decltype(f2); // int&
decltype(f3); // int&
decltype(f4); // int&	
++f1; // это сработает и поменяет foo._foo, хотя {e} должен был быть const
```

Почему так вышло? Ответ в специализации по умолчанию:
```c++
template<std::size_t i, class T>
struct std::tuple_element<i, const T>
{
    using type = std::add_const_t<std::tuple_element_t<i, T>>;
};
```

Чтобы починить проблему в нашей реализации достаточно добавить такую спецификацию:
```c++
template<>
struct std::tuple_element<0, const Foo>
{
    using type = const int&;
};
```

Тогда получим:
```c++
decltype(f1); // const int&
decltype(f2); // const int&
decltype(f3); // int&
decltype(f4); // int&
++f1; // это уже не сработает
```

