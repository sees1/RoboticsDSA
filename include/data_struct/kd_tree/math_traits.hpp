#include <type_traits>

template <typename T, typename = void>
struct has_bound : std::false_type {};
template <typename T>
struct has_bound<T, std::void_t<decltype(std::declval<T>().getBBox())>> : std::true_type {};


template <typename Primitive>
struct primitive_traits {
  using type = Primitive;
  static constexpr bool has_bound_v = has_bound<type>::value;
};