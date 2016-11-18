// Copyright Benoit Blanchon 2014-2016
// MIT License
//
// Arduino JSON library
// https://github.com/bblanchon/ArduinoJson
// If you like this project, please add a star!

#pragma once

#include "../JsonBuffer.hpp"
#include "../JsonVariant.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "StringFuncs.hpp"

namespace ArduinoJson {
namespace Internals {

template <typename TSource, typename Enable = void>
struct ValueSetter {
  template <typename TDestination>
  static bool set(JsonBuffer*, TDestination& destination,
                  const TSource& source) {
    destination = source;
    return true;
  }
};

template <typename TSource>
struct ValueSetter<TSource, typename TypeTraits::EnableIf<
                                StringFuncs<TSource>::should_duplicate>::type> {
  template <typename TDestination>
  static bool set(JsonBuffer* buffer, TDestination& destination,
                  const TSource& source) {
    const char* copy = buffer->strdup(source);
    if (!copy) return false;
    destination = copy;
    return true;
  }
};
}
}
