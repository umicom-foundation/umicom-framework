/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/construction_cases.h
 * PURPOSE: Verify that rejected construction preserves an existing inline value.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VALUE_CONSTRUCTION_CASES_H
#define UMICOM_VALUE_CONSTRUCTION_CASES_H
#include "fixture_assertions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Each owner supplies its public constructor and a domain-valid fixture. Keep
 * the output and comparison bytes off the stack so array-bearing records are
 * covered on platforms with small default thread stacks. Every refusal is
 * checked byte-for-byte, including fields not mentioned in the attempted edit. */
#define UMI_CONSTRUCTION_REQUIRE(Expression) \
    do { if (!(Expression)) { \
        fprintf(stderr, "Construction integrity failed at %s:%d: %s\n", \
                __FILE__, __LINE__, #Expression); \
        goto construction_cleanup; \
    } } while (0)
#endif
