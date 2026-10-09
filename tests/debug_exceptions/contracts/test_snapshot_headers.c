/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_exceptions/contracts/test_snapshot_headers.c
 * PURPOSE: Keep persisted exception records and session filter state compatible
 *          in either public-header order, including real registry archives.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Each target selects a different first header. The filter header itself reaches
 * the debugger service graph, so this also checks its transitive include contract. */
#if defined(UMICOM_EXCEPTION_FILTERS_FIRST) && UMICOM_EXCEPTION_FILTERS_FIRST
#include "umicom/debug_runtime/exception_filters.h"
#include "umicom/debug/exception.h"
#else
#include "umicom/debug/exception.h"
#include "umicom/debug_runtime/exception_filters.h"
#endif

/* The filter aggregate must never alias the established persisted record. These
 * unevaluated expressions check the public C types without providing mock
 * implementations or linking a substitute debugger runtime. */
_Static_assert(
    _Generic((UmiDebugExceptionSnapshot *)0,
        UmiDebugExceptionFiltersSnapshot *: 0,
        default: 1),
    "Persisted exception records and session filter snapshots need distinct types");
_Static_assert(
    _Generic(&UmiDebugRuntimeExceptionFiltersRead,
        UmiStatus (*)(const UmiDebugRuntimePlatform *, UmiDebugExceptionFiltersSnapshot *): 1,
        default: 0),
    "Exception filter reads must require the complete session filter aggregate");
_Static_assert(
    _Generic(&umi_debug_exception_registry_upsert,
        UmiStatus (*)(UmiDebugExceptionRegistry *, const UmiDebugExceptionSnapshot *): 1,
        default: 0),
    "Exception registries must retain the established persisted record contract");

/* Reuse the established public-record checks against the real implementation.
 * Its lifecycle, validation, transfer and portable archive cases verify that
 * including the session filter API does not change record storage or behaviour. */
#include "../../snapshot_contracts/test_debug_exception.c"
