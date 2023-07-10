
#ifndef __tests_ini_test_H__
#define __tests_ini_test_H__

#include <cstring>
#include <istream>

#include "ini.hpp"
#include "m.h"

namespace ini_test {

namespace detail {

const auto ini_values = R"(
[owner]
name=John Doe
organization=Acme Widgets Inc.

[database]
; use IP address in case network name resolution is not working
server=
port=143
file="payroll.dat"
)";

void ini_test_0(void) {
  std::istringstream in(ini_values);
  io::IniFile data;
  data.decode(in);

  assert_eq(std::strcmp(data["owner"]["name"].as<const char *>(), "John Doe"),
            0);
  assert_eq(std::strcmp(data["owner"]["organization"].as<const char *>(),
                        "Acme Widgets Inc."),
            0);
  assert_eq(std::strcmp(data["database"]["server"].as<const char *>(), ""), 0);
  assert_eq(data["database"]["port"].as<int>(), 143);
  assert_eq(
    std::strcmp(data["database"]["file"].as<const char *>(), "\"payroll.dat\""),
    0);
}

} // namespace detail

void ini_test(void) {
  using namespace detail;
  test_case(ini_test_0);
}

} // namespace ini_test

#endif // __tests_ini_test_H__
