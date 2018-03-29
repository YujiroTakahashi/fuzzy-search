#include <iostream>
#include <time.h>

#include "trie.h"

int main(int argc, const char** argv)
{
    croco::Trie trie;
    croco::Trie::retval_t results;
#if 1
    {
        std::cout << "辞書の読み込み開始.\n";
        clock_t start = clock();
        trie.loadDictionary("data/dictionary.txt");
        clock_t end = clock();     // 終了時間
        std::cout << "読み込み時間：" << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n\n\n";
    }
#endif
    {
        std::cout << "コストの読み込み開始.\n";
        clock_t start = clock();
        trie.loadCost("data/connection.txt");
        clock_t end = clock();     // 終了時間
        std::cout << "読み込み時間：" << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n\n\n";
    }
#if 1
    {
        std::cout << "検索....\n";
        clock_t start = clock();
        uint32_t cost = 2;

        trie.search("シュミレーション", cost, results);
        for (auto &res: results) {
            std::cout << "" << res.first << "\t";
            std::cout << "::" << res.second << "\n";
        }

        clock_t end = clock();     // 終了時間
        std::cout << "\n検索時間：" << (double)(end - start) / CLOCKS_PER_SEC << "sec." << std::endl;
    }
#endif
    return 0;
}
