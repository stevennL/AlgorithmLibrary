#ifndef TASTYLIB_UNIQUEPTR_H_
#define TASTYLIB_UNIQUEPTR_H_

#include "tastylib/internal/base.h"
#include <utility>

TASTYLIB_NS_BEGIN

template<typename, typename>
class UniquePtr;

template<typename T, typename D>
void swap(UniquePtr<T, D>& lhs, UniquePtr<T, D>& rhs) {
    lhs.swap(rhs);
}

class DefaultDeleter {
public:
    template<typename T>
    void operator()(T* p) const noexcept {
        delete p;
    }
};

// Simplified version of std::unique_ptr
template<typename T, typename D = DefaultDeleter>
class UniquePtr {
    friend void swap<T, D>(UniquePtr<T, D>& lhs, UniquePtr<T, D>& rhs);

public:
    // Default ctor (optional custom deleter)
    UniquePtr(const D& d = D()) : UniquePtr(nullptr, d) {}

    // Construct from raw pointer (optional custom deleter)
    explicit UniquePtr(T* p, const D& d = D()) : ptr(p), deleter(d) {}

    // Forbid copy
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Move ctor
    UniquePtr(UniquePtr&& other) : ptr(other.ptr), deleter(std::move(other.deleter)) {
        other.ptr = nullptr;
    }

    // Move assignment
    UniquePtr& operator=(UniquePtr&& rhs) {
        if (this != &rhs) {
            del();
            ptr = rhs.ptr;
            deleter = std::move(rhs.deleter);
            rhs.ptr = nullptr;
        }
        return *this;
    }

    // Nullptr assignment
    UniquePtr& operator=(std::nullptr_t) noexcept {
        del();
        return *this;
    }

    // Dtor
    ~UniquePtr() noexcept {
        del();
    }

    // Swap members with another UniquePtr
    void swap(UniquePtr& rhs) {
        using std::swap;
        swap(ptr, rhs.ptr);
        swap(deleter, rhs.deleter);
    }

    // Safe bool conversion
    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }

    // Dereference operator
    T& operator*() const noexcept {
        return *ptr;
    }

    // Member access operator
    T* operator->() const noexcept {
        return ptr;
    }

    // Return underlying pointer
    T* get() const noexcept {
        return ptr;
    }

    // Relinquish control of the underlying pointer
    T* release() noexcept {
        T *ret = ptr;
        ptr = nullptr;
        return ret;
    }

    // Reset underlying pointer
    void reset(T* p = nullptr) noexcept {
        del();
        ptr = p;
    }

private:
    void del() noexcept {
        if (ptr) {
            deleter(ptr);
            ptr = nullptr;
        }
    }

private:
    T *ptr;
    D deleter;
};

TASTYLIB_NS_END

#endif
