#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

using namespace std;

#define DEBUG 0

ostream &operator<<(ostream &os, vector<uint8_t> vec)
{
    for (uint8_t val : vec)
        os << (uint16_t)val << ", ";
    return os;
}

struct bit_stream
{
    vector<uint8_t> values {};
    uint8_t counter {};
    
    bit_stream(const vector<uint8_t> &values) : values(values)
    {}
    
    uint64_t get_n_next_bits(uint8_t count)
    {
        uint64_t res {};
        while (count > 0)
        {
            uint64_t current_byte = (values.at(0) & (UCHAR_MAX >> counter));
            uint8_t bits_to_take = count > (8 - counter) ? 8 - counter : count; //How many bits to read
            
            current_byte >>= (8 - counter - bits_to_take);
            res = (res << bits_to_take) | current_byte;
            count -= bits_to_take;
            counter += bits_to_take;
            if (counter >= 8)
            {
                counter %= 8;
                values = {values.begin() + 1, values.end()};
            }
        }
        return res;
    }
};


struct packet
{
    uint8_t version_number {}, type_id {}, length_type_id {};
    uint64_t literal_value {};
    uint8_t number_of_literal_groups {};
    vector<packet> sub_packets {};
    
    packet(bit_stream &data)
    {
        version_number = data.get_n_next_bits(3);
        type_id = data.get_n_next_bits(3);
        
        if (type_id == 4) //literal value
        {
            bool resume {true};
            while (resume)
            {
                ++number_of_literal_groups;
                resume = data.get_n_next_bits(1);
                literal_value = (literal_value << 4) | data.get_n_next_bits(4);
            }
        }
        //else if (type_id == 6) //the packet is an operator
        else
        {
            length_type_id = data.get_n_next_bits(1);
            if (length_type_id == 0)
            {
                uint16_t total_length_in_bits = data.get_n_next_bits(15);
                uint64_t bits_read {};
                while (bits_read < total_length_in_bits)
                {
                    sub_packets.emplace_back(packet(data));
                    bits_read += sub_packets.back().get_number_of_bits();
                }
            }
            else if (length_type_id == 1)
            {
                uint16_t number_of_sub_packets = data.get_n_next_bits(11);
                for (int i = 0; i < number_of_sub_packets; ++i)
                    sub_packets.emplace_back(packet(data));
            }
        }
    }
    
    uint64_t get_number_of_bits()
    {
        uint64_t bits = 3 + 3; //version number and type id
        if (type_id == 4)
        {
            bits += number_of_literal_groups * 5;
        }
        else
        {
            bits += length_type_id == 0 ? 15 : 11;
            ++bits;
            for (packet p : sub_packets)
                bits += p.get_number_of_bits();
        }
        return bits;
    }
    
    uint64_t get_version_sum()
    {
        uint64_t res = version_number;
        for (packet &p : sub_packets)
            res += p.get_version_sum();
        
        return res;
    }
    
    uint64_t evaluate() const
    {
        uint64_t res {};
        switch (type_id)
        {
            case 0:
                for_each(sub_packets.begin(), sub_packets.end(), [&](const packet &p)
                {
                    res += p.evaluate();
                });
                break;
            case 1:
                res = 1;
                for_each(sub_packets.begin(), sub_packets.end(), [&](const packet &p)
                {
                    res *= p.evaluate();
                });
                break;
            case 2:
                res = ULONG_LONG_MAX;
                for_each(sub_packets.begin(), sub_packets.end(), [&](const packet &p)
                {
                    uint64_t temp = p.evaluate();
                    if (temp < res)
                        res = temp;
                });
                break;
            case 3:
                res = 0;
                for_each(sub_packets.begin(), sub_packets.end(), [&](const packet &p)
                {
                    uint64_t temp = p.evaluate();
                    if (temp > res)
                        res = temp;
                });
                break;
            case 4:
                return literal_value;
            case 5:
                return sub_packets.at(0).evaluate() > sub_packets.at(1).evaluate() ? 1 : 0;
            case 6:
                return sub_packets.at(0).evaluate() < sub_packets.at(1).evaluate() ? 1 : 0;
            case 7:
                return sub_packets.at(0).evaluate() == sub_packets.at(1).evaluate() ? 1 : 0;
        }
        
        return res;
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
    
    string temp {};
    file >> temp;
    file.close();
    
    uint32_t counter {};
    uint8_t val {};
    vector<uint8_t> values {};
    unordered_map<char, uint8_t> hex_values {{'0', 0},
                                             {'1', 1},
                                             {'2', 2},
                                             {'3', 3},
                                             {'4', 4},
                                             {'5', 5},
                                             {'6', 6},
                                             {'7', 7},
                                             {'8', 8},
                                             {'9', 9},
                                             {'A', 10},
                                             {'B', 11},
                                             {'C', 12},
                                             {'D', 13},
                                             {'E', 14},
                                             {'F', 15}};
    for (char &hex : temp)
    {
        val |= hex_values.find(hex)->second;
        if (++counter % 2 == 0)
        {
            values.emplace_back(val);
            val = 0;
        }
        val = val << 4;
    }
    bit_stream bits(values);
    packet packet(bits);
    
    cout << packet.get_version_sum() << endl;
    cout << packet.evaluate() << endl;
    
    return EXIT_SUCCESS;
}
