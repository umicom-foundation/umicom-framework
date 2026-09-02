/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/control_types.h
 *
 * PURPOSE:
 *   Define stable bounded types and repository-control state vocabulary.
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
#ifndef UMICOM_REPOSITORY_CONTROL_TYPES_H
#define UMICOM_REPOSITORY_CONTROL_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REPOSITORY_CONTROL_NAME_CAPACITY 128U
#define UMI_REPOSITORY_CONTROL_PATH_CAPACITY 2048U
#define UMI_REPOSITORY_CONTROL_URL_CAPACITY 2048U
#define UMI_REPOSITORY_CONTROL_REF_CAPACITY 256U
#define UMI_REPOSITORY_CONTROL_SHA_CAPACITY 65U
#define UMI_REPOSITORY_CONTROL_TEXT_CAPACITY 1024U
#define UMI_REPOSITORY_CONTROL_ITEM_CAPACITY 64U
#define UMI_REPOSITORY_CONTROL_EDGE_CAPACITY 128U
#define UMI_REPOSITORY_CONTROL_TRANSACTION_CAPACITY 64U
#define UMI_REPOSITORY_CONTROL_EVENT_CAPACITY 128U
#define UMI_REPOSITORY_CONTROL_ISSUE_CAPACITY 64U

/**
 * List the named repository control state values accepted by this public contract.
 */
typedef enum UmiRepositoryControlState {
    UMI_REPOSITORY_CONTROL_IDLE = 0,
    UMI_REPOSITORY_CONTROL_INSPECTING = 1,
    UMI_REPOSITORY_CONTROL_PLANNED = 2,
    UMI_REPOSITORY_CONTROL_STAGING = 3,
    UMI_REPOSITORY_CONTROL_VERIFIED = 4,
    UMI_REPOSITORY_CONTROL_FAILED = 5
} UmiRepositoryControlState;

/**
 * List the named repository control severity values accepted by this public contract.
 */
typedef enum UmiRepositoryControlSeverity {
    UMI_REPOSITORY_CONTROL_INFO = 0,
    UMI_REPOSITORY_CONTROL_WARNING = 1,
    UMI_REPOSITORY_CONTROL_ERROR = 2
} UmiRepositoryControlSeverity;

/**
 * Provide the repository control state text operation used by this module and its client
 * applications.
 */
const char *umi_repository_control_state_text(UmiRepositoryControlState state);
/**
 * Provide the repository control severity text operation used by this module and its
 * client applications.
 */
const char *umi_repository_control_severity_text(
    UmiRepositoryControlSeverity severity);

#ifdef __cplusplus
}
#endif

#endif
