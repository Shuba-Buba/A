#include <iostream>
#include <vector>
#include <algorithm>
#include <list>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
	std::vector<std::list<std::pair<const KeyType, ValueType>>> Table;
	Hash hasher;
	size_t count_element = 0;


	template <class Type_of_Vector>
	void update(std::vector<std::list<Type_of_Vector>>& A, size_t new_len) {
		std::vector<std::list<Type_of_Vector>> B(new_len);
		for (auto &bucket : A) {
			for (auto &to : bucket) {
				size_t pos = hasher(to.first) % new_len;
				B[pos].push_back(to);
			}
		}
		this->Table = std::move(B);
	}


public:

	using Pa = std::pair<const KeyType, ValueType>;
	class iterator {
		private:
			typename std::vector<std::list<Pa>>::iterator bucket_tail;
			typename std::list<Pa>::iterator ind;
			std::vector<std::list<Pa>> *master;
		public:
			iterator() = default;

			iterator(const iterator &other) {
				ind = other.ind;
				master = other.master;
				bucket_tail = other.bucket_tail;
			}

			iterator(typename std::vector<std::list<Pa>>::iterator A,
				typename std::list<Pa>::iterator B, std::vector<std::list<Pa>> *C) {
				bucket_tail = A;
				ind = B;
				master = C;
			}

			bool operator==(const iterator &other) {
				return (other.ind == ind && other.bucket_tail == bucket_tail);
			}

			bool operator!=(const iterator &other) {
				return (other.ind != ind || other.bucket_tail != bucket_tail || other.master != master);
			}

			iterator &operator++() {
				if (bucket_tail == master->end()) {
					return *this;
				}
				++ind;
				if (ind == bucket_tail->end()) {
					while (true) {
						++bucket_tail;
						if (bucket_tail == master->end()) {
							ind = master->begin()->begin();
							return *this;
						}

						if (bucket_tail->size() != 0) {
							ind = bucket_tail->begin();
							return *this;
						}
					}
					ind = bucket_tail-> begin();
					return *this;
				}
				return *this;
			}
			iterator operator++(int) {
				auto alien(*this);
				++(*this);
				return alien;
			}

			typename std::list<Pa>::iterator operator->() {
				return ind;
			}
			Pa& operator*() {
				return *ind;
			}
	};

	class const_iterator {
		private:
			typename std::vector<std::list<Pa>>::const_iterator bucket_tail;
			typename std::list<Pa>::const_iterator ind;
			const std::vector<std::list<Pa>> *master;
		public:
			const_iterator() = default;

			const_iterator(const const_iterator &other) {
				ind = other.ind;
				master = other.master;
				bucket_tail = other.bucket_tail;
			}

			const_iterator(const typename std::vector<std::list<Pa>>::const_iterator A,
				const typename std::list<Pa>::const_iterator B, const std::vector<std::list<Pa>> *C) {
				bucket_tail = A;
				ind = B;
				master = C;
			}

			bool operator==(const const_iterator &other) const {
				return (other.ind == ind && other.bucket_tail == bucket_tail);
			}

			bool operator!=(const const_iterator &other) const {
				return (other.ind != ind || other.bucket_tail != bucket_tail || other.master != master);
			}

			const_iterator &operator++() {
				if (bucket_tail == master->end()) {
					return *this;
				}
				++ind;
				if (ind == bucket_tail->end()) {
					while (true) {
						++bucket_tail;
						if (bucket_tail == master->end()) {
							ind = master->begin()->begin();
							return *this;
						}

						if (bucket_tail->size() != 0) { //mb nado ispravit
							ind = bucket_tail->begin();
							return *this;
						}
					}
					ind = bucket_tail-> begin();
					return *this;
				}
				return *this;
			}
			const_iterator operator++(int) {
				auto alien(*this);
				++(*this);
				return alien;
			}

			typename std::list<Pa>::const_iterator operator->() const {
				return ind;
			}
			const Pa& operator*() const {
				return *ind;
			}
	};

	void print() {
		for (auto to : Table) {
			for (auto kek : to) {
				std::cout << kek.first << ' ' << kek.second << "\n";
			}
		}
	}

	HashMap() {
		Table.resize(2);
		count_element = 0;
		hasher = std::hash<KeyType>();
	}

	void insert(const std::pair<KeyType,ValueType> &A) {
		size_t pos = hasher(A.first) % Table.size();
		bool ok = false;
		for (auto &to : Table[pos]) {
			if (to.first == A.first) {
				ok = true;
				break;
			}
		}
		if (!ok) {
			Table[pos].push_back(A);
			++count_element;
		}
		if (count_element == Table.size()) {
			update(Table, Table.size() * 2);
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

	size_t size() const {
        return count_element;
    }


    bool empty() const {
        return (count_element == 0);
    }

    Hash hash_function() const {
        return hasher;
    }

    void erase(const KeyType &A) {
    	size_t pos = hasher(A) % Table.size();
    	for (auto iter = Table[pos].begin(); iter != Table[pos].end(); ++iter) {
    		if (iter->first == A) {
    			Table[pos].erase(iter);
    			--count_element;
    			break;
    		}
    	}
    	if (count_element <= Table.size() / 4) {
    		size_t new_size = count_element * 2;
    		if (new_size == 0)
    			new_size = 2;
    		update(Table, new_size);
    	}
    }

    iterator begin() {
    	for (auto to = Table.begin(); to != Table.end(); ++to) {
    		if (to->size() != 0) {
    			return iterator(to, to->begin(), &Table);
    		}
    	}
    	return iterator(Table.end(), Table.begin()->begin(), &Table);
    }

    iterator end() {
    	return iterator(Table.end(), Table.begin()->begin(), &Table);
    }

    const_iterator begin() const {
    	for (auto to = Table.begin(); to != Table.end(); ++to) {
    		if (to->size() != 0) {
    			return const_iterator(to, to->begin(), &Table);
    		}
    	}
    	return const_iterator(Table.end(), Table.begin()->begin(), &Table);
    }

    const_iterator end() const {
    	return const_iterator(Table.end(), Table.begin()->begin(), &Table);
    }

       iterator find(const KeyType &key) {
        size_t pos = hasher(key) % Table.size();
        for (auto to = Table[pos].begin(); to != Table[pos].end(); ++to) {
            if (to->first == key) {
                return iterator(Table.begin() + pos, to, &Table);
            }
        }
        return end();
    }

    const_iterator find(const KeyType &key) const {
        size_t pos = hasher(key) % Table.size();
        for (auto to = Table[pos].begin(); to != Table[pos].end(); ++to) {
            if (to->first == key) {
                return const_iterator(Table.begin() + pos, to, &Table);
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
        return to->second;
    }

    HashMap(const HashMap &other) {
        clear();
        hasher = other.hasher;
        for (const auto &to : other) {
            insert(to);
        }

    }

    HashMap &operator=(const HashMap &other) {
    	if (this == &other) {
            return *this;
        }
        this->clear();
        this->hasher = other.hasher;
        for (const auto &it : other) {
            this->insert(it);
        }
        return *this;
    }

    HashMap(Hash _) : hasher(_) {
        clear();
    }

    void clear() {
    	Table.clear();
    	Table.resize(2);
        count_element = 0;
    }
};