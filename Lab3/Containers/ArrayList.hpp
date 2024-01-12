#pragma once

#include <iterator>
#include <memory>
#include <List.hpp>

template <typename T, typename Allocator = std::allocator<T>>
class ArrayList : public List<T> {
 public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using pointer = T *;
  using const_pointer = const T *;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using size_type = std::size_t;
  using alloc_traits = std::allocator_traits<Allocator>;

  ArrayList() : data_(nullptr), capacity_(0), size_(0), alloc_(Allocator()) {}

  ~ArrayList() { freeDataArray(data_, capacity_, size_); }

  void push_back(T &&value) {
    AutomaticReserveLogic();
    CreateElement(&data_[size_], std::move(value));
    ++size_;
  }

  iterator insert(const_iterator pos, const_reference value) {
    return InsertElement(pos, value);
  }

  void erase(iterator pos) {
    size_type before_erase_count =
        static_cast<size_type>(std::distance(begin(), pos));
    size_type after_erase_count =
        static_cast<size_type>(std::distance(pos, end()));
    pointer new_data = alloc_traits::allocate(alloc_, capacity_);

    ConstructElementsFromAnotherData(before_erase_count, data_, new_data);
    ConstructElementsFromAnotherData(after_erase_count - 1,
                                     data_ + before_erase_count + 1,
                                     new_data + before_erase_count);
    freeDataArray(data_, capacity_, size_);
    data_ = new_data;
    --size_;
  }

  // ELEMENT ACCESS
  reference at(size_type pos) {
    if (!(pos < size_)) {
      throw std::out_of_range(std::to_string(pos) + "not less then" +
                              std::to_string(size_));
    }
    return data_[pos];
  }
  const_reference front() const { return data_[0]; }

  // ITERATORS
  iterator begin() { return data_; }
  const_iterator begin() const { return data_; };
  const_iterator cbegin() { return data_; };
  iterator end() { return data_ + size_; }
  const_iterator end() const { return data_ + size_; };
  const_iterator cend() { return data_ + size_; };

  // CAPACITY
  size_type size() const { return size_; }

  void reserve(size_type new_capacity) {
    if (new_capacity > capacity_) {
      pointer new_data = MoveToNewDataArray(new_capacity, size_, data_);
      freeDataArray(data_, capacity_, size_);
      data_ = new_data;
      capacity_ = new_capacity;
    }
  }

  void shrink_to_fit() {
    pointer new_data = MoveToNewDataArray(size_, size_, data_);
    freeDataArray(data_, capacity_, size_);
    data_ = new_data;
    capacity_ = size_;
  };

  void swap(ArrayList &other) {
    std::swap(data_, other.data_);
    std::swap(capacity_, other.capacity_);
    std::swap(alloc_, other.alloc_);
    std::swap(size_, other.size_);
  }

  // слой совместимости с родительским классом
  void insert(int pos, T value) {
    const_iterator toInsert = std::next(begin(), pos);
    insert(toInsert, value);
  }
  void erase(int pos) {
    iterator toRemove = std::next(begin(), pos);
    erase(toRemove);
  }
  int size() { return size_; }

 private:
  pointer data_;
  size_type capacity_;
  size_type size_;
  Allocator alloc_;

  template <typename... Args>
  void CreateElement(pointer pos, Args &&...value) {
    alloc_traits::construct(alloc_, pos, std::forward<Args>(value)...);
  }

  // if new_capacity < element_recreating_count -- UB
  pointer MoveToNewDataArray(size_type new_capacity,
                             size_type element_recreating_count,
                             pointer data_for_moving) {
    pointer new_data = alloc_traits::allocate(alloc_, new_capacity);
    try {
      ConstructElementsFromAnotherData(element_recreating_count,
                                       data_for_moving, new_data);
    } catch (...) {
      alloc_traits::deallocate(alloc_, new_data, new_capacity);
      throw;
    }
    return new_data;
  }

  void ConstructElementsFromAnotherData(size_type count, pointer data_from,
                                        pointer data_to) {
    for (size_type i = 0; i < count; ++i) {
      try {
        alloc_traits::construct(alloc_, data_to + i, data_from[i]);
      } catch (...) {
        for (size_type j = 0; j < i; ++j) {
          alloc_traits::destroy(alloc_, data_to + j);
        }
        throw;
      }
    }
  }

  void freeDataArray(pointer data_array, size_type capacity, size_type size) {
    if (data_array != nullptr) {
      for (size_type i = 0; i < size; ++i) {
        alloc_traits::destroy(alloc_, data_array + i);
      }
      alloc_traits::deallocate(alloc_, data_array, capacity);
    }
  }

  void AutomaticReserveLogic() {
    if (size_ >= capacity_) {
      if (capacity_ == 0) {
        reserve(1);
      } else {
        reserve(capacity_ * 2);
      }
    }
  }

  template <typename U>
  iterator InsertElement(const_iterator pos, U &&value) {
    size_type before_insert_count =
        static_cast<size_type>(std::distance(cbegin(), pos));
    size_type after_insert_count =
        static_cast<size_type>(std::distance(pos, cend()));
    AutomaticReserveLogic();
    pointer new_data = alloc_traits::allocate(alloc_, capacity_);
    ConstructElementsFromAnotherData(before_insert_count, data_, new_data);
    ConstructElementsFromAnotherData(after_insert_count,
                                     data_ + before_insert_count,
                                     new_data + before_insert_count + 1);
    CreateElement(new_data + before_insert_count, std::forward<U>(value));
    freeDataArray(data_, capacity_, size_);
    data_ = new_data;
    ++size_;
    return data_ + before_insert_count;
  }
};