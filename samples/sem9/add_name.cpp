class Widget {
public:
    void addName(std::string& name) {
        names.push_back(name);
    }

    void doSmth(int x) {

    }

private:
    std::vector<std::string&> names;
};
