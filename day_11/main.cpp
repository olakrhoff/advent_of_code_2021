#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

#define DEBUG 0

typedef struct octopus
{
    uint8_t row {}, col {}, energy {};
    bool flashed {false}, dummy {false};
    
    octopus(uint8_t x, uint8_t y, uint8_t energy) : row(x), col(y), energy(energy)
    {}
    
    void increment_energy()
    {
        energy = ++energy % 10;
    }
    
    void increment_if_not_zero()
    {
        if (energy != 0)
            increment_energy();
    }
    
    bool operator==(const octopus &rhs) const
    {
        return row == rhs.row &&
               col == rhs.col;
    }
    
    bool operator!=(const octopus &rhs) const
    {
        return !(rhs == *this);
    }
    
} octopus_t;

typedef struct octopuses
{
    uint8_t length {10};
    vector<octopus_t> octopuses_vec;
    
    explicit octopuses(const vector<uint8_t> &octopusesVec)
    {
        for (int row = 0; row < 10; ++row)
            for (int col = 0; col < 10; ++col)
                octopuses_vec.emplace_back(octopus_t(row, col, octopusesVec.at(row * length + col)));
    }
    
    bool is_valid_at(int row, int col)
    {
        if (row < 0 || row > 9 || col < 0 || col > 9)
            return false;
        return true;
    }
    
    bool are_all_zero()
    {
        for (auto &oc : this->octopuses_vec)
            if (oc.energy != 0)
                return false;
        return true;
    }
    
    void increment_all()
    {
        for (octopus_t &oc : octopuses_vec)
            oc.increment_energy();
    }
    
    void flash(octopus_t &oct)
    {
        if (oct.flashed)
            return;
        
        if (oct.energy == 0)
            oct.flashed = true;
        
        for (int row = -1; row < 2; ++row)
            for (int col = -1; col < 2; ++col)
                if (is_valid_at(oct.row + row, oct.col + col))
                {
                    octopus_t *temp = &octopuses_vec.at((oct.row + row) * 10 + oct.col + col);
                    temp->increment_if_not_zero();
                    if (temp->energy == 0)
                        flash(*temp);
                }
    }
    
    uint64_t step()
    {
        increment_all();
        for (octopus_t &oc : octopuses_vec)
            if (oc.energy == 0)
                flash(oc);
        
        uint64_t zeros {};
        for (octopus_t &oc : octopuses_vec)
        {
            oc.flashed = false;
            if (oc.energy == 0)
                ++zeros;
        }
            
        return zeros;
    }
    
    void print()
    {
        for (int row = 0; row < 10; ++row)
        {
            for (int col = 0; col < 10; ++col)
            {
                cout << (char)(octopuses_vec.at(row * 10 + col).energy + 48);
            }
            cout << endl;
        }
        cout << endl;
    }
    
} octopuses_t;

int main()
{
    string filename = "../data.txt";
    if (DEBUG)
        filename = "../test_data.txt";
    ifstream file;
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    vector<uint8_t> values;
    char buffer;
    while (!file.eof())
    {
        file >> buffer;
        values.emplace_back(buffer - 48);
    }
    file.close();
    
    octopuses_t octopuses_list(values);
    
    uint64_t flashes {};
    for (int i = 1; i < 1000; ++i)
    {
        flashes += octopuses_list.step();
        if (octopuses_list.are_all_zero())
            cout << "Iteration: " << i << endl;
    }
    
    cout << flashes << endl;
    
    return EXIT_SUCCESS;
}