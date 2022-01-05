#pragma once

#include <vector>
#include <Tracy.hpp>

template <typename T, unsigned ... Dims> struct MDArray;

template <typename T, unsigned PrimaryD>
struct MDArray <T, PrimaryD> {
    MDArray() {
        data.resize(PrimaryD);
    }

    typedef std::vector<T> type;
    type data;
    T& operator[](unsigned index) {
        ZoneScopedN("Running [] operator on 1D Array")
        return data[index];
    }
};

template <typename T, unsigned PrimaryD, unsigned ... RestD>
struct MDArray<T, PrimaryD, RestD...> {
    MDArray() {
        LesserDimArrayT base{};
        data.resize(PrimaryD, base);
    }

    typedef MDArray<T, RestD...> LesserDimArrayT;
    typedef std::vector<LesserDimArrayT> type;
    type data;
    LesserDimArrayT& operator[](unsigned index) {
        ZoneScopedN("Running [] operator on MD Array")
        return data[index];
    }
};