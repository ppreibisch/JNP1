#ifndef IMAGES_H
#define IMAGES_H

#include <functional>
#include <cmath>
#include "functional.h"
#include "coordinate.h"
#include "color.h"

namespace Detail {
    inline Point to_polar_safe(const Point point) {
        return point.is_polar ? point : to_polar(point);
    }

    inline Point from_polar_safe(const Point point) {
        return !point.is_polar ? point : from_polar(point);
    }
}

template<typename T>
using Base_image = std::function<T(const Point)>;

using Fraction = double;
using Region = Base_image<bool>;
using Image = Base_image<Color>;
using Blend = Base_image<Fraction>;

template<typename T>
Base_image<T> constant(T t) {
    return [t](const Point p) {
        (void) p;
        return t;
    };
}

template<typename T>
Base_image<T> rotate(Base_image<T> image, double phi) {
    return compose(
            Detail::to_polar_safe,
            [=](const Point p) {
                return Point(p.first, p.second - phi, true);
            },
            image
    );
}

template<typename T>
Base_image<T> translate(Base_image<T> image, Vector v) {
    return compose(
            Detail::from_polar_safe,
            [=](const Point p) {
                return Point(p.first - v.first, p.second - v.second);
            },
            image
    );
}

template<typename T>
Base_image<T> scale(Base_image<T> image, double s) {
    return compose(
            Detail::from_polar_safe,
            [s](const Point p) {
                return Point(p.first / s, p.second / s, false);
            },
            image
    );
}

template<typename T>
Base_image<T> circle(Point q, double r, T inner, T outer) {
    return compose(
            Detail::from_polar_safe,
            [=](const Point p) {
                return distance(p, Detail::from_polar_safe(q)) <= r ? inner : outer;
            }
    );
}

template<typename T>
Base_image<T> checker(double d, T this_way, T that_way) {
    return compose(
            Detail::from_polar_safe,
            [=](const Point p) {
                return static_cast<int>(floor(p.first / d) + floor(p.second / d)) % 2 ? that_way : this_way;
            }
    );
}

template<typename T>
Base_image<T> polar_checker(double d, int n, T this_way, T that_way) {
    return compose(
            Detail::to_polar_safe,
            [=](const Point p) {
                return Point(p.first, p.second * static_cast<double>(n) * d / (2 * M_PI), false);
            },
            checker(d, this_way, that_way)
    );
}

template<typename T>
Base_image<T> rings(Point q, double d, T this_way, T that_way) {
    return compose(
            Detail::from_polar_safe,
            [=](const Point p) {
                return static_cast<int>(distance(p, Detail::from_polar_safe(q)) / d) % 2 ? that_way : this_way;
            }
    );
}

template<typename T>
Base_image<T> vertical_stripe(double d, T this_way, T that_way) {
    return compose(
            Detail::from_polar_safe,
            [=](const Point p) {
                return abs(p.first) * 2 <= d ? this_way : that_way;
            }
    );
}

inline Image cond(Region region, Image this_way, Image that_way) {
    return compose(
            Detail::from_polar_safe,
            [=](Point p) {
                return region(p) ? this_way(p) : that_way(p);
            }
    );
}

inline Image lerp(Blend blend, Image this_way, Image that_way) {
    return lift(
            [](double blend, Color this_way, Color that_way) {
                return this_way.weighted_mean(that_way, blend);
            },
            blend, this_way, that_way);
}

inline Image darken(Image image, Blend blend) {
    return lerp(blend, image, constant(Colors::black));
}

inline Image lighten(Image image, Blend blend) {
    return lerp(blend, image, constant(Colors::white));
}

#endif
