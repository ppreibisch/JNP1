#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <regex>
#include <set>

#define lastPlace 7
using namespace std;
using pairOfUint_t = pair<uint32_t, uint64_t>;

struct CustomCompare {
    bool operator()(const pairOfUint_t &a, const pairOfUint_t &b) const {
        if (a.second == b.second)
            return a.first < b.first;
        return a.second > b.second;
    }
};

void printError(size_t lineNumber, string &line) {
    line.pop_back();
    cerr << "Error in line " << lineNumber << ": " << line << '\n';
}

void printTop7Notes(const vector <pairOfUint_t> &newTop7Notes, vector <pairOfUint_t> &prevTop7Notes) {
    bool find;
    int8_t actPosition = 1, prevPosition;
    for (pairOfUint_t act: newTop7Notes) {
        find = false;
        prevPosition = 1;
        for (pairOfUint_t prev: prevTop7Notes) {
            if (prev.first == act.first) {
                find = true;
                break;
            }
            ++prevPosition;
        }
        if (!find)
            cout << act.first << " -" << '\n';
        else
            cout << act.first << " " << prevPosition - actPosition << '\n';
        ++actPosition;
    }

    prevTop7Notes = newTop7Notes;
}

void printTop7Points(const set <pairOfUint_t, CustomCompare> &actTop7,
                     set <pairOfUint_t, CustomCompare> &prevTop7) {
    // Zmieniamy set na vector ze względu, gdyż mamy wypisywanie miejsc dla vectorów (oszczędza to powtórzenie kodu),
    // a zmiana jest w koszcie stałym, gdyż rozmiar seta jest <= 7.
    vector <pairOfUint_t> vectorActTop7(actTop7.begin(), actTop7.end());
    vector <pairOfUint_t> vectorPrevTop7(prevTop7.begin(), prevTop7.end());
    printTop7Notes(vectorActTop7, vectorPrevTop7);
    prevTop7 = actTop7;
}

vector <pairOfUint_t> top7(unordered_map <uint32_t, uint64_t> &votes) {
    set <pairOfUint_t, CustomCompare> top7;
    for (pairOfUint_t element: votes) {
        if (element.second != 0)
            top7.insert(make_pair(element.first, element.second));
        if (top7.size() > lastPlace) {
            set<pairOfUint_t>::iterator it;
            it = prev(top7.end());
            top7.erase(it);
        }
    }

    uint8_t points = 7;
    vector <pairOfUint_t> res;
    for (pairOfUint_t element: top7) {
        res.emplace_back(element.first, points);
        --points;
    }
    return res;
}

// Sprawdzamy poprawność głosów, czy piosenka jest w zakresie, czy nie ma powtórek oraz,
// czy nie wyleciała w poprzednich podsumowaniach.
bool correctVotes(vector <uint32_t> &songs, unordered_set <uint32_t> &illegalSongs, uint32_t maxNumber) {
    sort(songs.begin(), songs.end());
    for (size_t i = 0; i < songs.size(); ++i) {
        if ((songs[i] > maxNumber || illegalSongs.find(songs[i]) != illegalSongs.end())
            || (i >= 1 && songs[i] == songs[i - 1]))
            return false;
    }
    return true;
}

// Sprawdzamy, czy piosenka, która była w poprzednim notowaniu jest w aktualnym, jeśli tak dodajemy jej punkty,
// jeśli nie, to oznaczamy ją jako piosenkę, na którą nie można już głosować.
void addIllegalSongsAndUpdatePoints(unordered_set <uint32_t> &illegalSongs, vector <pairOfUint_t> &actTop7Notes,
                                    const vector <pairOfUint_t> &prevTop7Notes) {
    bool find;
    for (pairOfUint_t prevTop7Note: prevTop7Notes) {
        uint32_t song = prevTop7Note.first;
        find = false;
        uint8_t index;
        for (uint8_t j = 0; j < actTop7Notes.size(); ++j) {
            if (song == actTop7Notes[j].first) {
                find = true;
                index = j;
                break;
            }
        }
        if (!find)
            illegalSongs.insert(song);
        else
            actTop7Notes[index].second += prevTop7Note.second;
    }
}

void updateTop7Points(const vector <pairOfUint_t> &actTop7Notes, set <pairOfUint_t, CustomCompare> &actTop7Points) {
    set<pairOfUint_t, CustomCompare>::iterator it;
    bool find;
    for (pairOfUint_t actTop7Note: actTop7Notes) {
        find = false;
        for (pairOfUint_t element: actTop7Points) {
            if (actTop7Note.first == element.first) {
                find = true;
                it = actTop7Points.find(element);
                break;
            }
        }
        if (!find) {
            actTop7Points.insert(actTop7Note);
            if (actTop7Points.size() > lastPlace)
                actTop7Points.erase(prev(actTop7Points.end()));
        } else {
            actTop7Points.erase(it);
            actTop7Points.insert(actTop7Note);
        }
    }
}

int main() {
    regex validVotes("^\\s*(0*[1-9][0-9]{0,7}\\s+)*$");
    regex validTop("^\\s*TOP\\s*");
    regex validNew(R"(^\s*NEW\s+0*[1-9][0-9]{0,7}\s*$)");

    string line;
    size_t lineNumber = 0;
    uint32_t songNumber;
    uint32_t maxNumber = 0;
    unordered_set <uint32_t> illegalSongs; // Zbiór utworów, na które nie można głosować.
    set <pairOfUint_t, CustomCompare> prevTop7Points, actTop7Points; // <a,b> -> a - id utworu, b - liczba punktów danego utworu.
    vector <pairOfUint_t> prevTop7Notes; // <a,b> -> a - id utworu, b - liczba punktów; utwory w kolejności notowań.
    unordered_map <uint32_t, uint64_t> votesNumber; // <a,b> -> a - id utworu, b - liczba głosów na dany numer.

    while (getline(cin, line)) {
        line += ' '; // Zapewnia poprawne działanie regex_match().
        ++lineNumber;
        if (regex_match(line, validVotes)) {
            stringstream reader(line);

            vector <uint32_t> votes;
            while (reader >> songNumber)
                votes.push_back(songNumber);

            if (!correctVotes(votes, illegalSongs, maxNumber)) {
                printError(lineNumber, line);
                continue;
            }

            for (uint32_t song: votes)
                ++votesNumber[song];

        } else if (regex_match(line, validTop)) {
            printTop7Points(actTop7Points, prevTop7Points);
        } else if (regex_match(line, validNew)) {
            stringstream reader(line);
            string s;
            uint32_t val;

            reader >> s >> val;

            if (val < maxNumber) {
                printError(lineNumber, line);
                continue;
            }
            maxNumber = val;

            vector <pairOfUint_t> actTop7Notes = top7(votesNumber);

            addIllegalSongsAndUpdatePoints(illegalSongs, actTop7Notes, prevTop7Notes);
            updateTop7Points(actTop7Notes, actTop7Points);
            printTop7Notes(actTop7Notes, prevTop7Notes);

            votesNumber.clear();
        } else {
            printError(lineNumber, line);
        }
    }
    return 0;
}
