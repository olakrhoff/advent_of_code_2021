#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

int main()
{
    //Reading data from file
    string filename = "../data.txt";
    if (DEBUG)
        filename = "../test_data.txt";
    ifstream file;
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    string temp {};
    while (!file.eof())
    {
        file >> temp;
    }
    file.close();
    
    return EXIT_SUCCESS;
}
