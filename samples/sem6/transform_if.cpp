std::vector<int> transform_if(std::vector<int> v) {
    std::vector<int> new_v;
    std::copy_if(v.begin(),v.end(), std::back_inserter(new_v), predicat);
    std::vector<int> result;
    std::transform(new_v.begin(), new_v.end(), std::back_inserter(result), func);
    return result;
}
