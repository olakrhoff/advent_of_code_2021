#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

struct image_t
{
    private:
        string enhancement_string {};
        vector<vector<uint8_t>> pixels {};
    
    public:
        image_t (string &enhancementString, const vector<vector<uint8_t>> &pixels) : enhancement_string(
                move(enhancementString)), pixels(pixels)
        {
            add_padding(100); //Use this to cheat around the infinity problem where all pixels flip between on and off.
        }
        
        void enhance()
        {
            image_t enhanced_image = *this;
            enhanced_image.pixels.clear();
            
            for (int row = 1; row < pixels.size() - 1; ++row)
            {
                enhanced_image.pixels.emplace_back(vector<uint8_t>{});
                for (int col = 1; col < pixels.at(row).size() - 1; ++col)
                {
                    enhanced_image.pixels.at(enhanced_image.pixels.size() - 1).emplace_back(enhancement_string[get_index_for_enhancement(row, col)] == '#');
                }
            }
            enhanced_image.add_padding(0);
            *this = enhanced_image;
        }
        
        uint32_t get_bright_pixels() const
        {
            uint32_t counter {};
            for (const auto &line : pixels)
                for (auto &pixel : line)
                    if (pixel == 1)
                        ++counter;
                    
            return counter;
        }
        
        friend ostream &operator<<(ostream &os, const image_t &image)
        {
            for (vector<uint8_t> row : image.pixels)
            {
                for (uint8_t pixel : row)
                    if (pixel == 1)
                        os << '#';
                    else if (pixel == 0)
                        os << '.';
                    else
                        exit(2);
                os << "\n";
            }
            
            return os;
        }
    
    private:
        uint16_t get_index_for_enhancement(uint64_t height, uint64_t width)
        {
            uint16_t index {};
            for (int row = -1; row < 2; ++row)
            {
                for (int col = -1; col < 2; ++col)
                {
                    index <<= 1;
                    index += pixels.at(height + row).at(width + col);
                }
            }
            
            return index;
        }
        
        void add_padding(uint8_t padding_depth)
        {
            for (int depth = 0; depth < padding_depth; ++depth)
            {
                for (auto &row : pixels)
                {
                    row.insert(row.begin(), 0);
                    row.emplace_back(0);
                }
                pixels.insert(pixels.begin(), vector<uint8_t>(pixels.at(0).size()));
                pixels.emplace_back(vector<uint8_t>(pixels.at(0).size()));
            }
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
    
    
    string enhancement_string {};
    file >> enhancement_string;
    
    vector<vector<uint8_t>> pixels {};
    
    string temp {};
    while (!file.eof())
    {
        file >> temp;
        pixels.emplace_back();
        for (char &c : temp)
            if (c == '.')
                pixels.at(pixels.size() - 1).emplace_back(0);
            else if (c == '#')
                pixels.at(pixels.size() - 1).emplace_back(1);
            else
                cout << "Something wrong" << endl;
    }
    file.close();
    
    
    image_t image(enhancement_string, pixels);
    
    for (int i = 0; i < 50; ++i)
    {
        image.enhance();
        cout << image << endl;
    }
    
    
    cout << image.get_bright_pixels() << endl;
    
    return EXIT_SUCCESS;
}
