# Семинар 8. Аллокаторы advanced
## Scoped allocator
При создании вложенных контейнеров встает вопрос о распределении памяти при их аллокации. Допустим, создаем массив строк `std::vector<std::string>`, тогда для вектора может быть использована одна арена памяти, а каждой из строк - какая-то своя.
Мы могли бы решить эту проблему явно передавая аллокатор в контейнер, то есть написав что-то подобное:
```c++
template<typename T>
using Allocator = SomeFancyAllocator<T>;

using String = std::basic_string<char, std::char_traits<char>, Allocator<char>>;
using Vector = std::vector<String, Allocator<String>>;

Allocator<String> as( some_memory_resource );
Allocator<char> ac(as);
Vector v(as);
v.push_back( String("hello", ac) );
v.push_back( String("world", ac) );
```

Это чинит проблему выше, но теперь нужно быть очень аккуратными с аллокаторами и всегда передавать их при создании новой строки. Тут очень легко допустить ошибку:
```c++
v.push_back( String("oops, not using same memory resource") );
```
В примере выше мы снова используем другой аллокатор.

`std::scoped_allocator_adaptor` - это адаптер над аллокатором, решающий проблему применения одного аллокатора при получении ресурсов для объектов в контейнере. Пример использования:
```c++
template<typename T>
  using Allocator = SomeFancyAllocator<T>;
using String = std::basic_string<char, std::char_traits<char>, Allocator<char>>;
using Vector = std::vector<String, std::scoped_allocator_adaptor<Allocator<String>>>;
                                   /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
Allocator<String> as( some_memory_resource );
Allocator<char> ac(as);
Vector v(as);
v.push_back( String("hello") );  // no allocator argument needed!
v.push_back( String("world") );  // no allocator argument needed!
```

Теперь можно писать даже так:
```c++
v.push_back( "hello" );
v.push_back( "world" );
```

Когда мы добавляем в вектор очередной элемент, то для конструирования делается следующий вызов:
```c++
std::allocator_traits<allocator_type>::construct( get_allocator(), void_ptr, obj );
```
И потом внутри аллокатора:
```c++
::new (void_ptr) value_type(obj);
```

Если же allocator_type в контейнере представляет scoped_allocator_adapter, то необходимо выяснить, принимает ли объект в качестве аргумента аллокатор и в зависимости от этого выполнить
если не принимает, то:
```c++
std::allocator_traits<outer_allocator_type>::construct(outer_allocator(), void_ptr, obj);
```

если принимает, то:
```c++
std::allocator_traits<outer_allocator_type>::construct(outer_allocator(), void_ptr, obj, inner_allocator());
```
либо для типов вида std::pair, std::function, std::tuple и тп:
```c++
std::allocator_traits<outer_allocator_type>::construct(outer_allocator(), void_ptr, std::allocator_arg, inner_allocator(), obj);
```

Давайте тут обозначим, что означает inner_allocator и outer_allocator.  
inner_allocator_type - это аллокатор, используемый контейнером внутри основного контейнера (он может быть в том числе и scoped_allocator_adaptor),  
outer_allocator_type - это аллокатор, ииспользуемый основным контейнером.  
Если вложенных контейнеров больше, чем вложенных inner_allocator_type'ов, то используемых последний из аллокаторов в списке.

## polymorphic_allocator
Рассмотрим следующий пример:
```c++
auto my_vector = std::vector<int, my_allocator>();
auto my_vector2 = std::vector<int, other_allocator>();
auto vec = my_vector;
vec = my_vector2;
```

В примере выше получим ошибку компиляции, потому что у векторов разные типы. В С++17 появились полиморфные аллокаторы с динамической диспетчеризацией, которые позволяют избежать этой проблемы.  
Внутри аллокаторы хранится указатель на некоторый интерфейс memory_resource, для изменения стратегии работы с памятью достаточно подменить экземпляр memory_resource, сохраняя при этом тип аллокатора. Сделать это можно в том числе и в рантайме. В остальном полиморфные аллокаторы работают по тем же правилам, что и стандартные.
Специфические типы данных, используемые новым аллокатором, находятся в нэймспэйсе `std::pmr`. Там же находятся темплейтные специализации стандартных контейнеров, которые умеют работать с полиморфным аллокатором.  
Есть проблема в несовместимости с обычными контейнерами из stl.
Основной компонент - абстрактный класс, реализация которого отвечает за работу с памятью `std::pmr::memory_resource`:
```c++
virtual void* do_allocate(std::size_t bytes, std::size_t alignment),
virtual void do_deallocate(void* p, std::size_t bytes, std::size_t alignment)
virtual bool do_is_equal(const std::pmr::memory_resource& other) const noexcept
```

Уже есть стандартные реализации пулов памяти:
```c++
synchronized_pool_resource
unsynchronized_pool_resource
monotonic_buffer_resource
```

Посмотрим на пример использования `monotonic_buffer_resource`:
```c++
#include <iostream>
#include <memory_resource>   // pmr core types
#include <vector>        	// pmr::vector
#include <string>        	// pmr::string
 
int main() {
	char buffer[64] = {}; // a small buffer on the stack
	std::fill_n(std::begin(buffer), std::size(buffer) - 1, '_');
	std::cout << buffer << '\n';
 
	std::pmr::monotonic_buffer_resource pool{std::data(buffer), std::size(buffer)};
 
	std::pmr::vector<char> vec{ &pool };
	for (char ch = 'a'; ch <= 'z'; ++ch)
    	vec.push_back(ch);
 
	std::cout << buffer << '\n';
}
```

При использовании контейнеров из pmr::, для вложенных контейнеров будет использован родительский аллокатор для управления памятью. Пример:
```c++
#include <iostream>
#include <memory_resource>   // pmr core types
#include <vector>        	// pmr::vector
#include <string>        	// pmr::string
 
int main() {
	std::cout << "sizeof(std::string): " << sizeof(std::string) << '\n';
	std::cout << "sizeof(std::pmr::string): " << sizeof(std::pmr::string) << '\n';
 
	char buffer[256] = {}; // a small buffer on the stack
	std::fill_n(std::begin(buffer), std::size(buffer) - 1, '_');
 
	const auto BufferPrinter = [](std::string_view buf, std::string_view title) {
    	std::cout << title << ":\n";
    	for (auto& ch : buf) {
        	std::cout << (ch >= ' ' ? ch : '#');
    	}
    	std::cout << '\n';
	};
 
	BufferPrinter(buffer, "zeroed buffer");
 
	std::pmr::monotonic_buffer_resource pool{std::data(buffer), std::size(buffer)};
	std::pmr::vector<std::pmr::string> vec{ &pool };
	vec.reserve(5);
 
	vec.push_back("Hello World");
	vec.push_back("One Two Three");
	BufferPrinter(std::string_view(buffer, std::size(buffer)), "after two short strings");
 
	vec.emplace_back("This is a longer string");
	BufferPrinter(std::string_view(buffer, std::size(buffer)), "after longer string strings");
 
	vec.push_back("Four Five Six");
	BufferPrinter(std::string_view(buffer, std::size(buffer)), "after the last string");   
}
```
Обратите внимание на следующие пункты:  
1. Размер pmr::string больше чем std::string. Связано этот с тем, что добавляется указатель на memory_resource;  
2. Мы резервируем вектор под 5 элементов, поэтому при добавлении 4х реаллокаций не происходит.  
3. Третья строка более длинная и для потребовался отдельный кусок памяти внутри нашего буфера, в векторе при этом сохраняется только указатель на этот блок.  
4. Как можно видеть из вывода, строка «This is a longer string» расположена почти в самом конце буфера.
5. Когда мы вставляем еще одну короткую строку, она попадает снова в блока памяти вектора.  

А что если тоже самое проделать для std::string?

По умолчанию, если память в пуле закончилась, то аллокатор запрашивает ее у new(). Это не совсем правда, на самом деле просиходит следующее: получаем дефолтный memory_resource с помощью `std::pmr::memory_resource* get_default_resource()`, что по умолчанию возвращает `std::pmr::new_delete_resource()`, который уже в свою очередь вызывает new(). Естественно, это поведение можно изменить с помощью `std::pmr::memory_resource* set_default_resource(std::pmr::memory_resource* r)`.

Методы `do_allocate()` и `do_deallocate()` используют аргумент «выравнивания», поэтому нам понадобится С++17 версия new() c поддержкой выравнивания:
```c++
void* lastAllocatedPtr = nullptr;
size_t lastSize = 0;
 
void* operator new(std::size_t size, std::align_val_t align) {
#if defined(_WIN32) || defined(__CYGWIN__)
	auto ptr = _aligned_malloc(size, static_cast<std::size_t>(align));
#else
	auto ptr = aligned_alloc(static_cast<std::size_t>(align), size);
#endif
 
	if (!ptr)
    	throw std::bad_alloc{};
 
	std::cout << "new: " << size << ", align: "
          	<< static_cast<std::size_t>(align)
  	        << ", ptr: " << ptr << '\n';
 
	lastAllocatedPtr = ptr;
	lastSize = size;
 
	return ptr;
}
```

Теперь вернемся к рассмотрению основного примера:
```c++
constexpr auto buf_size = 32;
uint16_t buffer[buf_size] = {}; // a small buffer on the stack
std::fill_n(std::begin(buffer), std::size(buffer) - 1, 0);
 
std::pmr::monotonic_buffer_resource pool{std::data(buffer), std::size(buffer)*sizeof(uint16_t)};
 
std::pmr::vector<uint16_t> vec{ &pool };
 
for (int i = 1; i <= 20; ++i)
	vec.push_back(i);
 
for (int i = 0; i < buf_size; ++i)
	std::cout <<  buffer[i] << " ";
 
std::cout << std::endl;
 
auto* bufTemp = (uint16_t *)lastAllocatedPtr;
 
for (unsigned i = 0; i < lastSize; ++i)
	std::cout << bufTemp[i] << " ";
```

Получим следующий вывод:
```c++
new: 128, align: 16, ptr: 0xc73b20
1 1 2 1 2 3 4 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 0
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 65535 132 0 0 0 0 0 0 0 144 0 0 0 65 0 0 0 16080 199 0 0 16176 199 0 0 16176 199 0 0 15344 199 0 0 15472 199 0 0 15472 199 0 0 0 0 0 0 145 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
```

Судя по выводу в консоль выделенного буфера хватает только для 16 элементов, и когда мы вставляем число 17, происходит новая аллокация 128 байт с помощью оператора new().

Для создания своей реализации интерфейса `memory_resource` необходимо:
1. унаследоваться от `std::pmr::memory_resource`  
2. Реализовать методы:
```c++
do_allocate()
do_deallocate()
do_is_equal()
```

3. Передать вашу реализацию `memory_resource` контейнерам.  

