#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cctype>

using namespace std;

#define DEBUG 0

struct node
{
    string name {};
    bool is_big_cave {};
    uint8_t num_visited {0};
    set<node *> connected_nodes {};
    
    explicit node(const string &name) : name(name)
    {
        is_big_cave = (bool)isupper(name[0]);
    }
    
    node(const node &other)
    {
        name = other.name;
        is_big_cave = other.is_big_cave;
        num_visited = other.num_visited;
    }
    
};

struct graph_t
{
    vector<node *> nodes {};
    bool has_visited_small_twice {false};
        
    virtual ~graph_t()
    {
        for (node *node_ptr : nodes)
            delete node_ptr;
    }
    
    explicit graph_t() = default;
    
    graph_t(const graph_t &other)
    {
        has_visited_small_twice = other.has_visited_small_twice;
        nodes.reserve(other.nodes.size());
        for (node *node_ptr : other.nodes)
        {
            node *new_node = get_node(node_ptr->name);
            new_node->num_visited = node_ptr->num_visited;
            new_node->is_big_cave = node_ptr->is_big_cave;
            
            for (node *connected : node_ptr->connected_nodes)
                new_node->connected_nodes.insert(get_node(connected->name));
        }
    }
    
    void add_edge(const string &edge)
    {
        string first_node_name {edge.substr(0, edge.find_first_of('-'))};
        string second_node_name {edge.substr(edge.find_first_of('-') + 1, edge.length())};
        
        node *first_node = get_node(first_node_name);
        node *second_node = get_node(second_node_name);
        
        first_node->connected_nodes.insert(second_node);
        second_node->connected_nodes.insert(first_node);
    }
    
    node *get_node(const string &name)
    {
        for (node *node_ptr : nodes)
            if (node_ptr->name == name)
                return node_ptr;
        
        nodes.emplace_back(new node(name));
        
        return nodes.back();
    }

};

uint64_t get_number_of_paths(graph_t graph, const string &start_node_name, const string &end_node_name, string path)
{
    uint64_t count {};
    
    if (DEBUG)
        path += "," + start_node_name; //Extend path
    
    if (start_node_name == end_node_name)
    {
        if (DEBUG)
        {
            path = path.substr(1, path.length());
            cout << path << "\n";
        }
        return 1; //Found end node
    }
    
    node *start_node = graph.get_node(start_node_name);
    
    ++start_node->num_visited;
    if (!start_node->is_big_cave && start_node->num_visited == 2)
        graph.has_visited_small_twice = true;
    
    for (node *node_ptr : start_node->connected_nodes)
        if (node_ptr->is_big_cave || node_ptr->num_visited == 0 || (!graph.has_visited_small_twice && node_ptr->name != "start" && node_ptr->name != "end"))
            count += get_number_of_paths(graph, node_ptr->name, end_node_name, path);
    
    return count;
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
    
    graph_t caves {};
    string line {};
    while (!file.eof())
    {
        file >> line;
        caves.add_edge(line);
        line.clear(); //empty the line buffer
    }
    file.close();
    
    cout << "File read" << endl;
    
    //Handling the data
    cout << get_number_of_paths(caves, "start", "end", "") << endl;
    
    return EXIT_SUCCESS;
}
