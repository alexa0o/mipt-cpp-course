struct NotSetted { };

struct Nothing {
};

template <class T>
class Maybe {
    T value;
    bool isSetted;

public:
    Maybe() : isSetted(false) { }
    Maybe(Nothing&&) : isSetted(false) { }
    Maybe(const T& value) : value(value), isSetted(true) { }

    T& GetRef() {
        if (!isSetted) throw NotSetted();
        return value;
    }

    const T& GetRef() const {
        if (!isSetted) throw NotSetted();
        return value;
    }

    T* Get() {
        if (!isSetted) return nullptr;
        return &value;
    }

    const T* Get() const {
        if (!isSetted) return nullptr;
        return &value;
    }

    bool IsSetted() const {
        return isSetted;
    }
};

Maybe<int> calculate(int x) {
    if (x >= 0) {
        return x * x;
    }
    // cant calculate value
    return Nothing();
}

struct Scores {
    Maybe<float> model1;
    Maybe<float> model2;
};

int main() {
    Scores scores;
    scores.model1 = 0.315f;

    const Maybe<float> score(0.315f);
    score.GetRef();
    score.Get();
}
