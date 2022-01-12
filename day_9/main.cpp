#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

#define DEBUG 0

#if DEBUG
#define LINE_LEN 10
#else
#define LINE_LEN 100
#endif

void add_adjacent(const vector<uint8_t> &vec, const pair<uint8_t, uint8_t> &pos, vector<pair<uint8_t, uint8_t>> &found,
                  uint64_t &basin_size)
{
    for (auto e : found)
        if (e.first == pos.first && e.second == pos.second)
            return;
    found.emplace_back(pos);
    ++basin_size;
    
    
    //up
    if (pos.first > 0 && vec.at((pos.first - 1) * LINE_LEN + pos.second) != 9)
        add_adjacent(vec, pair<uint8_t, uint8_t>(pos.first - 1, pos.second), found, basin_size);
    //right
    if (pos.second < LINE_LEN - 1 &&
        vec.at(pos.first * LINE_LEN + pos.second + 1) != 9)
        add_adjacent(vec, pair<uint8_t, uint8_t>(pos.first, pos.second + 1), found, basin_size);
    //down
    if (pos.first < vec.size() / LINE_LEN - 1 &&
        vec.at((pos.first + 1) * LINE_LEN + pos.second) != 9)
        add_adjacent(vec, pair<uint8_t, uint8_t>(pos.first + 1, pos.second), found, basin_size);
    //left
    if (pos.second > 0 && vec.at(pos.first * LINE_LEN + pos.second - 1) != 9)
        add_adjacent(vec, pair<uint8_t, uint8_t>(pos.first, pos.second - 1), found, basin_size);
}

uint64_t find_basin_size(const vector<uint8_t> &vec, pair<uint8_t, uint8_t> &pos)
{
    vector<pair<uint8_t, uint8_t>> found;
    uint64_t basin_size {};
    add_adjacent(vec, pos, found, basin_size);
    
    return basin_size;
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
    
    char buffer[LINE_LEN] {};
    vector<uint8_t> heights;
    while (!file.eof())
    {
        file.read(buffer, LINE_LEN);
        file.ignore();
        for (char c : buffer)
            heights.emplace_back(c - 48);
    }
    file.close();
    
    vector<pair<uint8_t, uint8_t>> low_points;
    uint64_t count {};
    
    //The middle
    for (int row = 1; row < heights.size() / LINE_LEN - 1; ++row)
        for (int col = 1; col < LINE_LEN - 1; ++col)
        {
            auto check = heights.at(row * LINE_LEN + col);
            if (check < heights.at((row - 1) * LINE_LEN + col)/*up*/ &&
                check < heights.at(row * LINE_LEN + col + 1)/*right*/ &&
                check < heights.at((row + 1) * LINE_LEN + col)/*down*/ &&
                check < heights.at(row * LINE_LEN + col - 1)/*left*/)
            {
                count += check + 1;
                low_points.emplace_back(pair<uint16_t, uint16_t>(row, col));
            }
        }
    //The top and bottom
    for (int col = 1; col < LINE_LEN - 1; ++col)
    {
        auto check = heights.at(col);
        if (check < heights.at(col + 1)/*right*/ &&
            check < heights.at(LINE_LEN + col)/*down*/ &&
            check < heights.at(col - 1)/*left*/)
        {
            count += check + 1;
            low_points.emplace_back(pair<uint16_t, uint16_t>(0, col));
        }
        
        check = heights.at((heights.size() / LINE_LEN - 1) * LINE_LEN + col);
        if (check < heights.at((heights.size() / LINE_LEN - 1) * LINE_LEN + col + 1)/*right*/ &&
            check < heights.at((heights.size() / LINE_LEN - 2) * LINE_LEN + col)/*up*/ &&
            check < heights.at((heights.size() / LINE_LEN - 1) * LINE_LEN + col - 1)/*left*/)
        {
            count += check + 1;
            low_points.emplace_back(pair<uint16_t, uint16_t>(heights.size() / LINE_LEN - 1, col));
        }
    }
    
    
    //The left and right
    for (int row = 1; row < heights.size() / LINE_LEN - 1; ++row)
    {
        auto check = heights.at(row * LINE_LEN);
        if (check < heights.at(row * LINE_LEN + 1)/*right*/ &&
            check < heights.at((row + 1) * LINE_LEN)/*down*/ &&
            check < heights.at((row - 1) * LINE_LEN)/*up*/)
        {
            count += check + 1;
            low_points.emplace_back(pair<uint16_t, uint16_t>(row, 0));
        }
        
        check = heights.at((row + 1) * LINE_LEN - 1);
        if (check < heights.at((row) * LINE_LEN - 1)/*up*/ &&
            check < heights.at((row + 1) * LINE_LEN - 2)/*left*/ &&
            check < heights.at((row + 2) * LINE_LEN - 1)/*down*/)
        {
            count += check + 1;
            low_points.emplace_back(pair<uint16_t, uint16_t>(row, LINE_LEN - 1));
        }
    }
    
    //The corners
    //Upper-left
    if (heights.at(0) < heights.at(1) && heights.at(0) < heights.at(LINE_LEN))
    {
        count += heights.at(0) + 1;
        low_points.emplace_back(pair<uint16_t, uint16_t>(0, 0));
    }
    //Upper-right
    if (heights.at(LINE_LEN - 1) < heights.at(LINE_LEN - 2) &&
        heights.at(LINE_LEN - 1) < heights.at((LINE_LEN << 1) - 1))
    {
        count += heights.at(LINE_LEN - 1) + 1;
        low_points.emplace_back(pair<uint16_t, uint16_t>(0, LINE_LEN - 1));
    }
    //Lower-left
    if (heights.at((heights.size() / LINE_LEN - 1) * LINE_LEN) <
        heights.at((heights.size() / LINE_LEN - 2) * LINE_LEN) &&
        heights.at((heights.size() / LINE_LEN - 1) * LINE_LEN) <
        heights.at((heights.size() / LINE_LEN - 1) * LINE_LEN + 1))
    {
        count += heights.at((heights.size() / LINE_LEN - 1) * LINE_LEN) + 1;
        low_points.emplace_back(pair<uint16_t, uint16_t>(heights.size() / LINE_LEN - 1, 0));
    }
    //Lower-right
    if (heights.at((heights.size() / LINE_LEN) * LINE_LEN - 1) <
        heights.at((heights.size() / LINE_LEN - 1) * LINE_LEN - 1) &&
        heights.at((heights.size() / LINE_LEN) * LINE_LEN - 1) < heights.at((heights.size() / LINE_LEN) * LINE_LEN - 2))
    {
        count += heights.at((heights.size() / LINE_LEN) * LINE_LEN - 1) + 1;
        low_points.emplace_back(pair<uint16_t, uint16_t>(heights.size() / LINE_LEN - 1, LINE_LEN - 1));
    }
    
    //cout << count << endl; //Task one
    
    typedef struct triplet
    {
        vector<uint16_t> val {};
        
        void add_if_bigger(uint16_t new_val)
        {
            if (val.size() < 3)
                val.emplace_back(new_val);
            else
            {
                auto min = std::min_element(val.begin(), val.end());
                if (*min < new_val)
                    *min = new_val;
            }
        }
        
        uint64_t sum()
        {
            uint64_t temp {1};
            for (auto i : val)
                temp *= i;
            return temp;
        }
    } triplet_t;
    
    triplet_t tri;
    for (auto pos : low_points)
        tri.add_if_bigger(find_basin_size(heights, pos));
    
    cout << tri.sum() << endl;
    
    return EXIT_SUCCESS;
}