/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/submodule_status.h
 *
 * PURPOSE:
 *   Summarise configured, initialised, dirty, missing and mismatched submodules.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_REPOSITORY_SUBMODULE_STATUS_H
#define INCLUDE_UMICOM_REPOSITORY_SUBMODULE_STATUS_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositorySubmoduleStatus {
    size_t configured;
    size_t initialised;
    size_t dirty;
    size_t missing;
    size_t head_mismatch;
} UmiRepositorySubmoduleStatus;

void umi_repository_submodule_status_init(UmiRepositorySubmoduleStatus *status);
UmiStatus umi_repository_submodule_status_validate(const UmiRepositorySubmoduleStatus *status);
int umi_repository_submodule_status_healthy(const UmiRepositorySubmoduleStatus *status);

#ifdef __cplusplus
}
#endif
#endif
