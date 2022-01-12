#include <iostream>
#include <vector>
#include <fstream>
#include <functional>

using namespace std;

#define DEBUG 0

typedef struct coordinate
{
    uint16_t x {}, y {}, value {1};
    
    coordinate() = default;
    
    coordinate(uint16_t x, uint16_t y) : x(x), y(y)
    {}
    
    bool operator<(const coordinate &rhs) const
    {
        if (x < rhs.x)
            return true;
        if (rhs.x < x)
            return false;
        return y < rhs.y;
    }
    
    bool operator>(const coordinate &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const coordinate &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const coordinate &rhs) const
    {
        return !(*this < rhs);
    }
    
    bool operator==(const coordinate &rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
    
    bool operator!=(const coordinate &rhs) const
    {
        return !(rhs == *this);
    }
    
    friend ostream &operator<<(ostream &os, const coordinate &coordinate)
    {
        os << "x: " << coordinate.x << " y: " << coordinate.y << " value: " << coordinate.value;
        return os;
    }
} coordinate_t;

void swap_coordinates(coordinate_t &from, coordinate_t &to)
{
    coordinate_t temp = from;
    from = to;
    to = temp;
}

void find_and_increment_coordinate(vector<coordinate_t> &grid, uint16_t x, uint16_t y)
{
    coordinate_t coor(x, y);
    uint32_t lower {0}, upper {(uint32_t) grid.size()};
    
    if (grid.empty())
    {
        grid.emplace_back(coor);
        return;
    }
    
    while (true)
    {
        uint32_t index = (upper + lower) >> 1; //lower + (upper - lower) / 2 = (upper + lower) / 2
        coordinate_t mid = grid.at(index);
        if (mid == coor)
        {
            ++grid.at(index).value;
            return;
        }
        else if (mid < coor)
            if (lower != index)
                lower = index;
            else
                ++lower;
        else if (upper != index)
            upper = index;
        else
            --upper;
        
        
        if (upper <= lower && mid != coor)
        {
            grid.insert(grid.begin() + lower, coor);
            return;
        }
    }
}

void add_line_to_grid(vector<coordinate_t> &grid, coordinate_t from, coordinate_t to)
{
    if (from.x == to.x) //Vertical line
    {
        if (from.y > to.y)
            swap_coordinates(from, to);
        for (int col = from.y; col <= to.y; ++col)
            find_and_increment_coordinate(grid, from.x, col);
    }
    else if (from.y == to.y) //Horizontal line
    {
        if (from.x > to.x)
            swap_coordinates(from, to);
        for (int row = from.x; row <= to.x; ++row)
            find_and_increment_coordinate(grid, row, from.y);
    }
    else //Diagonal lines (only at 45 degrees
    {
        if (from.x > to.x)
            swap_coordinates(from, to);
        int sign {1};
        if (from.y > to.y)
            sign = -1;
        for (int i = 0; i <= to.x - from.x; ++i)
        {
            find_and_increment_coordinate(grid, from.x + i, from.y + (i * sign));
        }
        if (DEBUG)
            cout << "Diagonal" << endl;
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
    
    vector<coordinate_t> grid;
    coordinate_t from, to;
    while (!file.eof())
    {
        file >> from.x;
        file.ignore();
        file >> from.y;
        
        file.ignore(4);
        
        file >> to.x;
        file.ignore();
        file >> to.y;
        add_line_to_grid(grid, from, to);
    }
    file.close();
    
    int temp_val {};
    for (coordinate_t coor : grid)
        if (coor.value > 1)
            ++temp_val;
    
    cout << temp_val << endl;
    
    if (DEBUG)
        for (int y = 0; y < 10; ++y)
        {
            for (int x = 0; x < 10; ++x)
            {
                bool found {false};
                coordinate_t coor(x, y);
                for (coordinate_t coord : grid)
                    if (coord == coor)
                    {
                        cout << coord.value << " ";
                        found = true;
                        break;
                    }
                if (!found)
                    cout << ". ";
            }
            cout << endl;
        }
    
    return EXIT_SUCCESS;
}
