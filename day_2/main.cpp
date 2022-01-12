#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

int main()
{
    string filename = "../data.txt";
    ifstream file;
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    uint64_t temp_val {}, depth {}, horizontal {}, aim {};
    string temp_dir {};
    while (!file.eof())
    {
        file >> temp_dir;
        file >> temp_val;
        
        /* Task 1
        if (temp_dir == "forward")
            horizontal += temp_val;
        else if (temp_dir == "down")
            depth += temp_val;
        else
            depth -= temp_val;
        */
        if (temp_dir == "forward")
        {
            horizontal += temp_val;
            depth += aim * temp_val;
        }
        else if (temp_dir == "down")
            aim += temp_val;
        else
            aim -= temp_val;
    }
    
    cout << depth * horizontal << endl;
    
    return EXIT_SUCCESS;
}
