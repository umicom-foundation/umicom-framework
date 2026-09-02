/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/suite.h
 *
 * PURPOSE:
 *   Define composition of independent Umicom applications into one suite while
 *   retaining application identity, launchability, paths, and shared Framework
 *   ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_SUITE_H
#define UMICOM_RUNTIME_SUITE_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SUITE_TEXT_CAPACITY 256U
#define UMI_SUITE_MAX_APPLICATIONS 32U

/**
 * Represent the suite application data shared with callers of this public contract.
 */
typedef struct UmiSuiteApplication {
    char id[UMI_SUITE_TEXT_CAPACITY];
    char name[UMI_SUITE_TEXT_CAPACITY];
    char path[UMI_SUITE_TEXT_CAPACITY];
    char executable[UMI_SUITE_TEXT_CAPACITY];
    int enabled;
} UmiSuiteApplication;

/**
 * Represent the suite data shared with callers of this public contract.
 */
typedef struct UmiSuite {
    char id[UMI_SUITE_TEXT_CAPACITY];
    char name[UMI_SUITE_TEXT_CAPACITY];
    UmiSuiteApplication applications[UMI_SUITE_MAX_APPLICATIONS];
    size_t application_count;
} UmiSuite;

/**
 * Initialise suite from caller-provided values so later operations receive a known state.
 */
void umi_suite_init(UmiSuite *suite, const char *id, const char *name);
/**
 * Add suite only after its inputs and available capacity have been checked.
 */
UmiStatus umi_suite_add(UmiSuite *suite,
                        const UmiSuiteApplication *application);
/**
 * Find suite while leaving the underlying catalogue or model owned by this module.
 */
const UmiSuiteApplication *umi_suite_find(const UmiSuite *suite,
                                          const char *application_id);
/**
 * Check that suite satisfies its contract before another service relies on it.
 */
UmiStatus umi_suite_validate(const UmiSuite *suite,
                             char *out_message,
                             size_t capacity);
/**
 * Provide the suite write manifest operation used by this module and its client
 * applications.
 */
UmiStatus umi_suite_write_manifest(const UmiSuite *suite,
                                   const char *path);

#ifdef __cplusplus
}
#endif

#endif
