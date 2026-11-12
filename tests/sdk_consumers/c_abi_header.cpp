/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_consumers/c_abi_header.cpp
 *
 * PURPOSE:
 *   Include one public C API directly from C++ and retain a real link reference without invoking the function.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#ifndef UMICOM_CPP_HEADER
#error UMICOM_CPP_HEADER must name the independently included public header
#endif
#ifndef UMICOM_CPP_SYMBOL
#error UMICOM_CPP_SYMBOL must name an external C function declared by that header
#endif

/* No umbrella header may accidentally establish the missing linkage first. */
#include UMICOM_CPP_HEADER
#include <type_traits>

/* A C++ standard header after the API also catches an unclosed extern block.
 * The volatile pointer makes the linker resolve the C implementation even when
 * optimizations are enabled. Arbitrary GUI and service functions are not called. */
static_assert(std::is_pointer_v<decltype(&UMICOM_CPP_SYMBOL)>);
static auto volatile linked_symbol = &UMICOM_CPP_SYMBOL;

int main()
{
    return linked_symbol == nullptr ? 1 : 0;
}
