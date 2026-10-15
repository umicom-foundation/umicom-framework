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
/** Set one literal -D definition on a private profile draft.
 * Existing names are replaced in place; a new name is appended. type may be
 * NULL or empty for an untyped definition. The complete resulting list passes
 * the same parser as Configure before publication. All refusals preserve every
 * profile byte. No filesystem access, command execution or trust change occurs.
 * Inputs may refer to profile-owned text; access must be serialized by its owner. */
UmiStatus UmiBuildConfigureDefinitionSet(UmiBuildProfile *profile,
    const char *name, const char *type, const char *value);

/** Remove an exact definition name from a private profile draft.
 * A missing valid name succeeds without rewriting the original text. Invalid
 * names, malformed existing definitions and capacity failures preserve the
 * complete profile. Removing a definition does not clear a prior CMake cache;
 * reconfigure an appropriate build directory to establish the intended state. */
UmiStatus UmiBuildConfigureDefinitionRemove(UmiBuildProfile *profile, const char *name);
#ifdef __cplusplus
}
#endif
#endif
