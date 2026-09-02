/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/persistence.h
 *
 * PURPOSE:
 *   Publish the public persistence contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_PERSISTENCE_H
#define UMICOM_DESKTOP_PERSISTENCE_H

#include "umicom/desktop/runtime.h"

#define UMI_DESKTOP_PERSISTENCE_TEXT_CAPACITY 131072U

/**
 * Write desktop persistence in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_desktop_persistence_encode(
    UmiDesktopRuntime *runtime,
    char *out_text,
    size_t capacity,
    size_t *out_length);
/**
 * Read desktop persistence into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_desktop_persistence_decode(
    UmiDesktopRuntime *runtime,
    const char *text);
/**
 * Provide the desktop persistence decode transactional operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_persistence_decode_transactional(
    UmiDesktopRuntime *runtime,
    const char *text);

#endif
