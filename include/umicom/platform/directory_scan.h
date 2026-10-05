/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/directory_scan.h
 * PURPOSE: Visit a bounded number of direct children without allocating an unbounded name list.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_DIRECTORY_SCAN_H
#define UMICOM_PLATFORM_DIRECTORY_SCAN_H
#include "umicom/platform/directory.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DIRECTORY_SCAN_MAXIMUM_ENTRIES 16384U
    /* Scan one existing ordinary absolute directory in native enumeration order.
 * Count every entry except dot and dot-dot, including entries a visitor ignores.
 * Stop with CAPACITY_EXCEEDED before visiting an entry beyond maximum_entries.
 * No recursion, sorting or name collection occurs. Metadata describes links;
 * links are never opened as files. The root itself must not be a link.
 *
 * Use a directory controlled by the current user: this metadata walk is not a
 * confinement boundary against concurrent renames. Rooted-file services should
 * recheck any selected file when reading its content. Visitors run on the caller
 * thread and may have side effects even when a later entry fails. Keep private
 * candidate state and publish it only after a successful scan. Cancellation is
 * checked between entries; an OS operation cannot be interrupted. */
    UmiStatus UmiDirectoryScanShallow(const char *root, size_t maximum_entries, UmiDirectoryVisitor visitor,
                                      void *context, const UmiCancellationToken *cancel);
#ifdef __cplusplus
}
#endif
#endif
