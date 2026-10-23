/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/healthcare/access_archive.h
 * PURPOSE: Save and reopen bounded synthetic access snapshots without clinical data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_HEALTHCARE_ACCESS_ARCHIVE_H
#define UMICOM_HEALTHCARE_ACCESS_ARCHIVE_H
#include "umicom/healthcare/synthetic_access.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_HEALTHCARE_ACCESS_ARCHIVE_MAX_BYTES 65536U

/* Version 1 JSON contains only synthetic IDs, quantities, state and revisions.
 * The registry and book must belong to the same session. Capture on their owner
 * thread; the returned immutable bytes may then be written by a worker. Output
 * length excludes the terminating NUL. Failure clears both outputs. Release
 * the owned allocation with ArchiveFree. No file I/O occurs in this component. */
UmiStatus UmiHealthcareAccessArchiveEncode(
    const UmiHealthcareSyntheticRegistry *registry,
    const UmiHealthcareAccessBook *book, char **outBytes, size_t *outLength);
void UmiHealthcareAccessArchiveFree(void *bytes);

/* Decode all rows before publishing a new registry/book pair. On any failure,
 * both outputs are NULL. The caller owns both successful handles. Reject unknown
 * versions/fields, duplicate members, oversized input and unreachable states.
 * Original insertion order, IDs, revisions, owners and timestamps are retained.
 * When replacing a session, invalidate its old views and queued commands;
 * revision numbers identify records within their owning session only.
 * A snapshot does not authenticate the author or prove an event occurred. A
 * restored expired deadline stays a reservation until explicitly expired.
 * Reopening an older archive can roll back a session: this is manual synthetic
 * exercise storage, not a clinical database, event log or multiuser protocol. */
UmiStatus UmiHealthcareAccessArchiveDecode(const void *bytes, size_t length,
    UmiHealthcareSyntheticRegistry **outRegistry, UmiHealthcareAccessBook **outBook);
#ifdef __cplusplus
}
#endif
#endif
