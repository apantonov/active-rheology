#ifndef __HELPER_HPP__
#define __HELPER_HPP__

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

#endif // __HELPER_HPP__