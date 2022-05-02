#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cctype>

using namespace std;

#define DEBUG 0

template<typename T>
struct c_stack
{
    T* elements {};
    uint64_t head {}, size {};
    bool empty {true};
    
    c_stack()
    {
        elements = new T[2];
        size = 2;
    }
    
    virtual ~c_stack()
    {
        delete [] elements;
    }
    
    void push(const T element)
    {
        if (head >= size)
            expand();
        elements[head++] = element;
        if (empty)
            empty = false;
    }
    
    T pop()
    {
        if (head > 0)
            return elements[head--];
        else if (head == 0 && !empty)
        {
            empty = true;
            return elements[0];
        }
    }
    
    private:
        void expand()
        {
            T *new_elements = new T[size << 1];
            memcpy(new_elements, elements, sizeof(T) * size);
            delete elements;
            elements = new_elements;
        }
};


struct node
{
    string name {};
    bool is_big_cave {}, visited {false};
    set<node *> connected_nodes {};
    
    explicit node(const string &name) : name(name)
    {
        is_big_cave = (bool)isupper(name[0]);
    }
    
    node(const node &other)
    {
        name = other.name;
        is_big_cave = other.is_big_cave;
        visited = other.visited;
    }
    
};

struct graph_t
{
    vector<node *> nodes {};
        
    virtual ~graph_t()
    {
        for (node *node_ptr : nodes)
            delete node_ptr;
    }
    
    explicit graph_t() = default;
    
    graph_t(const graph_t &other)
    {
        nodes.reserve(other.nodes.size());
        for (node *node_ptr : other.nodes)
        {
            node *new_node = get_node(node_ptr->name);
            new_node->visited = node_ptr->visited;
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

uint64_t get_number_of_paths(graph_t graph, const string &start_node_name, const string &end_node_name)
{
    uint64_t count {};
    
    if (start_node_name == end_node_name)
        return 1; //Found end node
    
    node *start_node = graph.get_node(start_node_name);
    
    start_node->visited = true;
    
    for (node *node_ptr : start_node->connected_nodes)
        if (node_ptr->is_big_cave || !node_ptr->visited)
            count += get_number_of_paths(graph, node_ptr->name, end_node_name);
    
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
    
    //Handling the data
    cout << get_number_of_paths(caves, "start", "end") << endl;
    
    return EXIT_SUCCESS;
}
