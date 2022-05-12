#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

struct node_t
{
    node_t(uint64_t indexId, uint8_t riskValue) : index_id(indexId), risk_value(riskValue)
    {}
    
    uint64_t index_id {}, smallest_risk_sum {UINT64_MAX};
    uint8_t risk_value {};
    vector<node_t *> connected_nodes {};
    /*
    bool operator<(const node_t &rhs) const
    {
        if (index_id < rhs.index_id)
            return true;
        if (rhs.index_id < index_id)
            return false;
        if (smallest_risk_sum < rhs.smallest_risk_sum)
            return true;
        if (rhs.smallest_risk_sum < smallest_risk_sum)
            return false;
        if (risk_value < rhs.risk_value)
            return true;
        if (rhs.risk_value < risk_value)
            return false;
        return connected_nodes < rhs.connected_nodes;
    }
    
    bool operator>(const node_t &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const node_t &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const node_t &rhs) const
    {
        return !(*this < rhs);
    }*/
};

struct graph_t
{
    vector<node_t *> nodes {};
    
    virtual ~graph_t()
    {
        for (auto &val : nodes)
            delete val;
    }
    
    void add_node(uint64_t index, uint64_t risk_value)
    {
        nodes.emplace_back(new node_t(index, risk_value));
    }
    
    node_t *get_node(uint64_t index)
    {
        node_t *node = nodes[index];
        if (node->index_id != index)
            exit(5);
        return node;
        /*
        for (auto &val : nodes)
            if (val->index_id == index)
                return val;
        return nullptr;*/
    }
    
    void connect_nodes(uint64_t first_index, uint64_t second_index)
    {
        node_t *first = get_node(first_index), *second = get_node(second_index);
        first->connected_nodes.emplace_back(second);
        second->connected_nodes.emplace_back(first);
    }
    
    uint16_t find_smallest_risk_path(uint64_t start_index, uint64_t end_index)
    {
        node_t *first = get_node(start_index);
        first->smallest_risk_sum = 0;
        
        vector<node_t *> next_nodes {};
        next_nodes.emplace_back(first);
        make_heap(next_nodes.begin(), next_nodes.end());
        
        while (!next_nodes.empty())
        {
            std::sort_heap(next_nodes.begin(), next_nodes.end(), [](const node_t *first, const node_t *second)
            {
                return first->smallest_risk_sum < second->smallest_risk_sum;
            });
            
            std::pop_heap(next_nodes.begin(), next_nodes.end());
            node_t *current_node = next_nodes.back();
            next_nodes.pop_back();
            
            if (current_node->index_id == end_index)
                return current_node->smallest_risk_sum;
            
            
            for (auto &val : current_node->connected_nodes)
            {
                if (val->smallest_risk_sum <= current_node->smallest_risk_sum + val->risk_value)
                    continue;
                val->smallest_risk_sum = current_node->smallest_risk_sum + val->risk_value;
                next_nodes.push_back(val);
                std::push_heap(next_nodes.begin(), next_nodes.end());
            }
        }
        throw -1;
    }
};

vector<string> increment_grid(const vector<string> &base)
{
    vector<string> new_grid {};
    
    for (auto &row : base)
    {
        string inc_row {};
        for (char col : row)
        {
            if (col == '9')
                inc_row += '1';
            else
                inc_row += (char)(col + 1);
        }
        new_grid.emplace_back(inc_row);
    }
    
    return new_grid;
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
    
    
    vector<string> rows {};
    string temp {};
    while (!file.eof())
    {
        file >> temp;
        rows.emplace_back(temp);
    }
    file.close();
    
    //Part two
    //Expand the map
    vector<vector<string>> grids {};
    grids.emplace_back(rows);
    for (int i = 0; i < 4; ++i)
    {
        grids.emplace_back(increment_grid(grids.at(i)));
    }
    //Create on long row
    vector<string> whole_row {};
    
    for (int row = 0; row < grids.at(0).size(); ++row)
    {
        string new_row {};
        for (int col = 0; col < grids.size(); ++col)
        {
            new_row += grids.at(col).at(row);
        }
        whole_row.emplace_back(new_row);
    }
    
    grids.clear();
    grids.emplace_back(whole_row);
    for (int i = 0; i < 4; ++i)
    {
        grids.emplace_back(increment_grid(grids.at(i)));
    }
    
    vector<string> big_grid {};
    
    for (auto &section : grids)
        for (string &row : section)
            big_grid.emplace_back(row);
    
    
    graph_t grid;
    
    //for (int row = 0; row < rows.size(); ++row)
    for (uint64_t row = 0; row < big_grid.size(); ++row)
        //for (int col = 0; col < rows.at(row).size(); ++col)
        for (uint64_t col = 0; col < big_grid.at(row).size(); ++col)
            //grid.add_node(row * rows.size() + col, rows.at(row).at(col) - '0');
            grid.add_node(row * big_grid.size() + col, big_grid.at(row).at(col) - '0');
    
    //Connect the grid
    //for (int row = 0; row < rows.size(); ++row)
    for (uint64_t row = 0; row < big_grid.size(); ++row)
    {   //for (int col = 0; col < rows.at(row).size(); ++col)
        for (uint64_t col = 0; col < big_grid.at(row).size() - 1; ++col)
        {
            /*
            if (col < rows.at(row).size() - 1)
                grid.connect_nodes(row * rows.size() + col, row * rows.size() + col + 1);
            if (row < rows.size() - 1)
                grid.connect_nodes(row * rows.size() + col, (row + 1) * rows.size() + col);
            */
            if (col < big_grid.at(row).size() - 1)
                grid.connect_nodes(row * big_grid.size() + col, row * big_grid.size() + col + 1);
            if (row < big_grid.size() - 1)
                grid.connect_nodes(row * big_grid.size() + col, (row + 1) * big_grid.size() + col);
        }
    }
    
    //cout << grid.find_smallest_risk_path(0, rows.size() * rows.at(0).size() - 1) << "\n";
    cout << grid.find_smallest_risk_path(0, big_grid.size() * big_grid.at(0).size() - 1) << "\n";
        
    return EXIT_SUCCESS;
}
