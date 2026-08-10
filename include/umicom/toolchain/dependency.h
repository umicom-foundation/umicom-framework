/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/dependency.h
 *
 * PURPOSE:
 *   Validate native package metadata, headers, import libraries, and runtime
 *   dependencies through the selected pkg-config provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_DEPENDENCY_H
#define UMICOM_TOOLCHAIN_DEPENDENCY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEPENDENCY_TEXT_CAPACITY 256U
#define UMI_DEPENDENCY_MAX 64U

typedef struct UmiDependencyRequirement {
    const char *package_name;
    const char *minimum_version;
    int required;
} UmiDependencyRequirement;

typedef struct UmiDependencyStatus {
    char package_name[UMI_DEPENDENCY_TEXT_CAPACITY];
    char version[UMI_DEPENDENCY_TEXT_CAPACITY];
    int required;
    int available;
} UmiDependencyStatus;

typedef struct UmiDependencyReport {
    UmiDependencyStatus items[UMI_DEPENDENCY_MAX];
    size_t count;
    size_t required_missing;
} UmiDependencyReport;

UmiStatus umi_dependency_check(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiDependencyRequirement *requirements,
    size_t requirement_count,
    UmiDependencyReport *out_report
);

#ifdef __cplusplus
}
#endif

#endif
