/*-----------------------------------------------------------------------------
 * Umicom Framework | Federated desktop persistence
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_PERSISTENCE_H
#define UMICOM_DESKTOP_PERSISTENCE_H

#include "umicom/desktop/runtime.h"

#define UMI_DESKTOP_PERSISTENCE_TEXT_CAPACITY 131072U

UmiStatus umi_desktop_persistence_encode(
    UmiDesktopRuntime *runtime,
    char *out_text,
    size_t capacity,
    size_t *out_length);
UmiStatus umi_desktop_persistence_decode(
    UmiDesktopRuntime *runtime,
    const char *text);
UmiStatus umi_desktop_persistence_decode_transactional(
    UmiDesktopRuntime *runtime,
    const char *text);

#endif
