/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/event.h
 *
 * PURPOSE:
 *   Publish deterministic repository-control state transition events.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_EVENT_H
#define UMICOM_REPOSITORY_EVENT_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositoryControlEvent {
    uint64_t sequence;
    UmiRepositoryControlState state;
    UmiStatus status;
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    char message[UMI_REPOSITORY_CONTROL_TEXT_CAPACITY];
} UmiRepositoryControlEvent;
UmiStatus umi_repository_event_init(
    UmiRepositoryControlEvent *event,
    uint64_t sequence,
    UmiRepositoryControlState state,
    UmiStatus status,
    const char *path,
    const char *message);
#ifdef __cplusplus
}
#endif
#endif
