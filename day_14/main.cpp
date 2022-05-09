#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace std;

#define DEBUG 0

struct coord_t
{
    uint32_t x {}, y {};
    
    coord_t(uint32_t x, uint32_t y) : x(x), y(y)
    {}
    
    bool operator==(const coord_t &rhs) const
    {
        return x == rhs.x &&
               y == rhs.y;
    }
    
    bool operator!=(const coord_t &rhs) const
    {
        return !(rhs == *this);
    }
    
    bool operator<(const coord_t &rhs) const
    {
        if (x != rhs.x)
            return x < rhs.x;
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
    
    friend ostream &operator<<(ostream &os, const coord_t &coord)
    {
        os << "(" << coord.x << "," << coord.y << ")";
        return os;
    }
};

struct transparant_paper_t
{
    static const bool X_AXIS = true;
    static const bool Y_AXIS = false;
    
    set<coord_t> dots {};
    
    transparant_paper_t() = default;
    
    void add_dot(const coord_t &&dot)
    {
        dots.insert(dot);
    }
    
    //Returns left and right or top and bottom papers.
    tuple<transparant_paper_t> split(bool axis, uint32_t at)
    {
        return tuple<transparant_paper_t>();
    }
    
    void fold(bool axis, uint32_t at)
    {
        set<coord_t> new_dots {};
        if (axis == X_AXIS)
        {
            for (auto dot : dots)
            {
                if (dot.x < at)
                    new_dots.insert(dot);
                else if (dot.x > at)
                    //new_dots.insert(coord_t(at - (dot.x - at), dot.y));
                    new_dots.insert(coord_t((at << 1) - dot.x, dot.y));
            }
        }
        else
        {
            for (auto dot : dots)
            {
                if (dot.y < at)
                    new_dots.insert(dot);
                else if (dot.y > at)
                    //new_dots.insert(coord_t(dot.x, at - (dot.y - at)));
                    new_dots.insert(coord_t(dot.x, (at << 1) - dot.y));
            }
        }
        
        dots = new_dots;
    }
    
    uint32_t max_x() const
    {
        return std::max_element(dots.begin(), dots.end())->x;
    }
    
    
    uint32_t max_y() const
    {
        return std::max_element(dots.begin(), dots.end(), [] (const coord_t lhs, const coord_t rhs)
        {
            return lhs.y < rhs.y;
        })->y;
    }
    
    friend ostream &operator<<(ostream &os, const transparant_paper_t &paper)
    {
        cout << "y: " << paper.max_y() << endl;
        cout << "x: " << paper.max_x() << endl;
        for (uint32_t y = 0; y <= paper.max_y(); ++y)
        {
            for (uint32_t x = 0; x <= paper.max_x(); ++x)
            {
                if (paper.dots.contains(coord_t(x, y)))
                    os << "#";
                else
                    os << ".";
            }
            os << "\n";
        }
        
        return os;
    }
};


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
    
    transparant_paper_t paper;
    
    string line {};
    while (!file.eof())
    {
        file >> line;
        if (line == "fold")
            break; //Folding instructions are coming
        uint8_t comma_index = line.find(',');
        string first = line.substr(0, comma_index);
        string second = line.substr(++comma_index, line.length());
        paper.add_dot(coord_t(stoi(first), stoi(second)));
        line.clear(); //empty the line buffer
    }
    
    
    while (!file.eof())
    {
        file >> line;
        if (line.find('=') == -1)
            continue;
        string number = line.substr(2, line.length());
        if (line[0] == 'x')
        {
            paper.fold(transparant_paper_t::X_AXIS, stoi(number));
            //break;
        }
        else //y
        {
            paper.fold(transparant_paper_t::Y_AXIS, stoi(number));
            //break;
        }
        line.clear(); //empty the line buffer
    }
    
    file.close();
    
    cout << paper.dots.size() << "\n";
    
    cout << paper << "\n";
    
    
    return EXIT_SUCCESS;
}
