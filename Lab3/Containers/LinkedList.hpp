#pragma once

#include <iterator>
#include <memory>
#include <List.hpp>

namespace list_internal {

struct BaseListNode {
  BaseListNode *prev_;
  BaseListNode *next_;

  BaseListNode() = default;
  BaseListNode(BaseListNode* prev, BaseListNode* next) : prev_(prev), next_(next) {}; 
  BaseListNode(const BaseListNode &) = delete;

  void HookBefore(BaseListNode *other) noexcept {
    next_ = other;
    prev_ = other->prev_;
    prev_->next_ = this;
    other->prev_ = this;
  }
  void Unhook() noexcept {
    prev_->next_ = next_;
    next_->prev_ = prev_;
    prev_ = this;
    next_ = this;
  }
};

template <typename T>
struct ListNode : public BaseListNode {
  T value_;
};

template <typename T, bool isConst>
class ListIterator {
 public:
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;
  using reference =
      typename std::conditional_t<isConst, const value_type &, value_type &>;
  using pointer =
      typename std::conditional_t<isConst, const value_type *, value_type *>;
  using base_node_pointer =
      typename std::conditional_t<isConst, const BaseListNode *,
                                  BaseListNode *>;
  using node_pointer =
      typename std::conditional_t<isConst, const ListNode<value_type> *,
                                  ListNode<value_type> *>;

  base_node_pointer node_;

  ListIterator(base_node_pointer node) : node_(node) {}

  ListIterator(const ListIterator &other) : node_(other.node_) {}

  // Non const to const
  template <
      bool otherIsConst,
      std::enable_if_t<isConst == true && otherIsConst == false, bool> = true>
  ListIterator(const ListIterator<T, otherIsConst> &other)
      : node_(other.node_) {}

  ListIterator<T, isConst> &operator++() {
    node_ = node_->next_;
    return *this;
  }
  ListIterator<T, isConst> operator++(int) {
    ListIterator<T, isConst> temp(*this);
    ++(*this);
    return temp;
  }
  ListIterator<T, isConst> &operator--() {
    node_ = node_->prev_;
    return *this;
  }
  ListIterator<T, isConst> operator--(int) {
    ListIterator<T, isConst> temp(*this);
    --(*this);
    return temp;
  }
  ListIterator &operator=(const ListIterator &other) {
    node_ = other.node_;
    return *this;
  }
  reference operator*() const {
    return static_cast<node_pointer>(node_)->value_;
  }
  pointer operator->() const {
    return &static_cast<node_pointer>(node_)->value_;
  }
  base_node_pointer GetNode() const noexcept { return node_; }
  bool operator==(const ListIterator &other) const {
    return node_ == other.node_;
  }
  bool operator!=(const ListIterator &other) const { return !(*this == other); }
};

}  // namespace list_internal

template <typename T, typename Allocator = std::allocator<T>>
class LinkedList : public List<T> {
 public:
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = list_internal::ListIterator<value_type, false>;
  using const_iterator = list_internal::ListIterator<value_type, true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using size_type = std::size_t;
  using base_node = list_internal::BaseListNode;
  using node = list_internal::ListNode<value_type>;
  using value_traits = std::allocator_traits<Allocator>;
  using node_alloc = typename value_traits::template rebind_alloc<node>;
  using node_traits = typename value_traits::template rebind_traits<node>;

  // Constructors

  LinkedList()
      : node_alloc_(node_alloc()),
        val_alloc_(Allocator()),
        size_(0),
        fakeNode_(&fakeNode_, &fakeNode_) {}

  LinkedList(const LinkedList &other)
      : node_alloc_(other.node_alloc_),
        val_alloc_(other.val_alloc_),
        size_(0),
        fakeNode_(&fakeNode_, &fakeNode_) {
    try {
      for (auto &element : other) {
        push_back(element);
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  ~LinkedList() { clear(); }

  // Iterators
  iterator begin() { return fakeNode_.next_; }
  iterator end() { return &fakeNode_; }
  const_iterator begin() const { return fakeNode_.next_; }
  const_iterator end() const { return &fakeNode_; }
  
  // Reverse iterators
  reverse_iterator rbegin() { return reverse_iterator(&fakeNode_); }
  reverse_iterator rend() { return reverse_iterator(fakeNode_.next_); }

  // Capacity
  size_type size() const { return size_; }

  // Modifiers
  iterator insert(const_iterator pos, const_reference value) {
    InsertNodeBefore(IteratorConstCast(pos), value);
    return --IteratorConstCast(pos);
  }
  iterator insert(const_iterator pos, value_type &&value) {
    InsertNodeBefore(IteratorConstCast(pos), std::move(value));
    return --IteratorConstCast(pos);
  }
  iterator erase(const_iterator pos) {
    return erase(pos, const_iterator(pos.GetNode()->next_));
  }
  iterator erase(const_iterator first, const_iterator last) {
    while (first != last) {
      EraseNode(IteratorConstCast(first++));
    }
    return IteratorConstCast(last);
  }

  void push_back(const_reference value) { InsertNodeBefore(end(), value); }

  void push_back(value_type &&value) {
    InsertNodeBefore(end(), std::move(value));
  }

  void pop_front() { EraseNode(begin()); }

  void clear() { erase(begin(), end()); }

  // слой совместимости с родительским классом
  void insert(int pos, T value) {
    const_iterator toInsert = std::next(begin(), pos);
    insert(toInsert, value);
  }
  void erase(int pos) {
    const_iterator toRemove = std::next(begin(), pos);
    erase(toRemove, const_iterator(toRemove.GetNode()->next_));
  }
  int size() { return size_; }

 private:
  using node_pointer = node *;
  using base_node_pointer = base_node *;

  node_alloc node_alloc_;
  Allocator val_alloc_;
  size_type size_;
  base_node fakeNode_;

  // Creates List Node with value_type element inside.
  // Receives universal reference parameter pack.
  // It can be lvalue value_type, rvalue value_type, or pack of arguments to
  // value_type constructor.
  template <typename... Args>
  node_pointer CreateNode(Args &&...value) {
    node_pointer temp = node_traits::allocate(node_alloc_, 1);
    try {
      value_traits::construct(val_alloc_, &(temp->value_),
                              std::forward<Args>(value)...);
    } catch (...) {
      node_traits::deallocate(node_alloc_, temp, 1);
      throw;
    }
    return temp;
  }
  // Remove node on pos from list.
  // Destroy value_type field and deallocate node
  void EraseNode(iterator pos) {
    node_pointer node = static_cast<node_pointer>(pos.GetNode());
    node->Unhook();
    value_traits::destroy(val_alloc_, &(node->value_));
    node_traits::deallocate(node_alloc_, node, 1);
    --size_;
  }
  // Create and add node in the list before another node
  // Receives the iterator on element before which the new one will be created
  // and parameter pack for CreateNode
  template <typename... Args>
  void InsertNodeBefore(iterator pos, Args &&...value) {
    node_pointer new_node = CreateNode(std::forward<Args>(value)...);
    base_node_pointer node_from_pos = pos.GetNode();
    new_node->HookBefore(node_from_pos);
    ++size_;
  }

  // Const cast from const iterator to non-const iterator
  iterator IteratorConstCast(const_iterator pos) const {
    iterator temp(
        const_cast<typename iterator::base_node_pointer>(pos.GetNode()));
    return temp;
  }
};