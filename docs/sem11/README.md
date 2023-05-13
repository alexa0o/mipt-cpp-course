# Семинар 11. Lambdas.
## C++03
С самого начала стандартные алгоритмы могли принимать вызываемые объекты: указатели на функции или функторы.  
Пример:
```c++
#include <iostream>
#include <algorithm>
#include <vector>

struct PrintFunctor {
    void operator()(int x) const {
        std::cout << x << std::endl;
    }
};

int main() {
    std::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    std::for_each(v.begin(), v.end(), PrintFunctor());   
}
```
Существенная проблема, что мы не можем объявить функтор в одной области видимости с вызовом функции, так как инстанцирование шаблона с локальным типом невозможно.  
Пример:
```c++
int main() {
    struct PrintFunctor {
        void operator()(int x) const {
            std::cout << x << std::endl;
        }
    };

    std::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    std::for_each(v.begin(), v.end(), PrintFunctor());   
}
```

Вывод компилятора gcc с `-std=c++98`:
```
error: template argument for 
'template<class _IIter, class _Funct> _Funct 
std::for_each(_IIter, _IIter, _Funct)' 
uses local type 'main()::PrintFunctor'
```

## C++11
Учитывая ограничения выше, были добавлены специальные lambda-выражения. Они используют новый синтаксис, который расширяется компилятором до реального класса.  
Пример:
```c++
#include <iostream>
#include <algorithm>
#include <vector>

int main() {
    struct {
        void operator()(int x) const {
            std::cout << x << '\n';
        }
    } someInstance;

    std::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    std::for_each(v.begin(), v.end(), someInstance);
    std::for_each(v.begin(), v.end(), [] (int x) { 
            std::cout << x << '\n'; 
        }
    );    
}
```
Посмотрим, как компилятор расширит этот код:
```c++
#include <iostream>
#include <algorithm>
#include <vector>

int main()
{
  std::vector<int, std::allocator<int> > v = std::vector<int, std::allocator<int> >();
  v.push_back(1);
  v.push_back(2);
    
  class __lambda_10_15
  {
    public: 
    inline void operator()(int x) const
    {
      std::cout.operator<<(x).operator<<(std::endl);
    }
    
    using retType_10_15 = void (*)(int);
    inline operator retType_10_15 () const noexcept
    {
      return __invoke;
    };
    
    private: 
    static inline void __invoke(int x)
    {
      __lambda_10_15{}.operator()(x);
    }
    
    public: 
    // inline /*constexpr */ __lambda_10_15(const __lambda_10_15 &) noexcept = default;
    // inline /*constexpr */ __lambda_10_15(__lambda_10_15 &&) noexcept = default;
    
  };
  
  __lambda_10_15 lam = __lambda_10_15(__lambda_10_15{});
  std::for_each(__gnu_cxx::__normal_iterator<int *, std::vector<int, std::allocator<int> > >(v.begin()), __gnu_cxx::__normal_iterator<int *, std::vector<int, std::allocator<int> > >(v.end()), __lambda_10_15(lam));
  return 0;
}
```

Получили преобразование лямбды в что-то похожее на:
```c++
struct {
    void operator()(int x) const {
        std::cout << x << '\n';
    }
} someInstance;
```

Синтаксис:
```c++
[] ()   { код; }
^  ^  ^
|  |  |
|  |  опционально: mutable, exception, trailing return, ...
|  |
|  список параметров
|
объявитель лямбды со списком захвата
```

Вычисление лямбда-выражения приводит к временному объекту-замыканию, тип которого является уникальным типом замыкания.  
Примеры:
```c++
[](float f, int a) { return a*f; }
[](MyClass t) -> int { auto a = t.compute(); return a; }
[](int a, int b) { return a < b; }
```

Давайте попробуем создать объект такого же типа, как и лямбда:
```c++
auto foo = [&x, &y]() { ++x; ++y; };
decltype(foo) fooCopy;
```

Компилятор:
```
error: use of deleted function 'main()::<lambda()>::<lambda>()'
       decltype(foo) fooCopy;
                   ^~~~~~~
note: a lambda closure type has a deleted default constructor
```

Код из тела лямбды "транслируется" в тело оператора круглые скобки в классе-замыкании. По умолчанию оператор константный, но мы можем явно убрать константность:
```c++
auto myLambda = [](int a) mutable { std::cout << a; }
```
Нужно это для ненулевого списка захвата.

### Захват переменных
В синтаксисе лямбды можно описать переменные из области видимости доступ к которым мы хотим иметь из тела лямбды. Соответственно, мы получим член копию внутри класса-замыкания.
Синтакис:
[&] — захват по ссылке, все переменные в автоматическом хранилище объявлены в области охвата
[=] — захват по значению, значение копируется
[x, & y] — явно захватывает x по значению, а y по ссылке

Посмотрим на пример:
```c++
#include <iostream>

int main () {
	int x = 1, y = 1;
	{
    	std::cout << x << " " << y << std::endl;
    	auto foo = [&x, &y]() { ++x; ++y; };
    	foo();
    	std::cout << x << " " << y << std::endl;
	}
}
```

Будет преобразованно в:
```c++
#include <iostream>

int main()
{
  int x = 1;
  int y = 1;
  {
    std::operator<<(std::cout.operator<<(x), " ").operator<<(y).operator<<(std::endl);
        
    class __lambda_7_17
    {
      public: 
      inline void operator()() const
      {
        ++x;
        ++y;
      }
      
      private: 
      int & x;
      int & y;
      public: 
      // inline /*constexpr */ __lambda_7_17(__lambda_7_17 &&) noexcept = default;
      __lambda_7_17(int & _x, int & _y)
      : x{_x}
      , y{_y}
      {}
      
    };
    
    __lambda_7_17 foo = __lambda_7_17(__lambda_7_17{x, y});
    foo.operator()();
    std::operator<<(std::cout.operator<<(x), " ").operator<<(y).operator<<(std::endl);
  };
  return 0;
}
```

Пример с захватом по значению:
```c++
#include <iostream>

int main () {
	int x = 1, y = 1;
	{
    	std::cout << x << " " << y << std::endl;
    	auto foo = [&x, y]() mutable { ++x; ++y; };
    	foo();
    	std::cout << x << " " << y << std::endl;
	}
}
```

Получим:
```c++
#include <iostream>

int main()
{
  int x = 1;
  int y = 1;
  {
    std::operator<<(std::cout.operator<<(x), " ").operator<<(y).operator<<(std::endl);
        
    class __lambda_7_17
    {
      public: 
      inline void operator()()
      {
        ++x;
        ++y;
      }
      
      private: 
      int & x;
      int y;
      public: 
      // inline /*constexpr */ __lambda_7_17(__lambda_7_17 &&) noexcept = default;
      __lambda_7_17(int & _x, int & _y)
      : x{_x}
      , y{_y}
      {}
      
    };
    
    __lambda_7_17 foo = __lambda_7_17(__lambda_7_17{x, y});
    foo.operator()();
    std::operator<<(std::cout.operator<<(x), " ").operator<<(y).operator<<(std::endl);
  };
  return 0;
}
```

Важно: замыкания С++ не увеличивают время жизни захваченных ссылок. Лучше избегать захвата по умолчанию во избежание нежелательных спецэффектов.

Захват не работает с глобальными переменными, в случае если мы будем пытаться захватить их по умолчанию, то не получим предпреждений от компилятора, в случае же с явным захватом можно расчитывать на warning или error.  
Пример:
```c++
int global = 10;

int main()
{
    std::cout << global << std::endl;
    auto foo = [=] () mutable { ++global; };
    foo();
    std::cout << global << std::endl;
    [] { ++global; } ();
    std::cout << global << std::endl;
    [global] { ++global; } ();
}
```

А что со статическими переменными?  
```c++
#include <iostream>

void bar()
{
    static int static_int = 10;
    std::cout << static_int << std::endl;
    auto foo = [=] () mutable { ++static_int; };
    foo();
    std::cout << static_int << std::endl;
    [] { ++static_int; } ();
    std::cout << static_int << std::endl;
    [static_int] { ++static_int; } ();
}

int main()
{
   bar();
}
```

Вывод:
```
10
11
12
```

### Захват члена класса
```c++
#include <iostream>
#include <functional>

struct Baz
{
    std::function<void()> foo()
    {
        return [=] { std::cout << s << std::endl; };
    }

    std::string s;
};

int main()
{
   auto f1 = Baz{"ala"}.foo();
   auto f2 = Baz{"ula"}.foo(); 
   f1();
   f2();
}
```
Посмотрим как поступит компилятор:
```c++
#include <iostream>
#include <functional>

struct Baz
{
  inline std::function<void ()> foo()
  {
        
    class __lambda_8_16
    {
      public: 
      inline void operator()() const
      {
        std::operator<<(std::cout, __this->s).operator<<(std::endl);
      }
      
      private: 
      Baz * __this;
      public: 
      // inline /*constexpr */ __lambda_8_16(const __lambda_8_16 &) noexcept = default;
      // inline /*constexpr */ __lambda_8_16(__lambda_8_16 &&) noexcept = default;
      __lambda_8_16(Baz * _this)
      : __this{_this}
      {}
      
    } __lambda_8_16{this};
    
    return std::function<void ()>(std::function<void ()>(__lambda_8_16));
  }
  
  std::basic_string<char> s;
  // inline ~Baz() noexcept = default;
};



int main()
{
  std::function<void ()> f1 = std::function<void ()>(Baz{std::basic_string<char>(std::basic_string<char>("ala", std::allocator<char>()))}.foo());
  std::function<void ()> f2 = std::function<void ()>(Baz{std::basic_string<char>(std::basic_string<char>("ula", std::allocator<char>()))}.foo());
  f1.operator()();
  f2.operator()();
  return 0;
}
```

Получим UB. Чтобы его избежать, можно явно указать что мы хотим захватить:
```c++
std::function<void()> foo()
{
    return [s] { std::cout << s << std::endl; };
}
```

Тогда компилятор честно выведет:
```
In member function 'std::function<void()> Baz::foo()':
error: capture of non-variable 'Baz::s'
error: 'this' was not captured for this lambda function
...
```

### Move-able-only объекты
Если у вас есть объект, который может быть только перемещен (например, unique_ptr), то вы не можете поместить его в лямбду в качестве захваченной переменной. Захват по значению не работает, поэтому вы можете захватывать только по ссылке… однако это не передаст его вам во владение, и, вероятно, это не то, что вы хотели.
```c++
std::unique_ptr<int> p(new int[10]);
auto foo = [p] () {}; // не компилируется....
```

### Захват констант
При захвате константность сохраняется:
```c++
int const x = 10;
auto foo = [x] () mutable { 
    std::cout << std::is_const<decltype(x)>::value << std::endl;
    x = 11;
};
foo();
```

### Вывод возвращаемого типа
Если результаты всех операторов return могут быть преобразованы к одному типу, то он будет выведен в качестве возвращаемого типа лямбды.

Лямбды можно вызывать сразу после определения:
```c++
int x = 1, y = 1;
[&]() { ++x; ++y; }(); // <-- call ()
std::cout << x << " " << y << std::endl;
```

Это часто бывает полезно при сложном определении константы. 

Замыкания без захвата могут преобразовываться в указатели на функции:
```c++
#include <iostream>

void callWith10(void(* bar)(int))
{
    bar(10);
}

int main()
{
    struct 
    {
        using f_ptr = void(*)(int);

        void operator()(int s) const { return call(s); }
        operator f_ptr() const { return &call; }

    private:
        static void call(int s) { std::cout << s << std::endl; };
    } baz;

    callWith10(baz);
    callWith10([](int x) { std::cout << x << std::endl; });
}
```

## C++14
Появился захват с инициализатором, то есть мы создаем новую переменную-член класса:
```c++
int main() {
    int x = 10;
    int y = 11;
    auto foo = [z = x+y]() { std::cout << z << '\n'; };
    foo();
}
```
Это решает проблему с типами, доступными только для перемещения:
```c++
#include <memory>

int main()
{
    std::unique_ptr<int> p(new int[10]);
    auto foo = [x=10] () mutable { ++x; };
    auto bar = [ptr=std::move(p)] {};
    auto baz = [p=std::move(p)] {};
}
```

Можем оптимизировать код, предподсчитывая какие-то значения в списке захвата:
```c++
#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <iostream>
#include <string>

int main()
{
    using namespace std::string_literals;
    std::vector<std::string> vs;
    std::find_if(vs.begin(), vs.end(), [](std::string const& s) {
     return s == "foo"s + "bar"s; });
    std::find_if(vs.begin(), vs.end(), [p="foo"s + "bar"s](std::string const& s) { return s == p; });
}
```

Решаем проблему с захватом членов класса:
```c++
struct Baz
{
    auto foo()
    {
        return [s=s] { std::cout << s << std::endl; };
    }

    std::string s;
};

int main()
{
   auto f1 = Baz{"ala"}.foo();
   auto f2 = Baz{"ula"}.foo(); 
   f1();
   f2();
}
```

Кроме того, мы используем auto для вывода всего метода (ранее, в C++11 мы могли использовать std::function).

Появились обобщенные лямбды:
```c++
auto foo = [](auto x) { std::cout << x << '\n'; };
foo(10);
foo(10.1234);
foo("hello world");
```

Это эквивалетно:
```c++
struct {
    template<typename T>
    void operator()(T x) const {
        std::cout << x << '\n';
    }
} someInstance;
```

Еще пример:
```c++
std::map<std::string, int> numbers { 
    { "one", 1 }, {"two", 2 }, { "three", 3 }
};

// каждый раз запись копируется из pair<const string, int>!
std::for_each(std::begin(numbers), std::end(numbers), 
    [](const std::pair<std::string, int>& entry) {
        std::cout << entry.first << " = " << entry.second << '\n';
    }
);
```
Все ли ок?  





Не совсем, так как типом значения для `std::map` является `std::pair<const Key, T>`, то мы будем делать лишние копии. Можно избегать таких ошибок:
```c++
std::for_each(std::begin(numbers), std::end(numbers), 
    [](auto& entry) {
        std::cout << entry.first << " = " << entry.second << '\n';
    }
);
```

## C++17
Лямбды теперь могут быть constexpr:
```c++
constexpr auto Square = [] (int n) { return n*n; }; // implicitly constexpr
static_assert(Square(2) == 4);
```

Строго: оператор вызова функции является функцией constexpr, если за объявлением параметра условия соответствующего лямбда-выражения следует constexpr, или он удовлетворяет требованиям для функции constexpr.  
Еще пример:
```c++
template<typename Range, typename Func, typename T>
constexpr T SimpleAccumulate(const Range& range, Func func, T init) {
    for (auto &&elem: range) {
        init += func(elem);
    }
    return init;
}

int main() {
    constexpr std::array arr{ 1, 2, 3 };

    static_assert(SimpleAccumulate(arr, [](int i) { 
            return i * i; 
        }, 0) == 14);
}
```

Можно так же захватывать переменные, конечно при условии что они тоже constexpr:
```c++
constexpr int add(int const& t, int const& u) {
    return t + u;
}

int main() {
    constexpr int x = 0;
    constexpr auto lam = [x](int n) { return add(x, n); };

    static_assert(lam(10) == 10);
}
```

Что не так с этим кодом?
```c++
constexpr int x = 0;
constexpr auto lam = [x](int n) { return n + x };
```

В clang получим следующее:
```
warning: lambda capture 'x' is not required to be captured for this use
```

Начиная с С++20 большинство стандартных алгоритмов и даже  контейнеров constexpr.

### Захват *this
Теперь можем захватывать члены класса по значению:
```c++
#include <iostream>

struct Baz {
    auto foo() {
        return [*this] { std::cout << s << std::endl; };
    }

    std::string s;
};

int main() {
   auto f1 = Baz{"ala"}.foo();
   auto f2 = Baz{"ula"}.foo(); 
   f1();
   f2();
}
```

Получим следубщий класс Baz:
```c++
struct Baz
{
  inline __lambda_6_16 foo()
  {
        
    class __lambda_6_16
    {
      public: 
      inline /*constexpr */ void operator()() const
      {
        std::operator<<(std::cout, (&__this)->s).operator<<(std::endl);
      }
      
      private: 
      Baz __this;
      
      public:
      __lambda_6_16(const Baz & _this)
      : __this{_this}
      {}
      
    } __lambda_6_16{*this};
    
    return __lambda_6_16;
  }
  
  std::basic_string<char> s;
  // inline Baz(const Baz &) noexcept(false) = default;
  // inline ~Baz() noexcept = default;
};
```

Захват с инициализатором позволял сохранить переменную-член, но что насчет методов?
```c++
struct Baz {
    auto foo() {
        return [this] { print(); };
    }

    void print() const { std::cout << s << '\n'; }

    std::string s;
};
```

```c++
auto foo() {
    return [self=*this] { self.print(); };
}

Но в C ++ 17 это можно сделать чище:

auto foo() {
    return [*this] { print(); };
}
```

Неявный захват this помечен как deprecated в С++20, теперь пишем `[=, this]`.

Посмотрим на лямбду:
```c++
auto foo = [] (const auto& vec) { 
        std::cout<< std::size(vec) << '\n';
        std::cout<< vec.capacity() << '\n';
};
```

Что если я передам в нее int?
```
prog.cc: In instantiation of 'main()::<lambda(const auto:1&)> [with auto:1 = int]':
prog.cc:16:11:   required from here
prog.cc:11:30: error: no matching function for call to 'size(const int&)'
   11 |         std::cout<< std::size(vec) << '\n';
```

В С++20 можно написать:
```c++
auto foo = []<typename T>(std::vector<T> const& vec) { 
        std::cout<< std::size(vec) << '\n';
        std::cout<< vec.capacity() << '\n';
};
```

И в случае с интом получить:
```
note:   mismatched types 'const std::vector<T>' and 'int'
```

## Бонус
```c++
void foo(int) {}
void foo(float) {}

int main()
{
  std::vector<int> vi;
  std::for_each(vi.begin(), vi.end(), foo);
}
```

Как быть?
