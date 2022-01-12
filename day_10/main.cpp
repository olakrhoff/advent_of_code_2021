#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

#define DEBUG 0
#define TEST 0

#define BUFFER_SIZE 250

enum SCORE
{
    BRACKETS = 3,
    SQUARE = 57,
    CURLY = 1197,
    GREATER = 25137
};

enum SCORE_INCOMPLETE
{
    IN_BRACKETS = 1,
    IN_SQUARE = 2,
    IN_CURLY = 3,
    IN_GREATER = 4
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
    
    string temp;
    char buffer[BUFFER_SIZE] {};
    uint16_t pos {};
    uint64_t sum {};
    vector<uint64_t> scores;
    while (!file.eof())
    {
        file >> temp;
        
        bool corrupt {false};
        for (char c : temp)
        {
            if (corrupt)
                break;
            switch (c)
            {
                case ')':
                    if (buffer[--pos] == '(')
                        buffer[pos] = '\0';
                    else
                    {
                        corrupt = true;
                        sum += SCORE::BRACKETS;
                    }
                    break;
                case '}':
                    if (buffer[--pos] == '{')
                        buffer[pos] = '\0';
                    else
                    {
                        corrupt = true;
                        sum += SCORE::CURLY;
                    }
                    break;
                case ']':
                    if (buffer[--pos] == '[')
                        buffer[pos] = '\0';
                    else
                    {
                        corrupt = true;
                        sum += SCORE::SQUARE;
                    }
                    break;
                case '>':
                    if (buffer[--pos] == '<')
                        buffer[pos] = '\0';
                    else
                    {
                        corrupt = true;
                        sum += SCORE::GREATER;
                    }
                    break;
                default:
                    buffer[pos++] = c;
            }
        }
        
        if (!corrupt) //If not corrupt, then it is incomplete
        {
            uint64_t score {};
            temp = string(buffer);
            std::reverse(temp.begin(), temp.end());
            std::transform(temp.begin(), temp.end(), temp.begin(), [](char c)
            {
                switch (c)
                {
                    case '(':
                        return ')';
                    case '[':
                        return ']';
                    case '{':
                        return '}';
                    case '<':
                        return '>';
                }
            });
            
            for (int i = 0; i < temp.length(); ++i)
            {
                uint64_t adder {0};
                switch (temp[i])
                {
                    case ')':
                        adder = SCORE_INCOMPLETE::IN_BRACKETS;
                        break;
                    case ']':
                        adder = SCORE_INCOMPLETE::IN_SQUARE;
                        break;
                    case '}':
                        adder = SCORE_INCOMPLETE::IN_CURLY;
                        break;
                    case '>':
                        adder = SCORE_INCOMPLETE::IN_GREATER;
                        break;
                }
                score *= 5;
                score += adder;
            }
            scores.emplace_back(score);
        }
        memset(buffer, 0, BUFFER_SIZE);
        pos = 0;
    }
    file.close();
    
    std::sort(scores.begin(), scores.end());
    
    cout << sum << endl;
    cout << scores.at(scores.size() >> 1) << endl;
    
    return EXIT_SUCCESS;
}