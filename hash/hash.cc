#ifdef NDEBUG
bool const debug = false;
#else
bool const debug = true;
#endif

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include "hash.h"

// Przestrzeń nazw zawierająca obsługę wypisywania informacji diagnostycznych.
namespace {

    // Funkcja wypisuje na wyjście diagnostyczne ciąg znaków w poprawnym formacie.
    //  seq  - ciąg wartosci typu uint64_t
    //  size - dlugość danego ciągu
    void dbg_print_seq(uint64_t const *seq, uint64_t size) {
    	if (!seq) {
	    std::cerr << "NULL";
	} else if (!size)
	    std::cerr << "\"\"";
	else {
	    for (uint64_t i = 0; i < size; i++) {
	    	if (i == 0)
	            std::cerr << "\"";
	      	std::cerr << seq[i];
	      	if (i == size - 1)
	            std::cerr << "\"";
	      	else
	            std::cerr << " ";
	    }
        }
    }

    // Funkcja wypisuje początkowe informacje diagnostyczne dla funkcji hash_create().
    //  h - funkcja haszujaca użyta przy tworzeniu tablicy
    void dbg_pre_hash_create(void const *h) {
        std::cerr << "hash_create(" << h << ")\n";
    }

    // Funkcja wypisuje finalne informacje diagnostyczne dla funkcji hash_create().
    //  id - identyfikator nowo utworzonej tablicy haszujacej
    void dbg_final_hash_create(unsigned long id) {
        std::cerr << "hash_create: hash table #" << id << " created\n";
    }

    // Funkcja wypisuje wstępne informacje diagnostyczne dla funkcji hash_delete().
    //  id - identyfikator usuwanej tablicy
    void dbg_pre_hash_delete(unsigned long id) {
      std::cerr << "hash_delete(" << id << ")\n";
    }

    // Funkcja wypisuje finalne informacje diagnostyczne dla funkcji hash_delete().
    //  id      - identyfikator usuwanej tablicy
    //  existed - informacja okreslająca czy usuwanie się powiodło
    void dbg_final_hash_delete(unsigned long id, bool existed) {
        if (!existed)
            std::cerr << "hash_delete: hash table #" << id << " does not exist\n";
        else
            std::cerr << "hash_delete: hash table #" << id << " deleted\n";
    }

    // Funkcja wypisuje wstępne informacje diagnostyczne dla funkcji hash_size().
    //  id - identyfikator sprawdzanej tablicy
    void dbg_pre_hash_size(unsigned long id) {
      std::cerr << "hash_size(" << id << ")\n";
    }

    // Funkcja wypisuje finalne informacje diagnostyczne dla funkcji hash_size().
    //  id     - identyfikator sprawdzanej tablicy
    //  result - wynik, mówiący ile elementów znajduje sie w danej tablicy, o ile istnieje
    //  exists - informacja mówiąca czy tablica o danym id istnieje
    void dbg_final_hash_size(unsigned long id, int result, bool exists) {
        if (!exists)
            std::cerr << "hash_size: hash table #" << id << " does not exist\n";
        else
            std::cerr << "hash_size: hash table #" << id << " contains " << result << " element(s)\n";
    }


    // Funkcja wypisuje wstępne informacje diagnostyczne dla funkcji hash_insert().
    //  id      - identyfikator tablicy do której wstawiano ciąg
    //  seq     - ciąg wartości typu uint64_t
    //  size    - długość danego ciągu
    void dbg_pre_hash_insert(unsigned long id, uint64_t const *seq, uint64_t size) {
        std::cerr << "hash_insert(" << id << ", ";
        dbg_print_seq(seq, size);
        std::cerr << ", " << size << ")\n";
	}

    // Funkcja wypisuje finalne informacje diagnostyczne dla funkcji hash_insert().
    //  id      - identyfikator tablicy do której wstawiano ciag
    //  seq     - ciąg wartości typu uint64_t
    //  size    - dlugość danego ciśgu
    //  exists  - informacja mowiąca czy tablica o danym id istnieje
    //  present - informacja mowiąca czy dany ciąg należał już do tablicy
    void dbg_final_hash_insert(unsigned long id, uint64_t const *seq, uint64_t size, bool exists, bool present) {
        if (!seq || !size) {
            if (!seq)
                std::cerr << "hash_insert: invalid pointer (NULL)\n";
            if (!size)
                std::cerr << "hash_insert: invalid size (" << size << ")\n";
        } else if (!exists) {
            std::cerr << "hash_insert: hash table #" << id << " does not exist\n";
        } else {
            std::cerr << "hash_insert: hash table #" << id << ", sequence ";
            dbg_print_seq(seq, size);
            if (!present)
                std::cerr << " inserted\n";
            else
                std::cerr << " was present\n";
        }
    }

	// Funkcja wypisuje wstepne informacje diagnostyczne dla funkcji hash_remove().
	//  id      - identyfikator tablicy z ktorej usuwano ciag
	//  seq     - ciag wartosci typu uint64_t
	//  size    - dlugosc danego ciagu
    void dbg_pre_hash_remove(unsigned long id, uint64_t const *seq, uint64_t size) {
    	std::cerr << "hash_remove(" << id << ", ";
    	dbg_print_seq(seq, size);
    	std::cerr << ", " << size << ")\n";
    }

    // Funkcja wypisuje finalne informacje diagnostyczne dla funkcji hash_remove().
    //  id      - identyfikator tablicy z ktorej usuwano ciag
    //  seq     - ciag wartosci typu uint64_t
    //  size    - dlugosc danego ciagu
    //  exists  - informacja mowiaca czy tablica o danym id istnieje
    //  present - informacja mowiaca czy dany ciag nalezal do tablicy
    void dbg_final_hash_remove(unsigned long id, uint64_t const *seq, uint64_t size, bool exists, bool present) {
        if (!seq || !size) {
            if (!seq)
                std::cerr << "hash_remove: invalid pointer (NULL)\n";
            if (!size)
                std::cerr << "hash_remove: invalid size (" << size << ")\n";
        } else if (!exists) {
            std::cerr << "hash_remove: hash table #" << id << " does not exist\n";
        } else {
            std::cerr << "hash_remove: hash table #" << id << ", sequence ";
            dbg_print_seq(seq, size);
            if (present)
                std::cerr << " removed\n";
            else
                std::cerr << " was not present\n";
        }
    }

    // Funkcja wypisuje wstępne informacje diagnostyczne dla funkcji hash_clear().
    //  id - identyfikator czyszczonej tablicy
    void dbg_pre_hash_clear(unsigned long id) {
    	std::cerr << "hash_clear(" << id << ")\n";
    }

    // Funkcja wypisuje finalne informacje diagnostyczne dla funkcji hash_clear().
    //  id      - identyfikator czyszczonej tablicy
    //  exists  - informacja mówiąca czy tablica o danym id istnieje
    //  empty   - informacja mówiąca czy dana tablica była wcześniej pusta
    void dbg_final_hash_clear(unsigned long id, bool exists, bool empty) {
        std::cerr << "hash_clear: hash table #" << id;
        if (!exists)
			std::cerr << " does not exist\n";
        else if (empty)
            std::cerr << " was empty\n";
        else
            std::cerr << " cleared\n";
    }

    // Funkcja wypisuje wstępne informacje diagnostyczne dla funkcji hash_test().
    //  id      - identyfikator sprawdzanej tablicy
    //  seq     - ciąg wartości typu uint64_t
    //  size    - dlugość danego ciągu
    void dbg_pre_hash_test(unsigned long id, uint64_t const *seq, uint64_t size) {
    	std::cerr << "hash_test(" << id << ", ";
    	dbg_print_seq(seq, size);
    	std::cerr << ", " << size << ")\n";
    }

    // Funkcja wypisuje finalne informacje diagnostyczne dla funkcji hash_test().
    //  id      - identyfikator sprawdzanej tablicy
    //  seq     - ciąg wartosci typu uint64_t
    //  size    - dlugość danego ciągu
    //  exists  - informacja mówiąca czy tablica o danym id istnieje
    //  present - informacja mówiąca czy dany ciąg należał juz do tablicy
    void dbg_final_hash_test(unsigned long id, uint64_t const *seq, uint64_t size, bool exists, bool present) {
        if (!seq || !size) {
            if (!seq)
                std::cerr << "hash_test: invalid pointer (NULL)\n";
            if (!size)
                std::cerr << "hash_test: invalid size (" << size << ")\n";
        } else if (!exists) {
            std::cerr << "hash_test: hash table #" << id << " does not exist\n";
        } else {
            std::cerr << "hash_test: hash table #" << id << ", sequence ";
            dbg_print_seq(seq, size);
            std::cerr << " is ";
            if (!present)
                std::cerr << "not ";
            std::cerr << "present\n";
        }
    }
}

namespace jnp1 {
    using ::std::set;
    using ::std::pair;
    using ::std::vector;
    using ::std::unordered_map;
    using sequences = set<vector<uint64_t>>;
    using hashTable = pair<hash_function_t, unordered_map<uint64_t, sequences>>;

    static unordered_map<unsigned long, hashTable> &hashTables() {
        static unordered_map<unsigned long, hashTable> hashTables;
        return hashTables;
    }

    static unsigned long &freeId() {
        static unsigned long freeId = 0;
        return freeId;
    }

    // Funkcja sprawdza czy tablica haszujaca o danym id istnieje.
    //  id - identyfikator szukanej tablicy
    static bool hash_id_exists(unsigned long id) {
        return hashTables().find(id) != hashTables().end();
    }

    // Funkcja sprawdza czy tablica o identyfikatorze id zawiera już ciąg seq.
    //  id      - identyfikator sprawdzanej tablicy
    //  seq     - ciąg wartości typu uint64_t
    //  size    - dlugość danego ciągu
    static bool hash_sequence_exists(unsigned long id, uint64_t const *seq, size_t size) {
        if (!hash_id_exists(id) || !seq || !size)
            return false;

        uint64_t hash = hashTables()[id].first(seq, size);
        vector<uint64_t> sequence(seq, seq + size);

        return hashTables()[id].second[hash].find(sequence) != hashTables()[id].second[hash].end();
    }

    unsigned long hash_create(hash_function_t hash_function) {
        if(debug)
			dbg_pre_hash_create((void const *) hash_function);

        hashTables()[freeId()++].first = hash_function;

        if (debug)
			dbg_final_hash_create(freeId() - 1);

        return freeId() - 1;
    }

    void hash_delete(unsigned long id) {
        if(debug)
			dbg_pre_hash_delete(id);

        bool check = hash_id_exists(id);

        if (debug)
            dbg_final_hash_delete(id, check);

        if (!check)
            return;

        hashTables().erase(id);
    }

    size_t hash_size(unsigned long id) {
        if(debug)
        	dbg_pre_hash_size(id);

        size_t size = 0;
        bool check = hash_id_exists(id);

        if (!check) {
            if (debug)
                dbg_final_hash_size(id, size, check);
            return size;
        }

        for (const pair<uint64_t, set<vector<uint64_t>>> sequences: hashTables()[id].second)
            size += sequences.second.size();
        if (debug)
            dbg_final_hash_size(id, size, check);

        return size;
    }

    bool hash_insert(unsigned long id, uint64_t const *seq, size_t size) {
        if(debug)
        	dbg_pre_hash_insert(id, seq, size);

        bool exists = hash_id_exists(id);
        bool test = hash_sequence_exists(id, seq, size);

        if (debug)
          dbg_final_hash_insert(id, seq, size, exists, test);

        if (!seq || !size || !exists || test)
            return false;

        uint64_t hash = hashTables()[id].first(seq, size);
        vector<uint64_t> sequence(seq, seq + size);
        hashTables()[id].second[hash].insert(sequence);

        return true;
    }

    bool hash_remove(unsigned long id, uint64_t const *seq, size_t size) {
      	if(debug)
        	dbg_pre_hash_remove(id, seq, size);

        bool exists = hash_id_exists(id);
        bool test = hash_sequence_exists(id, seq, size);

        if (debug)
            dbg_final_hash_remove(id, seq, size, exists, test);

        if (!seq || !size || !exists || !test)
            return false;

        uint64_t hash = hashTables()[id].first(seq, size);
        vector<uint64_t> sequence(seq, seq + size);
        hashTables()[id].second[hash].erase(sequence);

        if (hashTables()[id].second[hash].empty())
            hashTables()[id].second.erase(hash);

        return true;
    }

    void hash_clear(unsigned long id) {
        if(debug)
        	dbg_pre_hash_clear(id);

        bool exists = hash_id_exists(id);

        if (!exists || hashTables()[id].second.empty()) {
            if (debug)
                dbg_final_hash_clear(id, exists, true);
            return;
        }

        if (debug)
            dbg_final_hash_clear(id, true, false);

        hashTables()[id].second.clear();
    }

    bool hash_test(unsigned long id, uint64_t const *seq, size_t size) {
        if(debug)
        	dbg_pre_hash_test(id, seq, size);

        bool exists = hash_id_exists(id);

        if (!seq || !size || !exists) {
            if (debug)
                dbg_final_hash_test(id, seq, size, exists, true);
            return false;
        }

        bool result = hash_sequence_exists(id, seq, size);
        if (debug)
            dbg_final_hash_test(id, seq, size, exists, result);

        return result;
    }
}
