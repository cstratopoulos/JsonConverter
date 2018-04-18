# The integration test suite consists purely of isolated .cpp files
# which include one of the catch-all headers from ossiaco/converter
# to ensure they can be compiled alone with no errors.

add_executable(converter_integration
    integration_main.cpp
    adapt.cpp
    allocate.cpp
    components.cpp
    serialize.cpp
)

target_link_libraries(converter_integration
    PUBLIC ossiaco_converter
)