#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

using namespace std;

#define DEBUG 0

struct snail_number_t
{
    string number {};
    
    explicit snail_number_t(string number) : number(std::move(number))
    {}
    
    void make_number_valid()
    {
        while (true)
        {
            if (explode())
                continue;
            if (split())
                continue;
            break;
        }
    }
    
    bool explode()
    {
        uint8_t depth {};
        for (uint16_t i = 0; i < number.size(); ++i)
        {
            if (number[i] == '[')
                ++depth;
            else if (number[i] == ']')
                --depth;
            
            if (depth > 4)
            {
                uint16_t end_index = i + 1;
                while (number[end_index] != ']')
                    ++end_index;
                
                string exploding_number = number.substr(i,  end_index - i + 1);
                string left_number = exploding_number.substr(1, exploding_number.find(',') - 1);
                string right_number = exploding_number.substr(exploding_number.find(',') + 1,  exploding_number.size() - exploding_number.find(',') - 2);
                
                uint8_t left {(uint8_t)stoi(left_number)}, right {(uint8_t)stoi(right_number)};
                
                number.replace(i, end_index - i + 1, "0");
                
                string left_part_of_number = number.substr(0, i), right_part_of_number = number.substr(i);
                for (int32_t j = left_part_of_number.size() - 1; j >= 0; --j) //add the left value to the next number to the left
                {
                    if (left_part_of_number.at(j) != '[' && left_part_of_number.at(j) != ']' && left_part_of_number.at(j) != ',')
                    {
                        uint8_t length_of_digit {1};
                        uint8_t old_left {(uint8_t)(left_part_of_number.at(j) - '0')};
                        if (j > 1 && left_part_of_number.at(j - 1) != '[' && left_part_of_number.at(j - 1) != ']' && left_part_of_number.at(j - 1) != ',')
                        {
                            uint8_t temp {(uint8_t)(left_part_of_number.at(j - 1) - '0')};
                            old_left = temp * 10 + old_left;
                            ++length_of_digit;
                            --j;
                        }
                        
                        left_part_of_number.replace(j, length_of_digit, to_string(old_left + left));
                        break;
                    }
                }
                
                for (int32_t j = 1; j < right_part_of_number.size(); ++j) //add the right value to the next number to the right
                {
                    if (right_part_of_number.at(j) != '[' && right_part_of_number.at(j) != ']' && right_part_of_number.at(j) != ',')
                    {
                        uint8_t length_of_digit {1};
                        uint8_t old_right {(uint8_t)(right_part_of_number.at(j) - '0')};
                        if (j < right_part_of_number.size() - 1 && right_part_of_number.at(j + 1) != '[' && right_part_of_number.at(j + 1) != ']' && right_part_of_number.at(j + 1) != ',')
                        {
                            uint8_t temp {(uint8_t)(right_part_of_number.at(j + 1) - '0')};
                            old_right = old_right * 10 + temp;
                            ++length_of_digit;
                        }
                        
                        right_part_of_number.replace(j, length_of_digit, to_string(old_right + right));
                        break;
                    }
                }
                number = left_part_of_number + right_part_of_number;
                return true;
            }
        }
        return false;
    }
    
    bool split()
    {
        for (uint32_t i = 0; i < number.size(); ++i)
        {
            if (number.at(i) != '[' && number.at(i) != ']' && number.at(i) != ',')
            {
                uint8_t value {(uint8_t)(number.at(i) - '0')};
                if (i < number.size() - 1 && number.at(i + 1) != '[' && number.at(i + 1) != ']' && number.at(i + 1) != ',')
                    value = value * 10 + (uint8_t)(number.at(i + 1) - '0');
                else
                    continue;
                
                uint8_t left = value / 2, right = value - left;
                number.replace(i, 2, string("[" + to_string(left) + "," + to_string(right) + "]"));
                return true;
            }
        }
        return false;
    }
    
    uint64_t get_magnitude() const
    {
        string num = number;
        
        for (uint16_t index = num.find_first_of(']'); index != USHRT_MAX; index = num.find_first_of(']'))
        {
            uint16_t start_bracket = index;
            while (num.at(start_bracket) != '[')
                --start_bracket;
            
            string exploding_number = num.substr(start_bracket,  index - start_bracket + 1);
            string left_number = exploding_number.substr(1, exploding_number.find(',') - 1);
            string right_number = exploding_number.substr(exploding_number.find(',') + 1,  exploding_number.size() - exploding_number.find(',') - 2);
            
            uint64_t left {(uint64_t)stoi(left_number)}, right {(uint64_t)stoi(right_number)};
            uint64_t magnitude = 3 * left + 2 * right;
            
            num.replace(start_bracket, index - start_bracket + 1, to_string(magnitude));
        }
        
        return stoi(num);
    }
    
    snail_number_t &operator+=(const snail_number_t &other)
    {
        number = "[" + number + "," + other.number + "]";
        make_number_valid();
        return *this;
    }
    
    snail_number_t operator+(const snail_number_t &other)
    {
        snail_number_t new_number("[" + number + "," + other.number + "]");
        new_number.make_number_valid();
     
        return new_number;
    }
    
    friend ostream &operator<<(ostream &os, const snail_number_t &snail_number)
    {
        return os << snail_number.number;
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
    
    vector<snail_number_t> numbers {};
    string temp {};
    while (!file.eof())
    {
        file >> temp;
        numbers.emplace_back(temp);
    }
    file.close();
    
    snail_number_t sum {*numbers.begin()};
    
    for (int i = 1; i < numbers.size(); ++i)
        sum += numbers.at(i);
    
    cout << sum.get_magnitude() << endl;
    
    uint64_t max_magnitude {0};
    for (int i = 0; i < numbers.size(); ++i)
    {
        for (int j = i + 1; j < numbers.size(); ++j)
        {
            uint64_t val = (numbers.at(i) + numbers.at(j)).get_magnitude();
            max_magnitude = max_magnitude < val ? val : max_magnitude;
            
            val = (numbers.at(j) + numbers.at(i)).get_magnitude();
            max_magnitude = max_magnitude < val ? val : max_magnitude;
        }
    }
    
    cout << max_magnitude << endl;
    
    return EXIT_SUCCESS;
}
