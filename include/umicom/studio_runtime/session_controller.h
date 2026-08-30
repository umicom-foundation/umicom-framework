/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/session_controller.h
 *
 * PURPOSE:
 *   Persist/restore the Framework Studio semantic layout and shell state through
 *   the existing UmiSessionStore. No toolkit widget tree is serialized.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SESSION_CONTROLLER_H
#define UMICOM_STUDIO_RUNTIME_SESSION_CONTROLLER_H

#include "umicom/platform/session_store.h"
#include "umicom/studio_runtime/layout_session.h"
#include "umicom/studio_runtime/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioRuntimeSessionController {
    UmiSessionStore *store;
    char path[UMI_STUDIO_RUNTIME_PATH_CAPACITY];
    char prefix[UMI_SESSION_KEY_CAPACITY];
    uint64_t revision;
    int loaded;
} UmiStudioRuntimeSessionController;

UmiStatus umi_studio_session_controller_create(
    const char *path,
    const char *prefix,
    UmiStudioRuntimeSessionController **out_controller);

void umi_studio_session_controller_destroy(
    UmiStudioRuntimeSessionController *controller);

UmiStatus umi_studio_session_controller_save(
    UmiStudioRuntimeSessionController *controller,
    UmiStudioRuntimePlatform *platform);

UmiStatus umi_studio_session_controller_restore(
    UmiStudioRuntimeSessionController *controller,
    UmiStudioRuntimePlatform *platform,
    int *out_restored);

#ifdef __cplusplus
}
#endif
#endif
