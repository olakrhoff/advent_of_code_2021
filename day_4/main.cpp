#include <iostream>
#include <vector>
#include <fstream>
#include <functional>

using namespace std;

#define DEBUG 0

struct bingo_number
{
    uint16_t value {};
    bool checked {false};
};

struct bingo_card
{
    vector<bingo_number> numbers;
    bool bingo {false};
    
    bingo_card(const vector<bingo_number> &numbers) : numbers(numbers)
    {
        if (numbers.size() != 25)
            exit(2); //This should not happen
    }
    
    int get_sum(int last_draw)
    {
        int temp {};
        for (bingo_number val : numbers)
            if (!val.checked)
                temp += val.value;
        return temp * last_draw;
    }
    
    void check_number(uint16_t val)
    {
        for (auto &num : numbers)
            if (num.value == val)
            {
                num.checked = true;
                break; //Does not need to search the others if ewe have found the value we are looking for
            }
    }
    
    bool has_bingo()
    {
        bool row {true}, col {true};
        int count_row {}, count_col {};
        
        for (int i = 0; i < 5; ++i)
        {
            for (int j = 0; j < 5; ++j)
            {
                //Check the rows
                if (!numbers.at(5 * i + j).checked)
                    row = false;
                else
                    ++count_row;
                if (!numbers.at(5 * j + i).checked)
                    col = false;
                else
                    ++count_col;
                if (!row && !col)
                    break;
            }
            if (count_row == 5 || count_col == 5)
            {
                bingo = true;
                return true; //We have a bingo
            }
            else
            {
                count_row = count_col = 0;
                row = col = true;
            }
        }
        return false;
    }
};

int main()
{
    string filename = "../data.txt";
    if (DEBUG)
        filename = "../test_data.txt";
    ifstream file;
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    
    vector<uint16_t> drawn_numbers;
    uint16_t temp_val {};
    char temp {};
    //Read all the numbers that are "drawn"
    while (!file.eof())
    {
        file >> temp_val;
        drawn_numbers.emplace_back(temp_val); //calibrate the ASCII offset
        if (file.peek() == '\n')
            break;
        else if (file.peek() == ',')
            file.ignore(); //read away the comma
    }
    
    //Read the bingo cards
    vector<bingo_card> bingo_cards;
    vector<bingo_number> bingo_card_values;
    bingo_number temp_bingo_number {};
    while (!file.eof())
    {
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
            {
                file >> temp_val;
                temp_bingo_number.value = temp_val;
                bingo_card_values.emplace_back(temp_bingo_number);
                if (file.peek() == '\n')
                    break;
                else if (file.peek() == ' ')
                    file.ignore();
            }
        bingo_cards.emplace_back(bingo_card(bingo_card_values));
        bingo_card_values.clear();
    }
    file.close();
    
    /* Task 1
    for (uint16_t val : drawn_numbers)
        for (bingo_card &b : bingo_cards)
        {
            b.check_number(val);
            if (b.has_bingo())
            {
                cout << b.get_sum(val) << endl;
                return EXIT_SUCCESS;
            }
        }
    */
    
    int number_of_bingos {};
    for (uint16_t val : drawn_numbers)
        for (bingo_card &b : bingo_cards)
        {
            b.check_number(val);
            if (!b.bingo && b.has_bingo())
            {
                if (++number_of_bingos == bingo_cards.size())
                {
                    cout << b.get_sum(val) << endl;
                    return EXIT_SUCCESS;
                }
            }
        }
    
    return EXIT_SUCCESS;
}
