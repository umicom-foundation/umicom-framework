/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/self_memory_internal.h
 *
 * PURPOSE:
 *   Keep the native self-memory observation boundary private to the desktop service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_SELF_MEMORY_INTERNAL_H
#define UMICOM_SELF_MEMORY_INTERNAL_H
#include "umicom/desktop_system/self_memory.h"
UmiStatus UmiDesktopSelfMemoryReadNative(UmiSelfMemorySample *out);
#endif
