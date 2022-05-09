#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

#define DEBUG 0

typedef map<char, uint64_t> cache_result_t;
typedef pair<pair<char, char>, uint8_t> cache_key_t;
typedef map<cache_key_t, cache_result_t> dynamic_cache_t;


void do_step(string &poly, const map<string, string> &insertion_rules)
{
    string new_poly {*poly.begin()}, first {}, second {new_poly};
    for (auto itr = ++poly.begin(); itr != poly.end(); ++itr)
    {
        first = second;
        second = *itr;
        
        new_poly += insertion_rules.find(first + second)->second;
        new_poly += second;
    }
    poly = new_poly;
}

cache_key_t get_key(const string &poly, uint8_t iterations)
{
    return  {pair<char, char>(poly.at(0), poly.at(1)), iterations };
}

void add_result(cache_result_t &results, char key, uint64_t val)
{
    auto res = results.find(key);
    if (res == results.end())
        results.insert_or_assign(key, val);
    else
        res->second += val;
}

void add_to_dynamic_cache(dynamic_cache_t &cache, cache_key_t &&key, cache_result_t &results)
{
    auto res = cache.find(key);
    if (res == cache.end())
        cache.insert_or_assign(key, results);
}

map<char, uint64_t> find_occurrences(const string &poly, const map<string, string> &insertion_rules, dynamic_cache_t &cache, uint16_t iterations)
{
    cache_result_t  results {};
    
    if (iterations <= 0)
        return results;
    
    auto res = cache.find(get_key(poly, iterations));
    if (res != cache.end())
    {
        for (auto val : res->second)
            add_result(results, val.first, val.second);
        return results;
    }
    //Not in cache
    string insertion = insertion_rules.find(poly)->second;
    string left_poly = poly.at(0) + insertion;
    string right_poly = insertion + poly.at(1);
    auto left = find_occurrences(left_poly, insertion_rules, cache, iterations - 1);
    add_to_dynamic_cache(cache, get_key(left_poly, iterations - 1), left);
    for (auto val : left)
        add_result(results, val.first, val.second);
    
    add_result(results, insertion.at(0), 1);
    
    auto right = find_occurrences(right_poly, insertion_rules, cache, iterations - 1);
    add_to_dynamic_cache(cache, get_key(right_poly, iterations - 1), left);
    for (auto val : right)
        add_result(results, val.first, val.second);
    
    add_to_dynamic_cache(cache, get_key(poly, iterations), results);
    return results;
}

uint64_t solve_problem(string &poly, const map<string, string> &insertion_rules, uint8_t iterations)
{
    dynamic_cache_t lookup_table {};
    map<char, uint64_t> results {};
    for (int i = 0; i < poly.length() - 1; ++i)
    {
        string key = poly.substr(i, 2);
        auto new_result = find_occurrences(key, insertion_rules, lookup_table, iterations);
        for (auto res : new_result)
            add_result(results, res.first, res.second); //Insert all the new results
    }
    for (char c : poly)
    {
        auto res = results.find(c);
        if (res == results.end())
            results.insert_or_assign(c, 1);
        else
            ++res->second;
    }
    
    uint64_t min {LONG_MAX}, min_index {}, max {0}, max_index {};
    for (auto val : results)
    {
        if (val.second < min)
            min = val.second;
        if (val.second > max)
            max = val.second;
    }
    
    return max - min;
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
    
    map<string, string> insertion_map {};
    string template_polymer {};
    file >> template_polymer;
    string key {}, val {}, temp {};
    while (!file.eof())
    {
        file >> key;
        file >> temp;
        file >> val;
        
        insertion_map.insert_or_assign(key, val);
        
        key.clear();
        temp.clear();
        val.clear();
    }
    file.close();
   
    /* Puzzle 1
    for (int i = 0; i < 10; ++i)
    {
        do_step(template_polymer, insertion_map);
        cout << "Step: " << i << "\n";
        //cout << template_polymer << "\n";
    }
    
    auto max_val = std::max_element(insertion_map.begin(), insertion_map.end(), [](const pair<string, string> &a, const pair<string, string> &b)
    {
        return a.second < b.second;
    });
    
    cout << max_val->second << "\n";
    uint32_t size = *max_val->second.begin() + 1;
    uint32_t *results = new uint32_t[size] {};
    
    for (char &itr : template_polymer)
        ++results[itr];
    
    uint32_t min {results[size - 1]}, min_index {}, max {results[size - 1]}, max_index {};
    for (uint32_t i = 0; i < size; ++i)
    {
        if (results[i] < min && results[i] != 0)
        {
            min = results[i];
            min_index = i;
        }
        if (results[i] > max)
        {
            max = results[i];
            max_index = i;
        }
    }
    
    delete[] results;
    
    cout << "Answer: " << max - min << "\n";
    */
    
    cout << solve_problem(template_polymer, insertion_map, 40) << "\n";
    return EXIT_SUCCESS;
}
