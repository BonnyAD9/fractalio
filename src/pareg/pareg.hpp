#pragma once

#include <stdexcept>

#include "parsable.hpp"

namespace mdt::pareg {

template<typename I, typename T>
concept Iterator = requires(I i) {
    T(*i);
    ++i;
    I(std::move(i));
};

template<typename T, typename I>
concept IntoIter = requires(T ii) {
    { ii.begin() } -> std::same_as<I>;
    { ii.end() } -> std::same_as<I>;
};

template<Iterator<std::string_view> I> class Pareg {
public:
    template<IntoIter<I> T> Pareg(T &ii) : Pareg(ii.begin(), ii.end()) { }

    Pareg(I begin, I end) : _cur(std::move(begin)), _end(std::move(end)) { }

    template<typename T, typename... Args>
        requires Parsable<T, Args...>
    T next_arg(Args... args) {
        if (_cur == _end) {
            throw std::runtime_error("Expected next argument.");
        }
        ++_cur;
        if (_cur == _end) {
            throw std::runtime_error("Expected next argument.");
        }
        return from_arg<T, Args...>(
            std::string_view(*_cur), std::forward<Args>(args)...
        );
    }

    template<typename T, typename B, typename... Args>
        requires Parsable<T, Args...>
    T next_arg(Args... args) {
        if (_cur == _end) {
            throw std::runtime_error("Expected next argument.");
        }
        ++_cur;
        if (_cur == _end) {
            throw std::runtime_error("Expected next argument.");
        }
        return from_arg<T, Args...>(
            std::string_view(*_cur), std::forward<Args>(args)...
        );
    }

    template<typename T, typename... Args>
        requires Parsable<T, Args...>
    T cur(Args... args) {
        if (_cur == _end) {
            throw std::runtime_error("Expected next argument.");
        }
        return from_arg<T, Args...>(
            std::string_view(*_cur), std::forward<Args>(args)...
        );
    }

    Pareg &operator++() {
        ++_cur;
        return *this;
    }

    std::string_view operator*() const { return std::string_view(*_cur); }

    [[nodiscard]]
    bool empty() const {
        auto c2 = _cur;
        return ++c2 == _end;
    }

    [[nodiscard]]
    std::size_t next_rem() const {
        auto c2 = _cur;
        return _end - ++c2;
    }

private:
    I _cur;
    I _end;
};

} // namespace mdt::pareg