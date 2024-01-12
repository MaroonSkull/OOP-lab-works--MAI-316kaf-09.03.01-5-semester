#pragma once

template<typename T>
class List {
    virtual void push_back(T&& value) = 0;
    virtual void insert(int pos, T value) = 0;
    virtual void erase(int pos) = 0;
    virtual int size() = 0;
};