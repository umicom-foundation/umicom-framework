/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_cpp_header.cpp
 *
 * PURPOSE:
 *   Verify each compiler header can be included directly and resolves a representative C symbol from a C++ client.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* This file is compiled separately for each selected header. No earlier
 * Framework include may accidentally establish its missing linkage contract. */
#ifndef UMICOM_CPP_HEADER
#error UMICOM_CPP_HEADER must name the independently included public header
#endif
#ifndef UMICOM_CPP_SYMBOL
#error UMICOM_CPP_SYMBOL must name its representative external function
#endif
#include UMICOM_CPP_HEADER
#include <type_traits>

/* Including a C++ standard header afterwards also detects a linkage block that
 * leaks beyond the public header. The volatile pointer retains a real symbol
 * reference during optimized builds; this test never calls an arbitrary API. */
static_assert(std::is_pointer_v<decltype(&UMICOM_CPP_SYMBOL)>);
static auto volatile linked_symbol = &UMICOM_CPP_SYMBOL;
int main()
{
    return linked_symbol == nullptr ? 1 : 0;
}
