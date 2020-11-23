/*
 * Copyright (C) 2016 Simon Fels <morphis@gravedo.de>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ANBOX_DBUS_INTERFACE_H_
#define ANBOX_DBUS_INTERFACE_H_
#include "anbox/system_configuration.h"

namespace anbox {
namespace dbus {
namespace interface {
struct Service {
  static inline const char* path() { return "/org/anbox"; }
  static inline const char* name() {
    static std::string id = SystemConfiguration::instance().container_name();
    std::string str = "org.anbox";
    if (!id.empty())
      str = str.append(id);
    return str.c_str();
  }
};
struct ApplicationManager {
  static inline const char* name() {
    static std::string id = SystemConfiguration::instance().container_name();
    std::string str = "org.anbox.ApplicationManager";
    if (!id.empty())
      str = str.append(id);
    return str.c_str();
  }
  struct Methods {
      struct Launch {
        static inline const char* name() { return "Launch"; }
      };
  };
  struct Properties {
    struct Ready {
      static inline const char* name() { return "Ready"; }
    };
  };
};
}  // namespace interface
}  // namespace dbus
}  // namespace anbox

#endif
