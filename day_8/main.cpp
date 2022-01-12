#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <string>

using namespace std;

#define DEBUG 0

string unique_list(string &dig);

string operator-(const string &lhs, const string &rhs)
{
    string new_string {lhs};
    vector<int> index;
    for (int i = 0; i < new_string.length(); ++i)
        if (rhs.find(new_string[i]) != -1)
            index.emplace_back(i);
    std::reverse(index.begin(), index.end());
    for (int i : index)
        new_string.erase(i, 1);
    
    return new_string;
}

string operator+(const string &lhs, const string &rhs)
{
    string new_string {lhs};
    new_string += rhs;
    return unique_list(new_string);
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
    
    uint64_t count {};
    while (!file.eof())
    {
        string val {};
        vector<string> digits(10), seven_display(7);
        for (int i = 0; i < 10; ++i)
        {
            file >> val;
            switch (val.length())
            {
                case 2:
                    digits.at(1) = val;
                    break;
                case 3:
                    digits.at(7) = val;
                    break;
                case 4:
                    digits.at(4) = val;
                    break;
                case 7:
                    digits.at(8) = val;
                    break;
                case 5: //2, 3, 5
                    if (digits.at(2) == "")
                        digits.at(2) = val;
                    else if (digits.at(3) == "")
                        digits.at(3) = val;
                    else
                        digits.at(5) = val;
                    break;
                case 6: //0, 6, 9
                    if (digits.at(0) == "")
                        digits.at(0) = val;
                    else if (digits.at(6) == "")
                        digits.at(6) = val;
                    else
                        digits.at(9) = val;
                    break;
            }
        }
        
        for (auto &dig : seven_display)
            dig = "abcdefg";
        file.ignore(3);
        for (auto &dig : digits)
            std::sort(dig.begin(), dig.end());
        
        vector<int> fives = {2, 3, 5}, sixes = {0, 6, 9};
        string temp;
        
        seven_display.at(0) = digits.at(7) - digits.at(1);
        seven_display.at(2) = seven_display.at(5) = digits.at(1);
        seven_display.at(4) = seven_display.at(6) = (digits.at(8) - digits.at(4)) - seven_display.at(0);
        seven_display.at(1) = seven_display.at(3) = (digits.at(4) - seven_display.at(2)) - seven_display.at(4);
        
        for (auto index : sixes)
            if (digits.at(index).find(seven_display.at(3)[0]) == -1 ||
                digits.at(index).find(seven_display.at(3)[1]) == -1)
            {
                if (digits.at(index).find(seven_display.at(3)[0]) == -1)
                    seven_display.at(3) = seven_display.at(3)[0];
                else
                    seven_display.at(3) = seven_display.at(3)[1];
                //Index points to the zero
                if (index != 0)
                {
                    temp = digits.at(0);
                    digits.at(0) = digits.at(index);
                    digits.at(index) = temp; //Switch zero with 6 or 9
                }
                break;
            }
        seven_display.at(1) = seven_display.at(1) - seven_display.at(3);
        
        for (auto index : fives)
            if (digits.at(index).find(seven_display.at(4)[0]) == -1 ||
                digits.at(index).find(seven_display.at(4)[1]) == -1)
            {
                if (digits.at(index).find(seven_display.at(4)[0]) == -1)
                {
                    seven_display.at(4) = seven_display.at(4)[0];
                    break;
                }
                else
                {
                    seven_display.at(4) = seven_display.at(4)[1];
                    break;
                }
            }
        seven_display.at(6) = seven_display.at(6) - seven_display.at(4);
        
        //Place the fives correctly
        for (auto index : fives)
            if (digits.at(index).find(seven_display.at(1)) != -1 && index != 5)
            {
                temp = digits.at(index);
                digits.at(index) = digits.at(5);
                digits.at(5) = temp;
                break;
            }
        if (digits.at(2).find(seven_display.at(4)) == -1)
        {
            temp = digits.at(2);
            digits.at(2) = digits.at(3);
            digits.at(3) = temp;
        }
        
        //Fix display 2 and 5
        if (digits.at(2).find(seven_display.at(2)[0]) != -1)
            seven_display.at(2) = seven_display.at(2)[0];
        else
            seven_display.at(2) = seven_display.at(2)[1];
        seven_display.at(5) = seven_display.at(5) - seven_display.at(2);
        
        //Fix 6 and 9 if needed
        if (digits.at(6).find(seven_display.at(2)) != -1)
        {
            temp = digits.at(6);
            digits.at(6) = digits.at(9);
            digits.at(9) = temp;
        }
        
        uint64_t temp_sum {};
        for (int j = 0; j < 4; ++j)
        {
            file >> temp;
            std::sort(temp.begin(), temp.end());
            for (int i = 0; i < digits.size(); ++i)
                if (digits.at(i) == temp)
                {
                    temp_sum += i * pow(10, 3 - j);
                    break;
                }
        }
        cout << temp_sum << endl;
        count += temp_sum;
        file.ignore();
    }
    file.close();
    
    cout << count << endl;
    
    return EXIT_SUCCESS;
}

string unique_list(string &dig)
{
    string temp {};
    for (char c : dig)
        if (temp.find(c) == -1)
            temp += c;
    sort(temp.begin(), temp.end());
    dig = temp;
    
    return dig;
}
