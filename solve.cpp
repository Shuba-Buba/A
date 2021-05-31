#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <list>


template <class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
    using PairValue = std::pair<const KeyType, ValueType>;
    std::vector<std::list<typename std::list<PairValue>::iterator>> Table;
    std::list<PairValue> Table1;

    Hash hasher;
    size_t length = 0;

    void reallocation(std::vector<std::list<typename std::list<PairValue>::iterator>>& A, std::list<PairValue> &T2, size_t new_len) {
        std::vector<std::list<iterator>> B(new_len);
        std::list<PairValue> T1;
        for (auto bucket : A) {
            for (auto to = bucket.begin(); to != bucket.end(); ++to) {
                size_t pos = hasher((*to)->first) % new_len;
                if (B[pos].size()) {
                    auto o = B[pos].back();
                    ++o;
                    T1.insert(o, *(*to));
                    o--;
                    B[pos].push_back(o);
                } else {
                    auto o = T1.end();
                    T1.insert(o, *(*to));
                    o--;
                    B[pos].push_back(o);
                }
            }
        }
        this->Table = std::move(B);
        this-> Table1 = std::move(T1);
    }

public:
    using iterator = typename std::list<PairValue>::iterator;
    using const_iterator = typename std::list<PairValue>::const_iterator;
    void insert(const std::pair<KeyType,ValueType> &A) {
        size_t pos = hasher(A.first) % Table.size();
        bool ok = false;
        for (auto to = Table[pos].begin(); to != Table[pos].end(); ++to) {
            if ((*to)->first == A.first) {
                ok = true;
                break;
            }
        }
        if (!ok) {
            if (Table[pos].size()) {
                auto o = Table[pos].back();
                ++o;
                Table1.insert(o, A);
                --o;
                Table[pos].push_back(o);
            } else {
                auto o = Table1.end();
                Table1.insert(o, A);
                o--;
                Table[pos].push_back(o);
            }
            ++length;
        }
        if (length == Table.size()) {
            reallocation(Table, Table1, Table.size() * 2);
        }
    }

    template<class it>
    HashMap(it begin, it end, Hash new_hasher = Hash()) : hasher(new_hasher) {
        Table.resize(2);
        while (begin != end) {
            auto new_element = *begin;
            insert(new_element);
            begin++;
        }
    }

    HashMap(const std::initializer_list<std::pair<const KeyType, ValueType>> &A, Hash new_hasher = Hash()) : hasher(new_hasher) {
        Table.resize(2);
        for (const auto &to : A) {
            insert(to);
        }
    }

        HashMap() : HashMap({}, std::hash<KeyType>()) {
        }

        size_t size() const {
            return length;
        }


        bool empty() const {
            return (length == 0);
        }

        Hash hash_function() const {
            return hasher;
        }

        void erase(const KeyType &key) {
            size_t pos = hasher(key) % Table.size();
            for (auto iter = Table[pos].begin(); iter != Table[pos].end(); ++iter) {
                if ((*iter)->first == key) {
                	Table1.erase(*iter);
                    Table[pos].erase(iter);
                    --length;
                    break;
                }
            }
            if (length <= Table.size() / 4) {
                size_t new_size = length * 2;
                if (new_size == 0) {
                   new_size = 2;
                }
                reallocation(Table, Table1, new_size);
            }
        }
        iterator begin() {
            return Table1.begin();
        }

        iterator end() {
            return Table1.end();
        }

        const_iterator begin() const {
            return Table1.cbegin();
        }

        const_iterator end() const {
           return Table1.end();
        }

        iterator find(const KeyType &key) {
            size_t pos = hasher(key) % Table.size();
            for (auto to = Table[pos].begin(); to != Table[pos].end(); ++to) {
                if ((*to)->first == key) {
                    return *to;
                }
            }
            return end();
        }

        const_iterator find(const KeyType &key) const {
            size_t pos = hasher(key) % Table.size();
            for (auto to = Table[pos].begin(); to != Table[pos].end(); ++to) {
                if ((*to)->first == key) {
                    return *to;
                }
            }
            return end();
        }

        ValueType &operator[](const KeyType &K) {
            auto to = find(K);
            if (to == end()) {
                insert({K, ValueType()});
            }
            to = find(K);
            return to->second;
        }

        const ValueType &at(const KeyType &K) const {
            auto to = find(K);
            if (to == end()) {
           	   throw std::out_of_range("Shuba_Buba");
            }
            return (*to).second;
        }

        HashMap(const HashMap &other) {
            clear();
            hasher = other.hasher;
            for (const auto &to : other.Table1) {
                insert(to);
            }
        }

        HashMap &operator=(const HashMap &other) {
            if (this == &other) {
                return *this;
            }
            this->clear();
            this->hasher = other.hasher;
            for (const auto &it : other.Table1) {
                this->insert(it);
            }
            return *this;
        }

        HashMap(Hash _) : hasher(_) {
            clear();
        }

        void clear() {
            for (size_t t = 0; t < Table.size(); ++t) {
                for (auto &to : Table[t]) {
                    to = Table1.end();
                }
            } 
            Table.clear();
            Table1.clear();
            Table.resize(2);
            length = 0;
        }
};