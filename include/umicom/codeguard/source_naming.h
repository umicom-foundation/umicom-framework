/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/source_naming.h
 *
 * PURPOSE:
 *   Define a reusable source-name policy that keeps one meaningful,
 *   unversioned filename for each implementation and document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CODEGUARD_SOURCE_NAMING_H
#define UMICOM_CODEGUARD_SOURCE_NAMING_H

#include "umicom/base/status.h"
#include "umicom/codeguard/result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiCodeGuardSourceNameIssue {
    UMI_CODEGUARD_SOURCE_NAME_OK = 0,
    UMI_CODEGUARD_SOURCE_NAME_VERSION_LABEL,
    UMI_CODEGUARD_SOURCE_NAME_BATCH_LABEL
} UmiCodeGuardSourceNameIssue;

/* Classify one path without opening or changing the referenced file. */
UmiCodeGuardSourceNameIssue umi_codeguard_source_name_classify(
    const char *path);

/* Return a short stable label that can be displayed by any application. */
const char *umi_codeguard_source_name_issue_text(
    UmiCodeGuardSourceNameIssue issue);

/* Add a quality finding when a filename contains a version or batch label. */
UmiStatus umi_codeguard_source_name_audit(const char *path,
                                          UmiCodeGuardResult *result);

#ifdef __cplusplus
}
#endif

#endif
