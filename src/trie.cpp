#include <iostream>

#include "trie.h"

namespace croco {

/**
 * コンストラクタ
 *
 * @access public
 * @param  std::string modelpath
 */
Trie::Trie()
{
    _nodes.emplace_front();
    _it = _nodes.begin();
}

/**
 * デストラクタ
 *
 * @access public
 */
Trie::~Trie()
{
    _nodes.clear();
}

/**
 * 辞書の読み込み
 *
 * @access public
 * @param  string dicfile
 * @return bool
 */
bool Trie::loadDictionary(std::string dicfile)
{
    std::ifstream ifs(dicfile);
    if (!ifs.is_open()) {
        return false;
    }

    uint16_t lid, rid;
    uint32_t distance;
    std::string line;
    while (std::getline(ifs, line)) {
        strs_t vals = _split(line, '\t');
        lid = std::stoul(vals.at(DICT_LEFT_ID));
        rid = std::stoul(vals.at(DICT_LEFT_ID));
        distance = std::stoul(vals.at(DICT_DISTANCE));
        _insert(vals.at(DICT_SURFACE), lid, rid, distance);
    }
    ifs.close();

    return true;
}

/**
 * コストの読み込み
 *
 * @access public
 * @param  string costfile
 * @return bool
 */
bool Trie::loadCost(std::string costfile)
{
    std::ifstream ifs(costfile);
    if (!ifs.is_open()) {
        return false;
    }

    /* ヘッダー：：サイズ情報取得 */
    std::string line;
    std::getline(ifs, line);
    strs_t vals = _split(line, ' ');

    _costs.clear();

    uint16_t lid, rid;
    uint32_t cost, idx;
    while (std::getline(ifs, line)) {
        std::vector<std::string> vals = _split(line, ' ');

        lid = std::stoi(vals.at(COST_LEFT_ID));
        rid = std::stoi(vals.at(COST_RIGHT_ID));
        cost = std::stoi(vals.at(COST_VALUE));
        idx = _encodeKey(lid, rid);

        _costs[idx] = static_cast<uint32_t>(cost);
    }
    ifs.close();

    return true;
}


/**
 * 検索
 *
 * @access public
 * @param  string word
 * @param  uint32_t cost
 * @param  retval_t results
 * @return void
 */
void Trie::search(std::string word, uint32_t cost, retval_t &results)
{
    strs_t words = _split(word);
    int sz = words.size();

    vals_t current_row(sz + 1);
    for (int i = 0; i <= sz; ++i) {
        current_row[i] = i;
    }

    _maxCost = cost;

    Node &node = _nodes.front();
    for (key_child_t child : node.children) {
        _searchImpl(
            child.second,
            child.first,
            words,
            current_row,
            results
        );
    }

    std::sort(results.begin(), results.end(), std::greater<std::pair<uint32_t, std::string>>());
}

/**
 * 検索(再帰検索)
 *
 * @access public
 * @param  string word
 * @param  int cost
 * @param  retval_t results
 * @return void
 */
void Trie::exactMatch(std::string word, retval_t &results)
{
    strs_t words = _split(word);
    Node *node = &(_nodes.front());

    _exactMatch(node, words, 0, results);
}

/**
 * エンコード
 *
 * @access public
 * @param  uint16_t lid
 * @param  uint16_t rid
 * @return uint32_t
 */
uint32_t Trie::_encodeKey(uint16_t lid, uint16_t rid)
{
    return (static_cast<uint32_t>(lid) << 16) | rid;
}

/**
 * 文字列分割
 *
 * @access public
 * @param  const std::string &str
 * @return strs_t
 */
Trie::strs_t Trie::_split(const std::string &str)
{
    strs_t words;
    int sz = str.size();
    for (int idx = 0; idx < sz;) {
        uint8_t hex = static_cast<uint8_t>(str[idx]);
        if ('\n' == str[idx] || ' ' == str[idx]) {
            words.push_back({str[idx], '\0'});
            idx += 1;
        } else if (hex < 0x80) {
            words.push_back({str[idx], '\0'});
            idx += 1;
        } else if (hex < 0xE0) {
            words.push_back({str[idx], str[idx +1], '\0'});
            idx += 2;
        } else if (hex < 0xF0) {
            words.push_back({str[idx], str[idx +1], str[idx +2], '\0'});
            idx += 3;
        } else {
            words.push_back({str[idx], str[idx +1], str[idx +2], str[idx +3], '\0'});
            idx += 4;
        }
    }
    return words;
}

/**
 * 文字列分割
 *
 * @access public
 * @param  const std::string &str
 * @param  char delim
 * @return strs_t
 */
Trie::strs_t Trie::_split(const std::string &str, char delim)
{
    strs_t words;
    std::string item;
    for (char ch: str) {
        if (ch == delim) {
            words.push_back(item);
            item.clear();
        } else {
            item += ch;
        }
    }

    if (!item.empty()) {
        words.push_back(item);
    }
    return words;
}

/**
 * 辞書へ挿入
 *
 * @access private
 * @param  string word
 * @param  int lid
 * @param  int rid
 * @param  int distance
 * @return void
 */
void Trie::_insert(std::string word, uint16_t lid, uint16_t rid, uint32_t distance)
{
    Node *node = static_cast<Node *>(&_nodes.front());

    strs_t words = _split(word);
    int sz = words.size();

    for (int i = 0; i < sz; ++i) {
        if (node->children.find(words[i]) == node->children.end()) {
            _it = _nodes.emplace_after(_it);
            node->children[words[i]] = static_cast<Node *>(&(*_it));
        }
        node = node->children[words[i]];
    }

    node->word = word;
    node->lid = lid;
    node->distance = distance;
}

/**
 * コストの取得
 *
 * @access private
 * @param uint16_t rid
 * @param uint16_t lid
 * @return uint32_t
 */
uint32_t Trie::_getCost(uint16_t rid, uint16_t lid)
{
    uint32_t idx = _encodeKey(lid, rid);

    if (_costs.find(idx) == _costs.end()) {
        return 0;
    }
    return _costs[idx];
}

/**
 * 完全一致
 *
 * @access private
 * @param Node *node
 * @param strs_t words
 * @param int pos
 * @param retval_t &results
 * @return void
 */
void Trie::_exactMatch(Node *node, strs_t &words, int pos, retval_t &results)
{
    if (words.size() > pos && node->children.find(words[pos]) != node->children.end()) {
        _exactMatch(node->children[words[pos]], words, pos+1, results);
    } else if (!node->word.empty()) {
        results.push_back(
            std::pair<uint32_t, std::string>(0, node->word)
        );
    }
}

/**
 * あいまい検索
 *
 * @access private
 * @param Node *node
 * @param std::string word
 * @param strs_t &words
 * @param vals_t &last_row
 * @param retval_t &results
 * @return void
 */
void Trie::_searchImpl(
    Node *node,
    std::string word,
    strs_t &words,
    vals_t &last_row,
    retval_t &results)
{
    int size = words.size(), insert_del, replace;
    vals_t current_row(size + 1);
    current_row[0] = last_row[0] + 1;

    for (int idx=0; idx < size; ++idx) {
        insert_del = std::min(current_row[idx] + 1, last_row[idx + 1] + 1);
        replace = (words[idx] == word) ? last_row[idx] : (last_row[idx] + 1);

        current_row[idx + 1] = std::min(insert_del, replace);
    }

    if (current_row[size] <= _maxCost && !node->word.empty()) {
        uint32_t score = current_row[size] + _getCost(node->lid, 0);
        score = score + (_maxCost - node->distance) * 5000;
        results.push_back(std::pair<uint32_t, std::string>(score, node->word));
    }

    if (*std::min_element(current_row.begin(), current_row.end()) <= _maxCost) {
        for (key_child_t child : node->children) {
            _searchImpl(
                child.second,
                child.first,
                words,
                current_row,
                results
            );
        }
    }
}

}   // namespace croco
