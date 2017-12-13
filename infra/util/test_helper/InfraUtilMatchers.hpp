#ifndef INFRA_UTIL_MATCHERS_HPP
#define INFRA_UTIL_MATCHERS_HPP

#include "gmock/gmock.h"
#include "infra/util/BoundedString.hpp"

namespace testing
{
    template<class T>
    class Matcher<infra::BoundedStringBase<T>>
        : public internal::MatcherBase<infra::BoundedStringBase<T>>
    {
    public:
        Matcher() = default;

        explicit Matcher(const MatcherInterface<infra::BoundedStringBase<T>>* impl)
            : internal::MatcherBase<infra::BoundedStringBase<T>>(impl)
        {}

        Matcher(infra::BoundedStringBase<T> s) { *this = Eq(std::string(s.data(), s.size())); }
        Matcher(const char* s) { *this = Eq(std::string(s)); }
    };

    template<class T>
    class Matcher<infra::Optional<T>>
        : public internal::MatcherBase<infra::Optional<T>>
    {
    public:
        Matcher() = default;

        explicit Matcher(const MatcherInterface<infra::Optional<T>>* impl)
            : internal::MatcherBase<infra::Optional<T>>(impl)
        {}

        Matcher(infra::Optional<T> o) { *this = Eq(o); }
    };
}

#endif
