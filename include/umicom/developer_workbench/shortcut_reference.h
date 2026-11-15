/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/shortcut_reference.h
 *
 * PURPOSE:
 *   Format the keyboard bindings supplied by a native host as a bounded, read-only UTF-8 reference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_WORKBENCH_SHORTCUT_REFERENCE_H
#define UMICOM_DEVELOPER_WORKBENCH_SHORTCUT_REFERENCE_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SHORTCUT_REFERENCE_LIMIT 512U
typedef struct UmiShortcutReferenceEntry {
    char command[128];
    char caption[256];
    char shortcut[96];
    int available;
} UmiShortcutReferenceEntry;
/** Format actual host bindings in supplied order. No binding is registered or
 * executed. Availability describes the captured instant, not a guarantee that
 * a later invocation will succeed. Count is 0..512; entries may be NULL only
 * for zero count. Fields must be nonempty, terminated within their arrays and
 * valid UTF-8; available is 0 or 1. Controls and backslashes are escaped.
 * Query bytes excluding NUL with out=NULL,capacity=0. A short buffer changes
 * required only and returns CAPACITY_EXCEEDED. Other failures preserve both
 * outputs. Success includes a terminating NUL when out is non-NULL.
 * Inputs and outputs must not overlap. Serialize input access. No allocation,
 * callbacks, I/O, preference changes or command dispatch occurs. */
UmiStatus UmiShortcutReferenceFormat(const UmiShortcutReferenceEntry *entries,
    size_t count, char *out, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
