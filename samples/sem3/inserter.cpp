template <class Container>
class my_insert_iterator {
public:
    my_insert_iterator(Container& c, typename Container::iterator i)
        : c_(c), i_(i) { }

    my_insert_iterator<Container>& operator++() {
        ++i_;
        return *this;
    }

    my_insert_iterator<Container>& operator*() {
        return *this;
    }

    my_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
        c_.insert(i_, value);
        return *this;
    }
private:
    Container& c_;
    typename Container::iterator i_;
};

template <class Container>
my_insert_iterator<Container> my_inserter(Container& c, typename Container::iterator i) {
    return my_insert_iterator<Container>(c, i);
}

int main() {
}
