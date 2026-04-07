#pragma once

#include <cstddef>
#include <iterator>

class ListHook {
public:
    ListHook() = default;

    bool IsLinked() const {
        return next_ != nullptr;
    }

    void Unlink() {
        if (!IsLinked()) {
            return;
        }
        next_->prev_ = prev_;
        prev_->next_ = next_;
        prev_ = nullptr;
        next_ = nullptr;
    }

    ~ListHook() {
        Unlink();
    }

    ListHook(const ListHook&) = delete;

private:
    template<typename T>
    friend class List;

    ListHook* prev_ = nullptr;
    ListHook* next_ = nullptr;

    void LinkBefore(ListHook* other) {
        prev_ = other->prev_;
        next_ = other;
        other->prev_->next_ = this;
        other->prev_ = this;
    }
};

template <typename T>
class List {
    static_assert(std::is_base_of_v<ListHook, T>);

private:

    template<bool IsConst>
    class base_iterator {
    public:
        using pointer_type = std::conditional_t<IsConst, const T*, T*>;
        using reference_type = std::conditional_t<IsConst, const T&, T&>;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

    private:
        using hook_ptr = std::conditional_t<IsConst, const ListHook*, ListHook*>;
        hook_ptr ptr;

    public:
        base_iterator(hook_ptr p): ptr(p) {}
        
        base_iterator(const base_iterator&) = default;
        base_iterator& operator=(const base_iterator&) = default;

        reference_type operator*() const {
            return *static_cast<pointer_type>(ptr);
        }

        pointer_type operator->() const {
             return static_cast<pointer_type>(ptr);
        }

        base_iterator& operator++() {
            ptr = ptr->next_;
            return *this;
        }

        base_iterator operator++(int) {
            base_iterator old = *this;
            ++(*this);
            return old;
        }

        base_iterator& operator--() {
            ptr = ptr->prev_;
            return *this;
        }

        base_iterator operator--(int) {
            base_iterator old = *this;
            --(*this);
            return old;
        }

        bool operator==(const base_iterator& rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator!=(const base_iterator& rhs) const {
            return !(*this == rhs);
        }

        operator base_iterator<true>() const {
            return {ptr};
        }
    };

public:
    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;

    iterator Begin() {
        return {dummy_.next_};
    }

    iterator End() {
        return {&dummy_};
    }

    const_iterator Begin() const {
        return {dummy_.next_};
    }

    const_iterator End() const {
        return {&dummy_};
    }

    const_iterator Cbegin() const {
        return {dummy_.next_};
    }

    const_iterator Cend() const {
        return {&dummy_};
    }

    List() {
        dummy_.next_ = dummy_.prev_ = &dummy_;
    }

    List(const List&) = delete;
    List(List&& other) : List() {
        MoveFrom(other);
    }


    List& operator=(const List&) = delete;
    List& operator=(List&& other) {
        if (this != &other) {
            Clear();
            MoveFrom(other);
        }
        return *this;
    }


    bool IsEmpty() const {
        return dummy_.next_ == &dummy_;
    }

    size_t Size() const {
        size_t n = 0;
        for (const_iterator it = Cbegin(); it != Cend(); ++it) {
            ++n;
        }
        return n;
    }

    void PushBack(T* elem) {
        elem->LinkBefore(&dummy_);
    }

    void PushFront(T* elem) {
        elem->LinkBefore(dummy_.prev_);
    }

    T& Front() {
        return *static_cast<T*>(dummy_.next_);
    }

    const T& Front() const {
        return *static_cast<T*>(dummy_.next_);
    }

    T& Back() {
        return *static_cast<T*>(dummy_.prev_);
    }
    const T& Back() const {
        return *static_cast<T*>(dummy_.prev_);
    }

    void PopBack() {
        dummy_.prev_->Unlink();
    }
    void PopFront() {
        dummy_.next_->Unlink();
    }

    iterator IteratorTo(T* element) const {
        return iterator{element};
    }

    void Clear() {
        while (!IsEmpty()) {
            PopBack();
        }
    }

    ~List() {
        Clear();
    }

private:
    void MoveFrom(List& other) {
        if (other.IsEmpty()) {
            return;
        }
        ListHook* first = other.dummy_.next_;
        ListHook* last = other.dummy_.prev_;
        first->prev_ = &dummy_;
        last->next_ = &dummy_;
        dummy_.next_ = first;
        dummy_.prev_ = last;
        other.dummy_.next_ = other.dummy_.prev_ = &other.dummy_;
    }

    ListHook dummy_;
};

template <typename T>
typename List<T>::iterator begin(List<T>& list) {  // NOLINT
    return list.Begin();
}

template <typename T>
typename List<T>::iterator end(List<T>& list) {  // NOLINT
    return list.End();
}
