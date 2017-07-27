#include <aho_corasick.hpp>
#include <iostream>

aho_corasick ac;
vector<string> strs = {"aab", "aax", "vbb"};
string str = "aabb";
vector<uint32_t> ans;

int main() {
    ac.create_trie(strs);
    ac.compute_suffixes();

    ans = ac.valid("aabb");
    for (const uint32_t i : ans)
        cout<<i<<" "; cout<<endl;
}
