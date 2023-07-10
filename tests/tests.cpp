
#include <iostream>

#include "ctpl_test.hpp"
#include "ini_test.hpp"
#include "json_test.hpp"

unsigned long _no_asserts = 0;

int main() {
  json_test::json_test();
  ini_test::ini_test();
  ctpl_test::ctpl_test();
}
