/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/configure_definitions.h
 * PURPOSE: Validate project CMake definitions without invoking a shell or evaluating source.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_CONFIGURE_DEFINITIONS_H
#define UMICOM_BUILD_CONFIGURE_DEFINITIONS_H
#include "umicom/build/profile.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /** Parse profile.configure_definitions using the shared quoted-argument grammar.
 * Every token must be -DNAME[:TYPE]=VALUE. Names begin with an ASCII letter or
 * underscore and continue with letters, digits, underscore, dot or hyphen.
 * Supported types are BOOL, PATH, FILEPATH, STRING and INTERNAL. Values may be
 * empty and preserve spaces/semicolons; control bytes are refused. Duplicate names
 * and settings owned by dedicated profile fields are invalid. At most 32 tokens
 * fit, each shorter than UMI_BUILD_ARGUMENT_CAPACITY, including its -D prefix.
 * This parses values only: it neither checks SDK paths nor grants build trust.
 * Failure clears out. The profile must not overlap output storage. */
    UmiStatus UmiBuildConfigureDefinitions(const UmiBuildProfile *profile, UmiArguments *out);
#ifdef __cplusplus
}
#endif
#endif
