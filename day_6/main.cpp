#include <iostream>
#include <vector>
#include <fstream>
#include <functional>

using namespace std;

#define DEBUG 0

void increment_fish(vector<uint64_t> &fish)
{
    uint64_t temp = fish.at(0);
    for (int i = 0; i < fish.size() - 1; ++i)
        fish.at(i) = fish.at(i + 1);
    fish.at(6) += temp;
    fish.at(8) = temp;
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
    
    vector<uint64_t> fish(9);
    while (!file.eof())
    {
        uint64_t temp {};
        file >> temp;
        file.ignore();
        
        ++fish.at(temp);
    }
    file.close();
    
    for (int day = 0; day < /*80 taks 1*/ 256; ++day)
        increment_fish(fish);
    
    uint64_t sum {};
    for (auto val : fish)
        sum += val;
    
    cout << sum << endl;
    
    return EXIT_SUCCESS;
}
