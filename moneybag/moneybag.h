#ifndef MONEYBAG_H
#define MONEYBAG_H

#include <cstdint>
#include <cstdbool>
#include <string>
#include <iostream>
#include <compare>
#include <stdexcept>
#include <boost/multiprecision/cpp_int.hpp>


class Moneybag {
    public:
        using coin_number_t = uint64_t;
    private:
        coin_number_t livres;
        coin_number_t soliduses;
        coin_number_t deniers;

    public:
        constexpr Moneybag(coin_number_t l, coin_number_t s, coin_number_t d) : livres(l), soliduses(s), deniers(d) {}

        constexpr Moneybag(const Moneybag &bag) = default;

        constexpr coin_number_t livre_number() const {
            return livres;
        }

        constexpr coin_number_t solidus_number() const {
            return soliduses;
        }

        constexpr coin_number_t denier_number() const {
            return deniers;
        }

        constexpr const Moneybag& operator+=(const Moneybag &other) {
            if (livres > UINT64_MAX - other.livres || soliduses > UINT64_MAX - other.soliduses || deniers > UINT64_MAX - other.deniers) {
                throw std::out_of_range("Overflow during addition");
            }
            livres += other.livres;
            soliduses += other.soliduses;
            deniers += other.deniers;

            return *this;
        }

        constexpr const Moneybag operator+(const Moneybag &other) const {
            return Moneybag(*this) += other;
        }

        constexpr const Moneybag& operator-=(const Moneybag &other) {
            if (livres < other.livres || soliduses < other.soliduses || deniers < other.deniers) {
                throw std::out_of_range("Underflow during substraction");
            }
            livres -= other.livres;
            soliduses -= other.soliduses;
            deniers -= other.deniers;

            return *this;
        }

        constexpr const Moneybag operator-(const Moneybag &other) const {
            return Moneybag(*this) -= other;
        }

        constexpr const Moneybag operator*=(const uint64_t lambda) {
            if (livres > UINT64_MAX / lambda || soliduses > UINT64_MAX / lambda || deniers > UINT64_MAX / lambda) {
                throw std::out_of_range("Overflow during multiplication");
            }
            livres *= lambda;
            soliduses *= lambda;
            deniers *= lambda;

            return *this;
        }

        constexpr const Moneybag operator*(const uint64_t lambda) const {
            return Moneybag(*this) *= lambda;
        }

        constexpr explicit operator bool() const {
            return livres || soliduses || deniers;
        }

        constexpr const std::partial_ordering operator<=>(const Moneybag &other) const {
            if (livres == other.livres && soliduses == other.soliduses && deniers == other.deniers) {
                return std::partial_ordering::equivalent;
            }
            if (livres <= other.livres && soliduses <= other.soliduses && deniers <= other.deniers) {
                return std::partial_ordering::less;
            }
            if (livres >= other.livres && soliduses >= other.soliduses && deniers >= other.deniers) {
                return std::partial_ordering::greater;
            }
            return std::partial_ordering::unordered;
        }

        constexpr bool operator==(const Moneybag &other) const {
            return livres == other.livres && soliduses == other.soliduses && deniers == other.deniers;
        }
};

inline const std::string plural(const Moneybag::coin_number_t count, const std::string singular, const std::string plural) {
    if (count == 1) {
        return singular;
    }
    else {
        return plural;
    }
}

inline std::ostream& operator<<(std::ostream& stream, const Moneybag &bag) {
    stream << "(" << bag.livre_number() << " " << plural(bag.livre_number(), "livr", "livres") << ", ";
    stream << bag.solidus_number() << " " << plural(bag.solidus_number(), "solidus", "soliduses") << ", ";
    stream << bag.denier_number() << " " << plural(bag.denier_number(), "denier", "deniers") << ")";
    return stream;
}

constexpr Moneybag operator*(const uint64_t lambda, const Moneybag& bag) {
    return Moneybag(bag) *= lambda;
}

constinit const Moneybag Livre = Moneybag(1, 0, 0);
constinit const Moneybag Solidus = Moneybag(0, 1, 0);
constinit const Moneybag Denier = Moneybag(0, 0, 1);

class Value {
    private:
        boost::multiprecision::uint128_t value;
    public:
        constexpr Value() : value(0) {}

        constexpr Value(const uint64_t val) : value(val) {}

        static constexpr boost::multiprecision::uint128_t sum(const Moneybag& bag) {
            boost::multiprecision::uint128_t sum = bag.denier_number();
            sum += 12 * (boost::multiprecision::uint128_t)bag.solidus_number();
            sum += 240 * (boost::multiprecision::uint128_t)bag.livre_number();
            return sum;
        }

        constexpr Value(const Moneybag& bag) : value(sum(bag)) {}

        constexpr Value(const Value &val) = default;

        constexpr std::strong_ordering operator<=>(const uint64_t number) const{
            if (value > number) {
                return std::strong_ordering::greater;
            }
            else if (value < number) {
                return std::strong_ordering::less;
            }
            else {
                return std::strong_ordering::equal;
            }
        }

        constexpr bool operator==(const uint64_t number) const {
            return value == number;
        }

        constexpr std::strong_ordering operator<=>(const Value& other) const {
            if (value > other.value) {
                return std::strong_ordering::greater;
            }
            else if (value < other.value) {
                return std::strong_ordering::less;
            }
            else {
                return std::strong_ordering::equal;
            }
        }

        constexpr bool operator==(const Value& other) const {
            return value == other.value;
        }

        explicit operator std::string() const {
            return value.str();
        }
};
#endif
