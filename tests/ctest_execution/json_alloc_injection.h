/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/json_alloc_injection.h
 * PURPOSE: Intercept allocation only in the dedicated fault-injection build.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CTEST_JSON_ALLOC_INJECTION_H
#define UMICOM_CTEST_JSON_ALLOC_INJECTION_H
#include <stdlib.h>
void *JsonFaultMalloc(size_t size);
void *JsonFaultCalloc(size_t count, size_t size);
void *JsonFaultRealloc(void *pointer, size_t size);
#define malloc JsonFaultMalloc
#define calloc JsonFaultCalloc
#define realloc JsonFaultRealloc
#endif
