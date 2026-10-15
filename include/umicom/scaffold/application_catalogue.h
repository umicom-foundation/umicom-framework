/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/scaffold/application_catalogue.h
 * PURPOSE: Inspect registered application modules for native developer tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_SCAFFOLD_APPLICATION_CATALOGUE_H
#define UMICOM_SCAFFOLD_APPLICATION_CATALOGUE_H
#include "umicom/runtime/application_module.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_APPLICATION_CATALOGUE_LIMIT 128U
typedef struct UmiApplicationCatalogue UmiApplicationCatalogue;
/** One registered direct child of applications. A failed manifest status is
 * retained as evidence of an unavailable or invalid module, not silently omitted. */
typedef struct UmiApplicationCatalogueEntry {
    char path[UMI_PATH_CAPACITY];
    UmiApplicationModuleDescriptor module;
    UmiStatus manifest_status;
} UmiApplicationCatalogueEntry;
/** A copied inventory summary. Registration is read from .gitmodules, not
 * inferred from copied directories. This does not prove a published gitlink. */
typedef struct UmiApplicationCatalogueSummary {
    char root[UMI_PATH_CAPACITY];
    size_t count;
    size_t ready;
    size_t unavailable;
} UmiApplicationCatalogueSummary;
/** Diagnostics never include repository URLs or authentication material. */
typedef struct UmiApplicationCatalogueIssue {
    UmiStatus status;
    char path[UMI_PATH_CAPACITY];
    char conflicting_path[UMI_PATH_CAPACITY];
} UmiApplicationCatalogueIssue;
/**
 * Read registered paths with Git's configuration parser, without include files,
 * then load each bounded application manifest. root must be absolute; gitProgram
 * is an explicit executable or NULL for PATH lookup. Use a worker: this performs
 * local process and file reads, never fetch, checkout, registration or execution
 * of application code. Parent directory aliases are trusted.
 * Missing manifests remain visible in entries. Duplicate identity, component,
 * option or executable names reject the catalogue to avoid ambiguous selection.
 * Failure clears out. issue is optional. The caller owns the returned catalogue.
 */
UmiStatus UmiApplicationCatalogueLoad(const char *root, const char *gitProgram,
    UmiApplicationCatalogue **out, UmiApplicationCatalogueIssue *issue);
/** Copy the root and counts. Invalid arguments clear a supplied output. */
UmiStatus UmiApplicationCatalogueSummaryRead(const UmiApplicationCatalogue *catalogue,
    UmiApplicationCatalogueSummary *out);
/** Borrow an entry until catalogue destruction. Invalid indices return NULL. */
const UmiApplicationCatalogueEntry *UmiApplicationCatalogueAt(
    const UmiApplicationCatalogue *catalogue, size_t index);
/** The first observed reason a proposed registration cannot be accepted.
 * This is source evidence, never permission to run Git or application code. */
typedef enum UmiApplicationCandidateReason {
    UMI_APPLICATION_CANDIDATE_READY = 0,
    UMI_APPLICATION_CANDIDATE_INVALID,
    UMI_APPLICATION_CANDIDATE_PATH,
    UMI_APPLICATION_CANDIDATE_ID,
    UMI_APPLICATION_CANDIDATE_OPTION,
    UMI_APPLICATION_CANDIDATE_COMPONENT,
    UMI_APPLICATION_CANDIDATE_EXECUTABLE,
    UMI_APPLICATION_CANDIDATE_UNAVAILABLE,
    UMI_APPLICATION_CANDIDATE_CAPACITY
} UmiApplicationCandidateReason;
/** Caller-owned preflight evidence. A conflicting path names the existing
 * registration; unavailable entries prevent a complete collision check. */
typedef struct UmiApplicationCandidateReport {
    UmiStatus status;
    UmiApplicationCandidateReason reason;
    size_t checked;
    char conflicting_path[UMI_PATH_CAPACITY];
} UmiApplicationCandidateReport;

/** Compare a proposed direct applications/slug path and validated descriptor
 * with this immutable catalogue. No files, Git index or settings are changed.
 * Success means no conflict was observed in the snapshot, not that a later
 * registration is authorised or race-free. Reload after repository changes.
 * report is required and is cleared before use; it must not overlap inputs. */
UmiStatus UmiApplicationCatalogueCheckCandidate(
    const UmiApplicationCatalogue *catalogue, const char *path,
    const UmiApplicationModuleDescriptor *module, UmiApplicationCandidateReport *report);
/** Return a static explanation for a preflight reason; unknown values are
 * described as invalid. The returned text is borrowed and must not be freed. */
const char *UmiApplicationCandidateReasonText(UmiApplicationCandidateReason reason);

/** Release the complete snapshot. NULL is accepted. */
void UmiApplicationCatalogueDestroy(UmiApplicationCatalogue *catalogue);
#ifdef __cplusplus
}
#endif
#endif
