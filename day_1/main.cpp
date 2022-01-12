#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>

using namespace std;

struct triple
{
    vector<int> values;
    int count {};
    
    triple(int one, int two, int three)
    {
        values.reserve(3);
        values.emplace_back(one);
        values.emplace_back(two);
        values.emplace_back(three);
    }
    
    int sum()
    {
        return accumulate(values.begin(), values.end(), 0);
    }
    
    void increment(int val)
    {
        values[count++] = val;
        count %= 3;
    }
    
};

int main()
{
    
    string filename = "../data.txt";
    ifstream file;
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    vector<int> values;
    int temp {};
    while (!file.eof())
    {
        file >> temp;
        values.emplace_back(temp);
    }
    
    int count {};
    temp = *values.begin();
    for (auto val = ++values.begin(); val != values.end(); val++)
    {
        if (*val > temp)
            ++count;
        temp = *val;
    }
    
    cout << count << endl;
    
    count = 0;
    triple first(values[0], values[1], values[2]), second(values[1], values[2], values[3]);
    for (int i = 3; i < values.size() - 1; ++i)
    {
        if (first.sum() < second.sum())
            ++count;
        first.increment(values.at(i));
        second.increment(values.at(i + 1));
    }
    if (first.sum() < second.sum())
        ++count;
    
    cout << count << endl;
    
    
    return EXIT_SUCCESS;
}
