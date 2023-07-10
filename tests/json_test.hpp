
#ifndef __tests_json_test_H__
#define __tests_json_test_H__

#include "json.hpp"
#include "m.h"

namespace json_test {

namespace detail {

const auto json_values = R"(
{
  "name": "John Doe",
  "age": 43,
  "address": {
    "street": "Downing Street",
    "number": 10
  },
  "phones": [
    "+44 1234567",
    "+44 2345678"
  ]
}
)";

void json_test_0(void) {
  auto data = io::json::parse(json_values);

  assert_eq(data["name"], "John Doe");
  assert_eq(data["age"], 43);
  assert_eq(data["address"]["street"], "Downing Street");
  assert_eq(data["address"]["number"], 10);
  assert_eq(data["phones"][0], "+44 1234567");
  assert_eq(data["phones"][1], "+44 2345678");
}

void json_test_1(void) {
  auto data = io::json::parse(json_values);

  assert_eq(data["address"].size(), 2);
  assert_eq(data["phones"].size(), 2);
}

} // namespace detail

void json_test(void) {
  using namespace detail;
  test_case(json_test_0);
  test_case(json_test_1);
}

} // namespace json_test

#endif // __tests_json_test_H__
