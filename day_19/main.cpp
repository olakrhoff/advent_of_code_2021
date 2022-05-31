#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <set>

using namespace std;

#define DEBUG 0

struct coord_t
{
    int32_t x {}, y {}, z {};
    
    coord_t() = default;
    
    coord_t(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z)
    {}
    
    explicit coord_t(const string &values)
    {
        string x_string = values.substr(0, values.find_first_of(','));
        string y_string = values.substr(values.find_first_of(',') + 1,
                                        values.find_last_of(',') - x_string.length() - 1);
        string z_string = values.substr(values.find_last_of(',') + 1);
        
        x = (int32_t) stoi(x_string);
        y = (int32_t) stoi(y_string);
        z = (int32_t) stoi(z_string);
    }
    
    bool operator<(const coord_t &rhs) const
    {
        if (x < rhs.x)
            return true;
        if (rhs.x < x)
            return false;
        if (y < rhs.y)
            return true;
        if (rhs.y < y)
            return false;
        return z < rhs.z;
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
    
    bool operator==(const coord_t &rhs) const
    {
        return x == rhs.x &&
               y == rhs.y &&
               z == rhs.z;
    }
    
    bool operator!=(const coord_t &rhs) const
    {
        return !(rhs == *this);
    }
    
    coord_t &operator+=(const coord_t &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        
        return *this;
    };
    
    coord_t &operator-=(const coord_t &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        
        return *this;
    };
    
    coord_t operator-(const coord_t &other) const
    {
        coord_t res {*this};
        
        res.x -= other.x;
        res.y -= other.y;
        res.z -= other.z;
        
        return res;
    }
    
    friend ostream &operator<<(ostream &os, const coord_t &coord)
    {
        return os << coord.x << "," << coord.y << "," << coord.z;
    }
};

struct cube_t
{
    private:
        
        //      | /
        //      |/
        // -----o----- X
        //     /|
        //    / |
        //   Z  Y
        //
        //X: width, Y: height, Z: depth
        enum ROTATE
        {
            X, Y, Z
        };
        
        uint8_t number_of_rotates {};
        enum POSITIONS
        {
            F, R, U, L, D, B
        } pos;
    public:
        vector<coord_t> scanners {1};
        set<coord_t> sensors {};
    
    public:
        void next_orientation()
        {
            rotate(Z);
            if (++number_of_rotates == 4)
            {
                number_of_rotates = 0;
                if (pos == F || pos == L)
                {
                    rotate(Y);
                    if (pos == F)
                        pos = R;
                    else
                        pos = B;
                }
                else if (pos == R || pos == D || pos == B || pos == U)
                {
                    rotate(X);
                    switch (pos)
                    {
                        case R:
                            pos = D;
                            break;
                        case D:
                            pos = L;
                            break;
                        case B:
                            pos = U;
                            break;
                        case U:
                            pos = F;
                            break;
                    }
                }
                else
                {
                    cout << "Bad switch" << endl;
                    exit(1);
                }
            }
        }
        
        pair<coord_t, int8_t> is_overlapping(const cube_t &other) const
        {
            const uint8_t MINIMUM_OVERLAPPING_SENSORS = 12;
            cube_t temp = other;
            for (int i = 0; i < 24; ++i) //Loop through all the possible orientations
            {
                for (coord_t sensor : temp.sensors)
                {
                    for (coord_t fixed_sensor : sensors)
                    {
                        cube_t cube_orientation = temp;
                        //fixed_sensor + diff = sensor
                        //diff = sensor - fixed_sensor
                        coord_t diff = sensor - fixed_sensor;
                        cube_orientation.translate(diff, 0);
                        if (fixed_sensor == coord_t(-618, -824, -621) && diff == coord_t(-68, 1246, 43))
                            cout << "Should match: " << diff << endl;
                        uint8_t overlapping_sensors {};
                        auto itr = cube_orientation.sensors.begin();
                        for (int j = 0; j < cube_orientation.sensors.size(); ++j)
                        {
                            if (sensors.contains(*itr))
                            {
                                if (++overlapping_sensors >= MINIMUM_OVERLAPPING_SENSORS)
                                    break;
                            }
                            else if (cube_orientation.sensors.size() - j - 1 < MINIMUM_OVERLAPPING_SENSORS - overlapping_sensors)
                                break;
                            itr++;
                        }
                        if (overlapping_sensors >= MINIMUM_OVERLAPPING_SENSORS)
                            return make_pair(diff, i);
                    }
                }
                temp.next_orientation();
            }
            return make_pair(coord_t(), -1);
        }
        
        void combine(const cube_t &other, pair<coord_t, int8_t> overlap)
        {
            cube_t temp = other;
            temp.translate(overlap.first, overlap.second);
            
            for (coord_t coord : temp.sensors)
                sensors.insert(coord);
            
            for (coord_t scanner : temp.scanners)
                scanners.emplace_back(scanner);
        }
    
    private:
        void translate(const coord_t &difference, int8_t orientation)
        {
            for (int8_t i = 0; i < orientation; ++i)
                next_orientation();
    
            vector<coord_t> sensors_temp {sensors.begin(), sensors.end()};
            
            for (coord_t &sensor : sensors_temp)
                sensor -= difference;
            
            sensors = {sensors_temp.begin(), sensors_temp.end()};
            
            for (coord_t &scanner : scanners)
                scanner -= difference;
        }
        
        //Rotates the cube clockwise with the axis chosen.
        void rotate(ROTATE dir)
        {
            vector<coord_t> sensors_temp {sensors.begin(), sensors.end()};
            switch (dir)
            {
                case X:
                    for (coord_t &scanner : scanners)
                        scanner = coord_t(scanner.x, -scanner.z, scanner.y);
                    for (coord_t &coord : sensors_temp)
                        coord = coord_t(coord.x, -coord.z, coord.y);
                    break;
                case Y:
                    for (coord_t &scanner : scanners)
                        scanner = coord_t(scanner.z, scanner.y, -scanner.x);
                    for (coord_t &coord : sensors_temp)
                        coord = coord_t(coord.z, coord.y, -coord.x);
                    break;
                case Z:
                    for (coord_t &scanner : scanners)
                        scanner = coord_t(-scanner.y, scanner.x, scanner.z);
                    for (coord_t &coord : sensors_temp)
                        coord = coord_t(-coord.y, coord.x, coord.z);
                    break;
                default:
                    cout << "Bad switch dir" << endl;
                    exit(1);
            }
            sensors = {sensors_temp.begin(), sensors_temp.end()};
        }
    
    public:
        friend ostream &operator<<(ostream &os, const cube_t &cube)
        {
            for (coord_t coord : cube.sensors)
                os << coord << "\n";
            return os;
        }
};

template<typename T>
T pop_front(vector<T> &vec)
{
    T front = vec.at(0);
    vec = {vec.begin() + 1, vec.end()};
    
    return front;
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
    
    
    vector<cube_t> cubes {};
    
    string temp {};
    while (!file.eof())
    {
        file >> temp;
        if (temp.find(',') == -1)
        {
            if (temp[0] >= '0' && temp[0] <= '9')
                cubes.emplace_back(cube_t());
            continue;
        }
        cubes.at(cubes.size() - 1).sensors.insert(coord_t(temp));
    }
    file.close();
    
    /*
    for (int i = 0; i < 24; ++i)
    {
        cout << cubes.at(0) << "\n\n";
        cubes.at(0).next_orientation();
    }
    */
    
    cube_t complete_map = pop_front(cubes);
    double total = cubes.size();
    
    
    while (!cubes.empty())
    {
        cube_t next = pop_front(cubes);
        auto overlap = complete_map.is_overlapping(next);
        if (overlap.second == -1)
        {
            cubes.emplace_back(next);
            continue;
        }
        else
            cout << (total - cubes.size()) * 100 / total << "%" << endl;
        complete_map.combine(next, overlap);
    }
    
    cout << complete_map.sensors.size() << endl;
    
    int64_t max_distance {};
    for (int i = 0; i < complete_map.scanners.size(); ++i)
        for (int j = i + 1; j < complete_map.scanners.size(); ++j)
        {
            coord_t s1 = complete_map.scanners.at(i), s2 = complete_map.scanners.at(j);
            int64_t temp_max = abs(s1.x - s2.x) + abs(s1.y - s2.y) + abs(s1.z - s2.z);
            max_distance = temp_max > max_distance ? temp_max : max_distance;
        }
        
    
    cout << max_distance << endl;
    
    return EXIT_SUCCESS;
}
