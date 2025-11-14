#pragma once

#include "parsable.hpp"

namespace mdt::pareg {

template<typename I, typename T>
concept Iterator = requires(I i) {
    T(*i);
    ++i;
    I(std::move(i));
};
    
template<Iterator<std::string_view> I>
class Pareg {
public:
    Pareg(I iter) : _iter(std::move(iter)) {}
    
    template<typename T, typename... Args> requires Parsable<T, Args...>
    T next_arg() {
        // TODO
    }
    
private:
    I _iter;
};
    
}