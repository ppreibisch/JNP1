#ifndef HASH_H
#define HASH_H

#ifdef __cplusplus
#include <cstdint>
#include <cstdio>
#include <iostream>
namespace jnp1 {
    extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#endif
    typedef uint64_t (*hash_function_t)(const uint64_t *, size_t);


    // Funkcja tworzy tablicę haszującą i zwraca jej identyfikator.
    //  hash_function - wskaźnik na funkcję haszującą, która daje w wyniku liczbę typu
    //    		uint64_t i ma kolejno parametry uint64_t const * oraz size_t.
    unsigned long hash_create(hash_function_t hash_function);

    // Funkcja usuwa tablicę haszującą o identyfikatorze id, o ile ona istnieje.
    // W przeciwnym przypadku nic nie robi.
    //  id - identyfikator usuwanej tablicy
    void hash_delete(unsigned long id);

    // Funkcja daje liczbę ciągów przechowywanych w tablicy haszującej
    // o identyfikatorze id lub 0, jeśli taka tablica nie istnieje.
    //  id - identyfikator sprawdzanej tablicy
    size_t hash_size(unsigned long id);

    // Funkcja wstawia do tablicy haszującej o identyfikatorze id ciąg liczb
    // całkowitych seq o długości size.
    // Wynikiem jest informacja, czy operacja się powiodła.
    // Operacja się nie powiedzie, jeśli nie ma takiej tablicy
    // haszującej, jeśli tablica haszująca zawiera już taki ciąg, jeśli
    // parametr seq ma wartość NULL lub parametr size ma wartość 0.
    //  id      - identyfikator tablicy do ktorej wstawiany jest ciag
    //  seq     - wstawiany ciąg wartości typu uint64_t
    //  size    - długość danego ciągu
    bool hash_insert(unsigned long id, uint64_t const *seq, size_t size);

    // Funkcja usuwa z tablicy haszującej o identyfikatorze id ciąg liczb
    // całkowitych seq o długości size.
    // Wynikiem jest informacja, czy operacja się powiodła.
    // Operacja się nie powiedzie, jeśli nie ma takiej tablicy haszujacej,
    // jeśli tablica haszująca nie zawiera takiego ciągu,
    // jeśli parametr seq ma wartość NULL lub parametr size ma wartość 0.
    //  id      - identyfikator tablicy z której usuwany jest ciąg
    //  seq     - usuwany ciąg wartości typu uint64_t
    //  size    - długość danego ciągu
    bool hash_remove(unsigned long id, uint64_t const *seq, size_t size);

    // Funkcja usuwa z tablicy wszystkie jej elemnty, jeśli tablica haszująca
    // o identyfikatorze id istnieje i nie jest pusta.
    // W przeciwnym przypadku nic nie robi.
    //  id - identyfikator usuwanej tablicy
    void hash_clear(unsigned long id);


    // Funkcja daje wynik true, jeśli istnieje tablica haszująca o identyfikatorze id
    // i zawiera ona ciąg liczb całkowitych seq o długości size.
    // Daje wynik false w przeciwnym przypadku oraz gdy parametr seq ma wartość NULL
    // lub parametr size ma wartość 0.
    //  id      - identyfikator tablicy gdzie sprawdzana jest obecność ciągu
    //  seq     - sprawdzany ciąg wartości typu uint64_t
    //  size    - długość danego ciągu
    bool hash_test(unsigned long id, uint64_t const *seq, size_t size);

#ifdef __cplusplus
    }
}
#endif

#endif
