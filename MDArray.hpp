#pragma once

#include <memory>
#include <Tracy.hpp>

template <typename T, unsigned ... Dims> struct MDArray;

template <typename T, unsigned PrimaryD>
struct MDArray <T, PrimaryD> {
    MDArray() {
        data = std::make_unique<T[]>(PrimaryD);
    }

    typedef std::unique_ptr<T[]> type;
    type data;
    T& operator[](unsigned index) {
        return data[index];
    }
};

template <typename T, unsigned PrimaryD, unsigned ... RestD>
struct MDArray<T, PrimaryD, RestD...> {
    MDArray() {
        data = std::make_unique<LesserDimArrayT[]>(PrimaryD);
    }

    typedef MDArray<T, RestD...> LesserDimArrayT;
    typedef std::unique_ptr<LesserDimArrayT[]> type;
    type data;

    LesserDimArrayT& operator[](unsigned index) {
        return data[index];
    }
};