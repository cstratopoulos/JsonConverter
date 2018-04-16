// Ossiaco JSON Converter Library
// 
// Copyright (C) 2018 Ossiaco
// 
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
// 
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_CORE_POINTER_WRAPPER_HPP
#define OSSIACO_CONVERTER_CORE_POINTER_WRAPPER_HPP

#include <memory>

namespace Ossiaco::converter {

/// Type erasure interface base for [ReferenceMapper] storage.
class PointerWrapperBase {
public:
    virtual ~PointerWrapperBase() = default;

    virtual std::shared_ptr<void> value() const = 0;

protected:
    PointerWrapperBase()                          = default;
    PointerWrapperBase(const PointerWrapperBase&) = delete;
    PointerWrapperBase(PointerWrapperBase&&)      = default;

    PointerWrapperBase& operator=(const PointerWrapperBase&) = delete;
    PointerWrapperBase& operator=(PointerWrapperBase&&) = default;
};

template<typename PropertyType>
class PointerWrapper : public PointerWrapperBase {
public:
    PointerWrapper(std::unique_ptr<PropertyType> value) : _value(std::move(value))
    {}

    std::shared_ptr<void> value() const override
    {
        return std::static_pointer_cast<void>(_value);
    }
private:
    std::shared_ptr<PropertyType> _value;
};

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_CORE_POINTER_WRAPPER_HPP
