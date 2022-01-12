#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>

using namespace std;

#define DEBUG 0

/* Task 1
uint64_t sum_fuel_consumption(vector<uint16_t> &positions, uint16_t horizontal_pos)
{
    uint64_t fuel_consumed {};
    for (auto pos : positions)
        fuel_consumed += abs(horizontal_pos - pos);
    
    return fuel_consumed;
}*/

uint64_t sum_factorial(uint16_t val)
{
    return ((val * val) + val) >> 1;
}

uint64_t sum_fuel_consumption(vector<uint16_t> &positions, uint16_t horizontal_pos)
{
    uint64_t fuel_consumed {};
    for (auto pos : positions)
        fuel_consumed += sum_factorial(abs(horizontal_pos - pos));
    
    return fuel_consumed;
}

vector<uint16_t> sub_vector(vector<uint16_t> vec, int start, int end)
{
    vector<uint16_t> return_vector;
    for (int i = start; i < end; ++i)
        return_vector.emplace_back(vec.at(i));
    
    return return_vector;
}

/**
 *
 * @param positions, must be sorted from least to greatest
 * @return, the horizontal position
 */
uint64_t find_locale_minimum(vector<uint16_t> &positions)
{
    uint32_t lower_index {positions.at(0)}, upper_index {positions.at(positions.size() - 1)};
    uint64_t lower {sum_fuel_consumption(positions, lower_index)}, upper {
            sum_fuel_consumption(positions, upper_index)};
    
    while (true)
    {
        uint32_t index = (upper_index + lower_index) >> 1; //lower + (upper - lower) / 2 = (upper + lower) / 2
        uint64_t mid = sum_fuel_consumption(positions, index);
        if (mid < lower)
            lower = sum_fuel_consumption(positions, ++lower_index);
        else if (upper_index != index)
            upper = sum_fuel_consumption(positions, --upper_index);
        
        if (upper_index <= lower_index)
            return sum_fuel_consumption(positions, index);
    }
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
    
    vector<uint16_t> positions;
    while (!file.eof())
    {
        uint16_t temp {};
        file >> temp;
        file.ignore();
        positions.emplace_back(temp);
    }
    file.close();
    std::sort(positions.begin(), positions.end());
    
    cout << find_locale_minimum(positions) << endl;
    
    return EXIT_SUCCESS;
}
