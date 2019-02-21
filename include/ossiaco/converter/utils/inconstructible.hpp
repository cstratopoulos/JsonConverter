// Ossiaco JSON Converter Library
//
// Copyright (C) 2018-2019 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_UTILS_INCONSTRUCTIBLE_HPP
#define OSSIACO_CONVERTER_UTILS_INCONSTRUCTIBLE_HPP

namespace Ossiaco::converter {

/// Utility base class for classes that purely provide static data/methods.
struct Inconstructible {
    Inconstructible()                       = delete;
    Inconstructible(const Inconstructible&) = delete;
    Inconstructible(Inconstructible&&)      = delete;

    Inconstructible& operator=(const Inconstructible&) = delete;
    Inconstructible& operator=(Inconstructible&&) = delete;

    ~Inconstructible() = default;
};


} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_UTILS_INCONSTRUCTIBLE_HPP
