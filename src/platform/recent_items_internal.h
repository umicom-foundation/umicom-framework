/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/recent_items_internal.h
 *
 * PURPOSE:
 *   Share the private bounded history layout with overflow regression fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RECENT_ITEMS_INTERNAL_H
#define UMICOM_RECENT_ITEMS_INTERNAL_H
#include "umicom/platform/recent_items.h"

/* This layout is not installed or exposed through the public SDK. Tests may
 * seed a boundary revision without performing billions of real mutations. */
struct UmiRecentItemRegistry {
    UmiRecentItemSnapshot items[UMI_PLATFORM_RECENT_ITEMS_CAPACITY];
    size_t count;
    uint64_t revision;
};
#endif
