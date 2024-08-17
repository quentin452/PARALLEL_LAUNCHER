#ifndef DEFINE_FLAG_OPERATIONS

namespace Flags {
template <typename E> inline bool has(E e, E x) { return (e & x) != (E)0; };
} // namespace Flags

#define DEFINE_FLAG_OPERATIONS(EnumType, BackingType)                          \
  inline EnumType operator|(EnumType f1, EnumType f2) {                        \
    return static_cast<EnumType>(static_cast<BackingType>(f1) |                \
                                 static_cast<BackingType>(f2));                \
  }                                                                            \
  inline EnumType operator&(EnumType f1, EnumType f2) {                        \
    return static_cast<EnumType>(static_cast<BackingType>(f1) &                \
                                 static_cast<BackingType>(f2));                \
  }                                                                            \
  inline EnumType operator^(EnumType f1, EnumType f2) {                        \
    return static_cast<EnumType>(static_cast<BackingType>(f1) ^                \
                                 static_cast<BackingType>(f2));                \
  }                                                                            \
  inline EnumType operator~(EnumType f) {                                      \
    return static_cast<EnumType>(~static_cast<BackingType>(f));                \
  }                                                                            \
  inline EnumType &operator|=(EnumType &f1, EnumType f2) {                     \
    return f1 = f1 | f2;                                                       \
  }                                                                            \
  inline EnumType &operator&=(EnumType &f1, EnumType f2) {                     \
    return f1 = f1 & f2;                                                       \
  }                                                                            \
  inline EnumType &operator^=(EnumType &f1, EnumType f2) {                     \
    return f1 = f1 ^ f2;                                                       \
  }
#endif
