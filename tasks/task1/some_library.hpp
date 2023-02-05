#include <string>
#include <string_view>

namespace SomeLibrary {
    class ValueHolder {
    public:
        ValueHolder(const std::string_view name, int value)
          : name_(name), value_(value)
        { }
  
        std::string GetName() const {
            return name_;
        }
  
        int GetValue() const {
            return value_;
        }
    private:
        std::string name_;
        int value_;
    };
}

