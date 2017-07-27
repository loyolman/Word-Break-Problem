#include <memory>
#include <utility>
#include <tuple>
#include <algorithm>

#include <vector>
#include <string>
#include <queue>

#include <cstdint>

using namespace std;

class node {
private:
    unique_ptr<node> kids['z'-'a'+1];
    node* suffix;

    const uint32_t distance;
    bool valid;
    uint32_t id_string;
public:
    node(uint32_t x) : kids(), suffix(), distance(x), valid(false), id_string(0) {}

    uint32_t get_distance() const {return distance;}

    node* get_at(char c) const {return kids[c-'a'].get();}
    void create_at(char c) {kids[c-'a'] = make_unique<node>(distance+1);}

    node* get_suffix() const {return suffix;}
    void create_suffix(node * p) {suffix = p;}

    bool get_valid() const {return valid;}
    void create_valid(bool v) {valid = v;}

    uint32_t get_id_string() const {return id_string;}
    void create_id_string(uint32_t v) {id_string = v;}
};

class aho_corasick {
private:
    unique_ptr<node> root; //root of trie

public:
    aho_corasick() : root(make_unique<node>(0)) {}

    void create_trie(const std::vector<std::string>&);
    void compute_suffixes();
    std::vector<uint32_t> valid(const std::string&) const;
};

void
aho_corasick::create_trie(const std::vector<std::string>& strs)
{
    for (uint32_t idx=0; idx<strs.size(); ++idx)  {
        node* act_node = root.get();

        for (const char c : strs.at(idx)) {
            if (!(act_node->get_at(c))) act_node->create_at(c);
            act_node = act_node->get_at(c);
        }

        act_node->create_valid(true);
        act_node->create_id_string(idx);
    }
}

void
aho_corasick::compute_suffixes()
{
    root->create_suffix(nullptr);
    queue<pair<node*, char>> q;

    //init queue
    for (char c='a'; c<='z'; ++c)
        if (root->get_at(c))
            q.push({root.get(), c});

    //work on queue
    while (!q.empty()) {
        node* dad;
        node* me;
        char transfer;

        tie(dad, transfer) = q.front();
        me = dad->get_at(transfer);
        q.pop();

        //insert sons of me to queue
        for (char c='a'; c<='z'; ++c)
            if (me->get_at(c))
                q.push({me, c});

        //compute suffix for me
        node* suffix_dad = dad->get_suffix();
        while (suffix_dad && !(suffix_dad->get_at(transfer)))
            suffix_dad=suffix_dad->get_suffix();
        if (!suffix_dad)
            me->create_suffix(root.get());
        else
            me->create_suffix(suffix_dad->get_at(transfer));
    }
}

std::vector<uint32_t>
aho_corasick::valid(const std::string& str) const
{
    std::vector<bool> valid(str.size()+1, false);
    std::vector<uint32_t> id_ending(str.size()+1);
    std::vector<uint32_t> len_ending(str.size()+1);
    node* act_node = root.get();

    valid.at(0)=true;

    for (uint32_t idx=0; idx<str.size(); ++idx) {
        while (act_node && !(act_node->get_at(str[idx])))
            act_node = act_node->get_suffix();
        if (!act_node)
            act_node = root.get();
        else
            act_node = act_node->get_at(str[idx]);

        if (act_node->get_valid() && valid.at(idx+1-(act_node->get_distance())))
            valid.at(idx+1)=true,
            id_ending.at(idx+1)=act_node->get_id_string(),
            len_ending.at(idx+1)=act_node->get_distance();
    }

    //compute answer if exists one
    std::vector<uint32_t> ans;

    if (!valid.at(str.size())) return ans;
    for (uint32_t idx=str.size(); idx!=0; idx=idx-len_ending.at(idx))
        ans.push_back(id_ending.at(idx));

    reverse(ans.begin(), ans.end());
    return ans;
}
