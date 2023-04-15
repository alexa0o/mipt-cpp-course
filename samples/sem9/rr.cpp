T = Type 
T = Type&
T = Type&&

static_cast<T&&>(...) = static_cast<Type(/&/&&)&&>(...);
                        static_cast<Type&>(...);
