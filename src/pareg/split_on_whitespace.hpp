#include <algorithm>
#include <cassert>
#include <cctype>
#include <optional>
#include <string_view>

namespace fio::pareg {

class SplitWhitespaceIterator {
public:
    SplitWhitespaceIterator() = default;

    SplitWhitespaceIterator(std::string_view data) : _data(data) { ++*this; }

    bool operator==(const SplitWhitespaceIterator &other) const {
        return (_data.empty() && !_cur && other._data.empty() &&
                !other._cur) ||
               _data.begin() == other._data.begin();
    }

    SplitWhitespaceIterator &operator++() {
        if (_data.empty()) {
            _cur = std::nullopt;
            return *this;
        }
        auto foo = [=](char c) -> bool { return std::isspace(c); };
        auto spos = std::ranges::find_if(_data, foo);
        _cur = std::string_view(_data.begin(), spos);
        auto pos = std::find_if_not(spos, _data.end(), foo);
        _data = { pos, _data.end() };
        return *this;
    }

    std::string_view operator*() const {
        assert(_cur);
        return *_cur;
    }

private:
    std::string_view _data;
    std::optional<std::string_view> _cur;
};

class SplitWhitespace {
public:
    SplitWhitespace(std::string_view data) : _data(data) { }

    [[nodiscard]]
    SplitWhitespaceIterator begin() const {
        return { _data };
    }

    [[nodiscard]]
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    SplitWhitespaceIterator end() const {
        return {};
    }

private:
    std::string_view _data;
};

} // namespace fio::pareg