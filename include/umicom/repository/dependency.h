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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_DEPENDENCY_H
#define UMICOM_REPOSITORY_DEPENDENCY_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositoryDependency {
    char id[UMI_REPOSITORY_CONTROL_NAME_CAPACITY];
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    int required;
} UmiRepositoryDependency;
UmiStatus umi_repository_dependency_init(
    UmiRepositoryDependency *dependency,
    const char *id,
    const char *path,
    int required);
UmiStatus umi_repository_dependency_validate(
    const UmiRepositoryDependency *dependency);
#ifdef __cplusplus
}
#endif
#endif
