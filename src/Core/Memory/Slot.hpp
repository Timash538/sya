#include <cstddef>
#include <memory>
#include <utility>

namespace sya
{

template <typename T>
class Slot
{
  public:
    template <typename... Args>
    void construct(Args&&... args);
    T& get();
    void destroy();

  private:
    T* ptr();
    alignas(T) std::byte object[sizeof(T)];
};

template <typename T>
template <typename... Args>
void Slot<T>::construct(Args&&... args)
{
    std::construct_at(ptr(), std::forward<Args>(args)...);
}

template <typename T>
T* Slot<T>::ptr()
{
    return reinterpret_cast<T*>(object);
}

template <typename T>
T& Slot<T>::get()
{
    return *ptr();
}

template <typename T>
void Slot<T>::destroy()
{
    std::destroy_at(ptr());
}

} // namespace sya
