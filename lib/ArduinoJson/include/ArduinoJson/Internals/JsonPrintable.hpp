// Copyright Benoit Blanchon 2014-2016
// MIT License
//
// Arduino JSON library
// https://github.com/bblanchon/ArduinoJson
// If you like this project, please add a star!

#pragma once

#include "../Configuration.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "DummyPrint.hpp"
#include "DynamicStringBuilder.hpp"
#include "IndentedPrint.hpp"
#include "JsonSerializer.hpp"
#include "JsonWriter.hpp"
#include "Prettyfier.hpp"
#include "StaticStringBuilder.hpp"

#if ARDUINOJSON_ENABLE_STD_STREAM
#include "StreamPrintAdapter.hpp"
#endif

namespace ArduinoJson {
namespace Internals {

// Implements all the overloads of printTo() and prettyPrintTo()
// Caution: this class use a template parameter to avoid virtual methods.
// This is a bit curious but allows to reduce the size of JsonVariant, JsonArray
// and JsonObject.
template <typename T>
class JsonPrintable {
 public:
  size_t printTo(Print &print) const {
    JsonWriter writer(print);
    JsonSerializer::serialize(downcast(), writer);
    return writer.bytesWritten();
  }

#if ARDUINOJSON_ENABLE_STD_STREAM
  std::ostream &printTo(std::ostream &os) const {
    StreamPrintAdapter adapter(os);
    printTo(adapter);
    return os;
  }
#endif

  size_t printTo(char *buffer, size_t bufferSize) const {
    StaticStringBuilder sb(buffer, bufferSize);
    return printTo(sb);
  }

  template <typename TString>
  typename TypeTraits::EnableIf<StringFuncs<TString>::has_append, size_t>::type
  printTo(TString &str) const {
    DynamicStringBuilder<TString> sb(str);
    return printTo(sb);
  }

  size_t prettyPrintTo(IndentedPrint &print) const {
    Prettyfier p(print);
    return printTo(p);
  }

  size_t prettyPrintTo(char *buffer, size_t bufferSize) const {
    StaticStringBuilder sb(buffer, bufferSize);
    return prettyPrintTo(sb);
  }

  size_t prettyPrintTo(Print &print) const {
    IndentedPrint indentedPrint = IndentedPrint(print);
    return prettyPrintTo(indentedPrint);
  }

  template <typename TString>
  typename TypeTraits::EnableIf<StringFuncs<TString>::has_append, size_t>::type
  prettyPrintTo(TString &str) const {
    DynamicStringBuilder<TString> sb(str);
    return prettyPrintTo(sb);
  }

  size_t measureLength() const {
    DummyPrint dp;
    return printTo(dp);
  }

  size_t measurePrettyLength() const {
    DummyPrint dp;
    return prettyPrintTo(dp);
  }

 private:
  const T &downcast() const {
    return *static_cast<const T *>(this);
  }
};

#if ARDUINOJSON_ENABLE_STD_STREAM
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const JsonPrintable<T> &v) {
  return v.printTo(os);
}
#endif
}
}
