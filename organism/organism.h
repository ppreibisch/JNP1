#ifndef ORGANISM_H
#define ORGANISM_H

#include <cstdint>
#include <concepts>
#include <optional>
#include <numeric>
#include <tuple>

template<typename species_t, bool can_eat_meat, bool can_eat_plants> requires std::equality_comparable<species_t>
class Organism {
    private:
        const species_t species;
        const uint64_t vitality;
    public:
        constexpr Organism(species_t const &species, uint64_t vitality) : species(species), vitality(vitality) {}

        constexpr uint64_t get_vitality() const {
            return vitality;
        }

        constexpr const species_t &get_species() const {
            return species;
        }

        constexpr bool is_dead() const {
            return vitality == 0;
        }

        constexpr bool is_plant() const {
            return !can_eat_meat && !can_eat_plants;
        }

        template<bool can_eat_meat2, bool can_eat_plants2>
        constexpr Organism eats(Organism<species_t, can_eat_meat2, can_eat_plants2> organism1) {
            uint64_t newVitality = vitality;

            if (can_eat_plants && organism1.is_plant()) // roślinożerca zjada roślinę
                newVitality += organism1.get_vitality();
            else if (is_plant() && can_eat_plants2) // roślinożerca zjada roślinę
                newVitality = 0;
            else if constexpr (can_eat_meat && can_eat_meat2) { // oba nawzajem mogą siebie zjeść
                if (vitality > organism1.get_vitality())
                    newVitality += organism1.get_vitality() / 2;
                else
                    newVitality = 0;
            }
            else if (can_eat_meat && !organism1.is_plant()) { // pierwszy może zjeść drugiego a drugi nie może pierwszego
                if (vitality > organism1.get_vitality())
                    newVitality += organism1.get_vitality() / 2;
            }
            else if (can_eat_meat2 && !is_plant()) { // drugi może zjeść pierwszego a pierwszy nie może drugiego
                if (organism1.get_vitality() > vitality)
                    newVitality = 0;
            }
            return {species, newVitality};
        }
};

template<typename species_t>
using Carnivore = Organism<species_t, true, false>;

template<typename species_t>
using Omnivore = Organism<species_t, true, true>;

template<typename species_t>
using Herbivore = Organism<species_t, false, true>;

template<typename species_t>
using Plant = Organism<species_t, false, false>;

template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
        Organism<species_t, sp2_eats_m, sp2_eats_p>,
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
encounter(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
          Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {
    static_assert(!(organism1.is_plant() && organism2.is_plant()), "rośliny nie mogą się spotkać");

    if (organism1.is_dead() || organism2.is_dead())
        return {organism1, organism2, std::nullopt};

    if (organism1.get_species() == organism2.get_species() && sp1_eats_m == sp2_eats_m && sp1_eats_p == sp2_eats_p) {
        Organism<species_t, sp1_eats_m, sp1_eats_p> child(organism1.get_species(), 
                std::midpoint(std::min(organism1.get_vitality(), organism2.get_vitality()), std::max(organism1.get_vitality(), organism2.get_vitality())));
        return {organism1, organism2, child};
    }

    return {organism1.eats(organism2), organism2.eats(organism1), std::nullopt};
}

namespace operators {
    template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
    constexpr auto operator+(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
                             Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {
        return get<0>(encounter(organism1, organism2));
    }
}

template<typename species_t, bool sp1_eats_m, bool sp1_eats_p, typename ... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1, Args ... args) {
    using namespace operators;
    return (organism1 + ... + args);
}

#endif
