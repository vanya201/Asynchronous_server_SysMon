project(Example)

file(GLOB SOURCES
    "${PROJECT_SOURCE_DIR}/*.h"
    "${PROJECT_SOURCE_DIR}/*.cpp"
)

add_executable(Example
    ${SOURCES}
)

set_target_properties(Example PROPERTIES 
   CXX_STANDARD 20
   CXX_STANDARD_REQUIRED ON
)

target_include_directories(
    Example
    PRIVATE
    ${CORE_SERVER_INCL}
    )

target_link_libraries(
    Example
    PRIVATE
    CoreServer
    )