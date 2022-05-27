#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <math.h>

using namespace std;

#define DEBUG 0

struct coord_t
{
    int16_t x, y;
    
    coord_t(int16_t x, int16_t y) : x(x), y(y)
    {}
    
    coord_t &operator+=(const coord_t& other)
    {
        x += other.x;
        y += other.y;
        
        return *this;
    }
    
    bool operator<(const coord_t &rhs) const
    {
        if (x < rhs.x)
            return true;
        if (rhs.x < x)
            return false;
        return y < rhs.y;
    }
    
    bool operator>(const coord_t &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const coord_t &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const coord_t &rhs) const
    {
        return !(*this < rhs);
    }
};

struct velocity_t : coord_t
{
    void step()
    {
        if (x > 0)
            --x;
        --y;
    }
};

int32_t sum_factorial(int32_t val)
{
    bool neg = val < 0;
    if (neg)
        val /= -1;
    
    int32_t res = (val * (val + 1) >> 1);
    
    return neg ? -res : res;
}

set<uint8_t> get_all_x(const coord_t &lower, const coord_t &upper)
{
    set<uint8_t> x_vals {};
    
    for (int16_t x = 0; x <= upper.x; ++x)
    {
        if (sum_factorial(x) < lower.x)
            continue; //Not possible
        int16_t temp {x};
        int32_t dis = 0;
        while(dis <= upper.x && temp > 0)
        {
            dis += temp--;
            if (dis >= lower.x && dis <= upper.x)
            {
                x_vals.insert(x);
                break;
            }
        }
    }
    
    return x_vals;
}

bool check_y(int32_t v_x, int32_t v_y, const coord_t &lower, const coord_t &upper)
{
    int32_t x {}, y {};
    
    while (true)
    {
        if (x >= lower.x && x <= upper.x && y >= lower.y && y <= upper.y)
            return true;
        else if (((v_x == 0 && x < lower.x) || x > upper.x) || y < lower.y)
            return false;
        x += v_x;
        y += v_y--;
        
        if (v_x > 0)
            --v_x;
    }
}

int32_t find_max_y_pos(const coord_t &lower, const coord_t &upper)
{
    set<uint8_t> valid_x_values = get_all_x(lower, upper);
    
    set<int32_t> v_vals {};
    
    for (auto &x : valid_x_values)
        for (int32_t v_y = 0; v_y <= abs(lower.y); ++v_y)
            if (check_y(x, v_y, lower, upper))
                v_vals.insert(v_y);
    
    return sum_factorial(*max_element(v_vals.begin(), v_vals.end()));
}

int32_t find_number_of_initial_velocities(const coord_t &lower, const coord_t &upper)
{
    set<uint8_t> valid_x_values = get_all_x(lower, upper);
    
    set<coord_t> v_vals {};
    
    for (auto &x : valid_x_values)
        for (int32_t v_y = lower.y - 1; v_y <= abs(lower.y); ++v_y)
            if (check_y(x, v_y, lower, upper))
                v_vals.insert(coord_t(x, (int16_t) v_y));

    return v_vals.size();
}

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
        string t;
        file >> t;
        temp += t;
    }
    file.close();
    
    string x_val = temp.substr(temp.find('x'), temp.find(',') - temp.find('x'));
    string y_val = temp.substr(temp.find('y'));
    const int16_t x_min = stoi(x_val.substr(x_val.find('=') + 1, x_val.find('.')));
    const int16_t x_max = stoi(x_val.substr(x_val.find_last_of('.') + 1));
    
    const int16_t y_min = stoi(y_val.substr(y_val.find('=') + 1, y_val.find('.')));
    const int16_t y_max = stoi(y_val.substr(y_val.find_last_of('.') + 1));
    
    coord_t lower {x_min, y_min};
    coord_t upper {x_max, y_max};
    
    
    cout << find_max_y_pos(lower, upper) << endl;
    cout << find_number_of_initial_velocities(lower, upper) << endl;
    
    return EXIT_SUCCESS;
}
