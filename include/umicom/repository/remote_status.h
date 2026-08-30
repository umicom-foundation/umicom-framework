/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remote_status.h
 *
 * PURPOSE:
 *   Represent repository remote and upstream configuration health.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_REPOSITORY_REMOTE_STATUS_H
#define INCLUDE_UMICOM_REPOSITORY_REMOTE_STATUS_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositoryRemoteStatus {
    size_t remote_count;
    int has_origin;
    int upstream_configured;
    int fetch_available;
} UmiRepositoryRemoteStatus;

void umi_repository_remote_status_init(UmiRepositoryRemoteStatus *status);
UmiStatus umi_repository_remote_status_validate(const UmiRepositoryRemoteStatus *status);

#ifdef __cplusplus
}
#endif
#endif
