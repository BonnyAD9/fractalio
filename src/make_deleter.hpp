#pragma once

#define MAKE_DELETER(name, fun)                                               \
    namespace del {                                                           \
    struct name {                                                             \
        void operator()(::name *ptr) {                                        \
            fun(ptr);                                                         \
        }                                                                     \
    };                                                                        \
    }