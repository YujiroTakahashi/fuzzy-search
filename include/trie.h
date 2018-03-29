#ifndef __CROCO_TRIE_H__
#define __CROCO_TRIE_H__

#include <cstdint>

#include <algorithm>
#include <forward_list>
#include <fstream>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace croco {

class Trie {
public:
    typedef std::vector<uint32_t> vals_t;
    typedef std::vector<std::string> strs_t;
    typedef std::vector<std::pair<uint32_t, std::string>> retval_t;
    typedef struct _Node {
        std::string word;
        uint16_t lid;
        uint16_t rid;
        uint32_t distance;
        std::unordered_map<std::string, struct _Node*> children;
    } Node;
    typedef std::pair<std::string, Node*> key_child_t;
    enum {
        DICT_READING,
        DICT_LEFT_ID,
        DICT_RIGHT_ID,
        DICT_DISTANCE,
        DICT_SURFACE
    };
    enum {
        HEADER_LEFT_SIZE,
        HEADER_RIGHT_SIZE
    };
    enum {
        COST_LEFT_ID,
        COST_RIGHT_ID,
        COST_VALUE
    };

private:
    std::forward_list<Node> _nodes;
    decltype(_nodes)::iterator _it;
    uint32_t _maxCost;
    std::unordered_map<uint32_t, uint32_t> _costs;

public:
    Trie();
    ~Trie();
    bool loadDictionary(std::string dicfile);
    bool loadCost(std::string costfile);
    void search(std::string word, uint32_t cost, retval_t &results);
    void exactMatch(std::string word, retval_t &results);

private:
    uint32_t _encodeKey(uint16_t rid, uint16_t lid);
    strs_t _split(const std::string &str);
    strs_t _split(const std::string &str, char delim);
    void _insert(std::string word, uint16_t lid, uint16_t rid, uint32_t distance);
    uint32_t _getCost(uint16_t rid, uint16_t lid);
    void _exactMatch(Node *node, strs_t &words, int pos, retval_t &results);
    void _searchImpl(
        Node *node,
        std::string letter,
        strs_t &words,
        vals_t &last_row,
        retval_t &results);
}; // class Trie

} // namespace croco

#endif // #define __CROCO_TRIE_H__

