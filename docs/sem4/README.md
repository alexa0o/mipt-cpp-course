# Семинар 4. Потоки и итераторы по потокам.
## std::fstream и std::stringstream
### std::fstream
Мы уже умеем работать с потоками для чтения и вывода в консоль. Помимо этого в C++ есть другие типы потоков, дающие возможность работать с файлами, строками и тп.  
Пример чтения слов из файла:
```c++
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("file.cpp");
    std::string word;
    while (in >> word) {
        std::cout << word << std::endl;
    }
}
```
Чтобы не пропустить отсутствие файла, рекомендуется использовать метод `in.is_open()`.
Можно в конструктор передать второй аргумент типа `std::ios_base::openmode` - это специализированные константы, меняющие формат работы с потоком, подробнее читать тут https://en.cppreference.com/w/cpp/io/ios_base/openmode  

### std::stringstream
С помощью потоков можно записывать данные прямо в строки. Давайте решим задачу:  
Есть `std::map<int, float>` где ключ - уникальный идентификатор (id), а float - значение (value). Нужно вывести их в строку вида: "id:value;id:value;...", где у value максимум 6 значащих цифр.
```c++
std::string FormatMap(const std::map<int, float>& m) {
    std::stringstream ss;
    ss << std::setprecision(6);
    for (const auto& it : m) {
        ss << it.first << ':' << it.second << ';';
    }
    return ss.str();
}
```
Можно использовать std::stringstream для преобразования типов:
```c++
int StringToInt(const std::string& s) {
    std::stringstream ss;
    ss << s;
    int k = 0;
    ss >> k;
    return k;
}
```
Также можно создать простой хелпер для быстрого создания строки и потоковой записи в нее:
```c++
class MakeString {
public:
    template<class T>
    MakeString& operator<< (const T& arg) {
        m_stream << arg;
        return *this;
    }
    operator std::string() const {
        return m_stream.str();
    }
protected:
    std::stringstream m_stream;
};

void func(int id, const std::string& data1, const std::string& data2) {
    throw std::runtime_error(MakeString() << "Operation with id = " << id << " failed, because data1 (" << data1 << ") is incompatible with data2 (" << data2 << ")");
}
```

## Итераторы по потокам
Давайте заведем итератор на std::cin:
```c++
std::istream_iterator<int> it(std::cin);

for (int i = 0; i < 3; ++i) {
  std::cout << *it;
  ++it;
}
```
Как реализовать istream_iterator? К какому типу итераторов он относится?

Еще пример итераторов по потокам:
```c++
#include <iostream>
#include <sstream>
#include <iterator>
#include <numeric>
#include <algorithm>
 
int main()
{
    std::istringstream str("0.1 0.2 0.3 0.4");
    std::partial_sum(std::istream_iterator<double>(str),
                     std::istream_iterator<double>(),
                     std::ostream_iterator<double>(std::cout, " "));
 
    std::istringstream str2("1 3 5 7 8 9 10");
    auto it = std::find_if(std::istream_iterator<int>(str2),
                      std::istream_iterator<int>(),
                      [](int i){return i%2 == 0;});
    if (it != std::istream_iterator<int>())
        std::cout << "\nThe first even number is " << *it << ".\n";
    //" 9 10" left in the stream
}
```

## std::string_view
Способ дешево и удобно работать с константными строками, как правило хранит внутри указатель и размер. Не владеет строкой!
```c++
#include <iostream>
#include <string_view>
 
int main()
{
    constexpr std::string_view unicode[] { "▀▄─", "▄▀─", "▀─▄", "▄─▀" };
 
    for (int y{}, p{}; y != 6; ++y, p = ((p + 1) % 4))
    {
        for (int x{}; x != 16; ++x)
            std::cout << unicode[p];
        std::cout << '\n';
    }
}
```
Обратите внимание, что std::string_view поддерживает такие методы как remove_prefix и remove_suffix. Как это работает?

Также есть метод substr, его ассимптотика О(1)!

Если исходная строка исчезнет во время работы с string_view, то это UB.

## std::span
Аналогичный string_view способ смотреть на последовательные массивы различных типов (от классических С массивов до std::vector). Появился в С++20. Простейше можно представить в следующем виде:
```c++
struct span<T>{
    T*  pointer; // start of sequence
    size_t size; // size of sequence
}
```
std::span из классического массива:
```c++
int a[2]={4,3};
std::span<int> s = a;
std::cout<< s[0] <<s[1];// 4 3
```
Из указателя:
```c++
int* a = new int[2]{8, 8};
std::span<int> s{a, 2}; // size=2
```
std::array
```c++
std::array<int,2> a = {5,6};
std::span<int> s = a;
```
std::vector
```c++
std::vector<int> a = {1,1};
std::span<int> s = a;
```
```c++
std::vector<int> v = {1,2,3,4};
std::span<int> s{a.data()+2,2};
std::cout<< s[0]<<s[1]<<'\n'; // 3 4
```
Компилятор может автоматически понять какой тип нужен
```c++
std::vector<int> a = {1,1};
std::span s = a; // span<int>
```

Что можно делать еще?
```c++
std::vector<int> v = {1,2,3,4};
std::span s = v;

for (auto& item:s)
        std::cout<<item;

auto a = s.front(); // first element of sequence
auto b = s.back(); // last element of sequence
auto c = s[2]; // access via []
auto d = s.data(); // pointer to first element

auto e = s.size();
bool f = s.empty();

auto g = s.begin(); // iterator to begining
auto h = s.end();// iterator to end
```

Обратите внимание на отсутствие cbegin, cend. 

Можем изменять значения элементов:
```c++
template<typename T, std::size_t length>
void change(std::span<T, length> span) {
    std::transform(span.begin(), span.end(), span.begin(), std::negate());
}

int main() {
    std::array data = {1,2,3,4,5};

    print(std::span{data});
    change(std::span{data});
    print(std::span{data});
}
```

Можем смотреть на часть
```c++
int main() {
    std::array data = {1, 2, 3, 4, 5};

    auto span = std::span{data};
    print(span);

    print(span.first(3));
    print(span.last(3));
    print(span.subspan(1, 3));
}
```

std::span может быть динамический и статический, в зависимости от того была ли известна его длина в compile time:
```c++
template<typename T, std::size_t length>
void print_extent(std::span<T, length> span) {
    std::cout << std::hex << "0x" << span.extent << '\n';
}

int main() {
    std::array array = {1,2,3,4,5};

    auto span = std::span{array};
    print_extent(span);

    print_extent(span.subspan(1, 3));

    std::vector<int> vector;
    print_extent(std::span{vector});

    int c_array[] = {42};
    print_extent(std::span{c_array});
}
```
Вывод:
```
0x5
0xffffffffffffffff
0xffffffffffffffff
0x1
```
Можем убедиться, что для вектора используется dynamic_extent, но вот для subspan от array тоже dynamic_extent, почему?

Можем статический span привести к динамическому:
```c++
#include <iostream>
#include <span>

int main() {
    char buffer[] = "hello, world";

    std::span<char, 13> static_span{buffer};
    std::span<char> dynamic_span{static_span};

    std::cout << std::boolalpha;

    std::cout << (static_span.extent == std::dynamic_extent) <<  '\n';
    std::cout << (dynamic_span.extent == std::dynamic_extent) <<  '\n';
}
```
Почему это интересно?
Можем писать не шаблонные функции, принимающие span как параметр:
```c++
#include <array>
#include <iostream>
#include <span>

void print(std::span<char> span) {
   for(auto c : span) std::cout << c;
   std::cout << '\n';
}

template<std::size_t size>
void print(std::array<char, size> array) {
   for(auto c : array) std::cout << c;
   std::cout << '\n';
}

int main() {
    std::array data{'h', 'e', 'l', 'l', 'o'};
    print(data);
    print(data);
}
```
Вариант со span универсальнее и короче.

Давайте посмотрим на размер span:
```c++
int main() {
    std::vector<int> vector;
    auto sv = std::span{vector};
    std::cout << sizeof(sv) << '\n';

    std::array<int, 10> array;
    auto sa = std::span{array};
    std::cout << sizeof(sa) << '\n';

    int c_array[10];
    auto sca = std::span{c_array};
    std::cout << sizeof(sca) << '\n';
}
```

```
16
8
8
```
В случае статического span нам не нужно сохранять размер.

Можем теперь при интеграции кода на C++ и C использовать span для работы с классическими массивами в стиле С++, а также писать универсальные функции для работы с последовательностями.
