/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/dependency.h
 *
 * PURPOSE:
 *   Define reusable repository dependency nodes.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_DEPENDENCY_H
#define UMICOM_REPOSITORY_DEPENDENCY_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository dependency data shared with callers of this public contract.
 */
typedef struct UmiRepositoryDependency {
    char id[UMI_REPOSITORY_CONTROL_NAME_CAPACITY];
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    int required;
} UmiRepositoryDependency;
/**
 * Initialise repository dependency from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_repository_dependency_init(
    UmiRepositoryDependency *dependency,
    const char *id,
    const char *path,
    int required);
/**
 * Check that repository dependency satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_repository_dependency_validate(
    const UmiRepositoryDependency *dependency);
#ifdef __cplusplus
}
#endif
#endif
