
//! C/C++ App GitHub Template
//!
//! Copyright (c) 2023, ThomasByr.
//! AGPL-3.0-or-later (https://www.gnu.org/licenses/agpl-3.0.en.html)
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions are met:
//!
//! * Redistributions of source code must retain the above copyright notice,
//!   this list of conditions and the following disclaimer.
//!
//! * Redistributions in binary form must reproduce the above copyright notice,
//!   this list of conditions and the following disclaimer in the documentation
//!   and/or other materials provided with the distribution.
//!
//! * Neither the name of this software's authors nor the names of its
//!   contributors may be used to endorse or promote products derived from
//!   this software without specific prior written permission.
//!
//! This program is free software: you can redistribute it and/or modify
//! it under the terms of the GNU Affero General Public License as published by
//! the Free Software Foundation, either version 3 of the License, or
//! (at your option) any later version.
//!
//! This program is distributed in the hope that it will be useful,
//! but WITHOUT ANY WARRANTY; without even the implied warranty of
//! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//! GNU Affero General Public License for more details.
//!
//! You should have received a copy of the GNU Affero General Public License
//! along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <cstdlib>
#include <iostream>

#include "clipp.hpp"
#include "utils/fmt.h"

#include "version.h"

enum class mode { run, version, help };

int main(int argc, char *argv[]) {
  using namespace io::clipp;
  auto selected = mode::help;

  // welcome
  fmt::info("Welcome to c-cpp-app!");
  fmt::info("Please visit us at "
            "https://github.com/ThomasByr/c-cpp-app");

  // define cli
  auto run_mode = (command("run").set(selected, mode::run) /* ... */);

  auto cli = (run_mode |
              /* ... */
              option("-h", "--help")
                .set(selected, mode::help)
                .doc("print this help message and exit") |
              option("-v", "--version")
                .set(selected, mode::version)
                .doc("show version information and exit"));

  // parse args
  if (!parse(argc, argv, cli)) {
    fmt::error("Invalid arguments");
    fmt::error("Try '%s --help' for more information", argv[0]);

    std::cerr << make_man_page(cli, C_CPP_APP_NAME) << std::endl;
    return EXIT_FAILURE;
  }
  switch (selected) {
  case mode::run: /* run app*/ break;
  case mode::version: fmt::info("c-cpp-app %s", C_CPP_APP_VERSION); break;
  case mode::help: std::cout << make_man_page(cli, C_CPP_APP_NAME) << std::endl;
  }

  return EXIT_SUCCESS;
}
