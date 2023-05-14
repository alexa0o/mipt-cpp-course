template <class Iterator>
class IndexedIterator {
public:
    // your code goes here

    IndexedIterator(Iterator it, size_t index) {

    }

    size_t index() const {

    }

};

template <class It>
IndexedIterator<It> CreateIndexedIterator(It iterator, size_t index = 0) {
    return IndexedIterator<It>(iterator, index);
}

