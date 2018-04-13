#ifndef OSSIACO_CONVERTER_FILE_HANDLE_HPP
#define OSSIACO_CONVERTER_FILE_HANDLE_HPP

#include <ossiaco/converter/config.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/core/exceptions.hpp>

#include <memory>

#include <cstdio>

namespace Ossiaco::converter {

namespace detail {

inline void fileDeleter(std::FILE* f)
{
    if (fclose(f))
        throw std::runtime_error("Terminating with failed call to fclose");
}

} // namespace detail

using UniqueFileHandle = std::unique_ptr<std::FILE, decltype(&detail::fileDeleter)>;

inline UniqueFileHandle uniqueHandleFOpen(const CharType* fileName, const CharType* mode)
{
    std::FILE* rawFile;
    errno_t err = OSSIACO_XPLAT_FOPEN_S(&rawFile, fileName, mode);

    if (err)
        throw OpenFileError(err);

    return UniqueFileHandle(rawFile, &detail::fileDeleter);
}

} // namespace Ossiaco::converter

#endif // OSSIACO_CONVERTER_FILE_HANDLE_HPP
