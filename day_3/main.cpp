#include <iostream>
#include <vector>
#include <fstream>
#include <functional>

using namespace std;

#define DEBUG 0

#if DEBUG
#define BINARY_LEN 5
#else
#define BINARY_LEN 12
#endif

class Filter
{
    public:
        std::function<bool(uint16_t)> func;
        
        Filter(const std::function<bool(uint16_t)> &func) : func(func)
        {}
};

std::vector<uint16_t> operator|(const std::vector<uint16_t> &input, const Filter &filter)
{
    std::vector<uint16_t> result;
    for (auto &e : input)
    {
        if (filter.func(e))
            result.emplace_back(e);
    }
    return result;
}

bool most_common_bit(vector<uint16_t> &data, int bit_pos)
{
    int count {};
    uint16_t filter = (1 << (bit_pos));
    for (uint16_t val : data)
        if ((val & filter) == 0)
            --count;
        else
            ++count;
    return count >= 0;
}


int main()
{
    string filename = "../data.txt";
    if (DEBUG)
        filename = "../test_data.txt";
    ifstream file;
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    vector<int> values(BINARY_LEN);
    char temp[13] {};
    while (!file.eof())
    {
        for (int &value : values)
        {
            file.read(temp, 1);
            if (*temp - 48 == 0) //Calibrate for ASCII offset
                --value;
            else
                ++value;
        }
        file.read(temp, 1);
    }
    
    uint64_t gamma {}, epsilon {};
    for (int i = 0; i < values.size(); ++i)
        if (values.at(i) > 0)
            gamma += (1 << (values.size() - 1 - i));
        else
            epsilon += (1 << (values.size() - 1 - i));
    
    cout << gamma * epsilon << endl;
    
    file.close();
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    vector<uint16_t> binaries;
    uint16_t temp_val {};
    while (!file.eof())
    {
        file.read(temp, BINARY_LEN);
        for (int i = 0; i < BINARY_LEN; ++i)
            if (temp[i] - 48 == 1)
                temp_val += (1 << (BINARY_LEN - 1 - i));
        binaries.emplace_back(temp_val);
        temp_val = 0;
        file.read(temp, 1);
    }
    
    auto binaries_copy = binaries;
    
    uint64_t oxy {}, co2 {};
    for (int i = 0; i < BINARY_LEN; ++i)
    {
        int bit_pos = BINARY_LEN - 1 - i;
        if (binaries.size() == 1)
            break;
        bool bit = most_common_bit(binaries, bit_pos);
        uint16_t filter = (1 << bit_pos);
        binaries = binaries | Filter([filter, bit](uint16_t e)
                                     {
                                         return (e & filter) == 0 && !bit || (e & filter) != 0 && bit;
                                     });
    }
    oxy = binaries.at(0);
    
    for (int i = 0; i < BINARY_LEN; ++i)
    {
        int bit_pos = BINARY_LEN - 1 - i;
        if (binaries_copy.size() == 1)
            break;
        bool bit = most_common_bit(binaries_copy, bit_pos);
        uint16_t filter = (1 << bit_pos);
        binaries_copy = binaries_copy | Filter([filter, bit](uint16_t e)
                                     {
                                         return !((e & filter) == 0 && !bit || (e & filter) != 0 && bit);
                                     });
    }
    co2 = binaries_copy.at(0);
    
    cout << oxy * co2 << endl;
    
    return EXIT_SUCCESS;
}
