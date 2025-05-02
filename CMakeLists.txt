cmake_minimum_required(VERSION 3.20)

project(vdvauser LANGUAGES CXX)

include(cmake/buildcache.cmake)
include(cmake/pkg.cmake)

# --- LINT ---
option(LINT "Run clang-tidy with the compiler." OFF)
if (LINT)
    # clang-tidy will be run on all targets defined hereafter
    include(cmake/clang-tidy.cmake)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(compile-options
            -Weverything
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-newline-eof
            -Wno-missing-prototypes
            -Wno-padded
            -Wno-double-promotion
            -Wno-undef
            -Wno-undefined-reinterpret-cast
            -Wno-float-conversion
            -Wno-global-constructors
            -Wno-exit-time-destructors
            -Wno-switch-enum
            -Wno-c99-designator
            -Wno-zero-as-null-pointer-constant
            -Wno-missing-noreturn
            -Wno-undefined-func-template
            -Wno-unsafe-buffer-usage
            -Wno-c++20-compat
            -Wno-reserved-macro-identifier
            -Wno-documentation-unknown-command
            -Wno-duplicate-enum
            -Wno-ctad-maybe-unsupported
            -Wno-unknown-pragmas
            -Wno-c++20-extensions
            -Wno-switch-default
            -Wno-unused-template
            -Wno-shadow-uncaptured-local
            -Wno-documentation-deprecated-sync
            -Wno-float-equal
            -Wno-deprecated-declarations
            -Werror)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(compile-options -Wall -Wextra -Werror -Wno-unknown-pragmas -Wno-deprecated-declarations)
elseif (MSVC)
    set(compile-options /WX /bigobj)
else ()
    set(compile-options
            -Wall
            -Wextra
            -Wno-mismatched-new-delete
            -Wno-maybe-uninitialized)
    if (NOT CMAKE_CROSSCOMPILING)
        set(compile-options ${compile-options} -Werror)
    endif ()
endif ()

# --- LIB ---
file(GLOB_RECURSE libfiles src/*.cc)
add_library(vdvauserlib ${libfiles})
target_include_directories(vdvauserlib PUBLIC include)
target_compile_features(vdvauserlib PUBLIC cxx_std_23)
target_compile_options(vdvauserlib PRIVATE ${compile-options})
target_link_libraries(vdvauserlib
        Boost::fiber
        web-server
        fmt
        pugixml
)

# --- EXE ---
add_executable(vdvauser exe/vdvauser.cc)
target_compile_features(vdvauser PUBLIC cxx_std_23)
target_link_libraries(vdvauser boost-program_options vdvauserlib)
