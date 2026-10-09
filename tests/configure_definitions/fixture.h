/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/configure_definitions/fixture.h
 * PURPOSE: Share bounded test assertions and platform-specific project roots for configure options.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_CONFIGURE_DEFINITIONS_FIXTURE_H
#define UMICOM_TEST_CONFIGURE_DEFINITIONS_FIXTURE_H
#include "umicom/build/configure_definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
#ifdef _WIN32
#define PROJECT_ROOT "C:/Umicom test projects/notes"
#else
#define PROJECT_ROOT "/tmp/Umicom test projects/notes"
#endif
static inline void Profile(UmiBuildProfile *profile)
{
    CHECK(umi_build_profile_set(profile, "notes", PROJECT_ROOT, "build") == UMI_STATUS_OK);
}
#endif
