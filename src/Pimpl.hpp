#pragma once

#include <memory>

namespace XMakeProjectManager::Internal {
    template<typename T>
    class Pimpl {
      public:
        Pimpl(std::nullptr_t) {}
        Pimpl() { m_impl = std::make_unique<T>(); }

        template<typename... Args>
        Pimpl(Args &&...args) {
            m_impl = std::make_unique<T>(std::forward<Args>(args)...);
        }

        Pimpl(Pimpl &&other) : m_impl{std::move(other.m_impl)} {}
        Pimpl &operator=(Pimpl &&other) {
            if(this != &other) {
                m_impl = std::move(other.m_impl);
            }

            return *this;
        }

        ~Pimpl() = default;

        T *operator->() { return m_impl.get(); }
        const T *operator->() const { return m_impl.get(); }

        T &operator*() { return *m_impl.get(); }
        const T &operator*() const { return *m_impl.get(); }

      private:
        std::unique_ptr<T> m_impl;
    };
} // namespace XMakeProjectManager::Internal
