/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_environment/fixture.h
 * PURPOSE: Share focused launch-environment assertions and portable profile roots.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RUN_ENVIRONMENT_FIXTURE_H
#define UMICOM_RUN_ENVIRONMENT_FIXTURE_H
#include "umicom/build/profile.h"
#include "umicom/platform/path.h"
#include "umicom/platform/process_environment.h"
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
#define PROJECT_ROOT "C:/Umicom Environment Fixture"
#else
#define PROJECT_ROOT "/tmp/umicom-environment-fixture"
#endif
#endif
