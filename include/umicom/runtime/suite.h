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

typedef struct UmiSuiteApplication {
    char id[UMI_SUITE_TEXT_CAPACITY];
    char name[UMI_SUITE_TEXT_CAPACITY];
    char path[UMI_SUITE_TEXT_CAPACITY];
    char executable[UMI_SUITE_TEXT_CAPACITY];
    int enabled;
} UmiSuiteApplication;

typedef struct UmiSuite {
    char id[UMI_SUITE_TEXT_CAPACITY];
    char name[UMI_SUITE_TEXT_CAPACITY];
    UmiSuiteApplication applications[UMI_SUITE_MAX_APPLICATIONS];
    size_t application_count;
} UmiSuite;

void umi_suite_init(UmiSuite *suite, const char *id, const char *name);
UmiStatus umi_suite_add(UmiSuite *suite,
                        const UmiSuiteApplication *application);
const UmiSuiteApplication *umi_suite_find(const UmiSuite *suite,
                                          const char *application_id);
UmiStatus umi_suite_validate(const UmiSuite *suite,
                             char *out_message,
                             size_t capacity);
UmiStatus umi_suite_write_manifest(const UmiSuite *suite,
                                   const char *path);

#ifdef __cplusplus
}
#endif

#endif
