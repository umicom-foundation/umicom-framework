/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Framework-owned learning output; applications do not duplicate this service. */
#ifndef UMICOM_EDUCATION_WORKSPACE_PROJECTS_H
#define UMICOM_EDUCATION_WORKSPACE_PROJECTS_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Immutable embedded bytes generated from the canonical teaching project
 * sources by CMake. Export does not compile, run, download or modify a repo. */
typedef struct UmiEducationProjectFile {
    const char *name;
    const unsigned char *bytes;
    size_t size;
} UmiEducationProjectFile;
size_t UmiEducationProjectFileCount(const char *projectId);
UmiStatus UmiEducationProjectFileAt(const char *projectId,size_t index,
    UmiEducationProjectFile *outFile);
/* Destination must be an absolute, NEW directory in a trusted local location.
 * Existing directories and files are refused, even if empty. All files are
 * created exclusively. This is not a filesystem sandbox: ancestor directories
 * must be owned/trusted by the caller. No rollback deletes a partial export.
 * On failure outFilesWritten reports complete writes; partial files may remain.
 * Windows accepts local drive paths (UTF-8); UNC/network destinations are denied. */
UmiStatus UmiEducationExportProject(const char *projectId,const char *destination,
    size_t *outFilesWritten);
#ifdef __cplusplus
}
#endif
#endif
