/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/submodule.h
 *
 * PURPOSE:
 *   Model one Framework-owned Git submodule dependency.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_SUBMODULE_H
#define UMICOM_REPOSITORY_SUBMODULE_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositorySubmodule {
    char name[UMI_REPOSITORY_CONTROL_NAME_CAPACITY];
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    char url[UMI_REPOSITORY_CONTROL_URL_CAPACITY];
    char branch[UMI_REPOSITORY_CONTROL_REF_CAPACITY];
    int required;
} UmiRepositorySubmodule;
UmiStatus umi_repository_submodule_init(
    UmiRepositorySubmodule *submodule,
    const char *name,
    const char *path,
    const char *url,
    const char *branch,
    int required);
UmiStatus umi_repository_submodule_validate(
    const UmiRepositorySubmodule *submodule);
#ifdef __cplusplus
}
#endif
#endif
