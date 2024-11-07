#ifndef EFP_TYPECLASS_HPP_
#define EFP_TYPECLASS_HPP_

#include "efp/meta.hpp"

namespace efp {

// typeclass is a function that takes a type or a type constructor and returns boolean
// Type -> Bool, (Type -> Type) -> Bool, template<typename... > class -> Bool
// type constructor could be seen as a Type -> Type

// Functor
template<template<typename...> class TypeConstructor>
struct Functor {
    
}

};  // namespace efp

#endif 