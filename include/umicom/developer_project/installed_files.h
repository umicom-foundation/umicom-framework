/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/installed_files.h
 * PURPOSE: Review a completed CMake installation and select its program explicitly.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_PROJECT_INSTALLED_FILES_H
#define UMICOM_DEVELOPER_PROJECT_INSTALLED_FILES_H
#include "umicom/build/profile.h"
#include "umicom/platform/cancellation.h"
#include "umicom/platform/directory.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_PROJECT_INSTALLED_FILE_LIMIT 32768U
#define UMI_PROJECT_INSTALL_MANIFEST_LIMIT (16U * 1024U * 1024U)
    typedef struct UmiProjectInstalledFiles UmiProjectInstalledFiles;
    typedef struct UmiProjectInstalledSummary
    {
        char source_directory[UMI_BUILD_PATH_CAPACITY];
        char build_directory[UMI_BUILD_PATH_CAPACITY];
        char install_directory[UMI_BUILD_PATH_CAPACITY];
        size_t count;
    } UmiProjectInstalledSummary;
    typedef struct UmiProjectInstalledFile
    {
        char path[UMI_BUILD_PATH_CAPACITY];
        UmiStatus inspection_status;
        UmiFileKind kind;
        uint64_t size;
        uint64_t modified_nanoseconds;
        bool program_candidate;
    } UmiProjectInstalledFile;
    /* Read the full-install manifest in the chosen build folder. Source must be
 * absolute; nonempty build and install folders may be relative to source.
 * There is no CWD fallback or preset evaluation: choose the actual folders.
 * UTF-8 paths, LF/CRLF, an optional final newline and an empty manifest are
 * supported. Blank interior lines, controls, dot segments, duplicate paths
 * and files outside the install prefix are refused. Component manifests and
 * DESTDIR staging are not inferred. No partial catalogue is returned.
 * Missing files remain visible with their inspection status. A regular .exe
 * on Windows, or an executable regular file on POSIX, is a candidate only;
 * this does not prove binary compatibility, dependencies, safety or trust.
 * Run on a worker. Cancellation is cooperative between storage operations.
 * Parent aliases may be followed: prefix checks are lexical, not confinement.
 * Keep the token alive for the call. NULL means no cancellation. */
    UmiStatus UmiProjectInstalledFilesRead(const UmiBuildProfile *profile, const UmiCancellationToken *cancel,
                                           UmiProjectInstalledFiles **out);
    void UmiProjectInstalledFilesDestroy(UmiProjectInstalledFiles *files);
    UmiStatus UmiProjectInstalledFilesSummary(const UmiProjectInstalledFiles *files,
                                              UmiProjectInstalledSummary *out);
    UmiStatus UmiProjectInstalledFilesAt(const UmiProjectInstalledFiles *files, size_t index,
                                         UmiProjectInstalledFile *out);
    /* Re-read the manifest and selected file metadata on a worker, then copy only
 * run_program into an otherwise unchanged profile. Refuse changed roots,
 * changed manifest bytes, removed/nonregular files, lost executable permission,
 * changed size or modification time. Failure leaves out unchanged; in-place
 * selection is supported. Metadata is not a content hash and changes can occur
 * after return. No save, launch, shell command or trust grant is performed.
 * Serialize catalogue access and retain it until this operation completes. */
    UmiStatus UmiProjectInstalledFilesSelect(const UmiProjectInstalledFiles *files, size_t index,
                                             const UmiBuildProfile *profile,
                                             const UmiCancellationToken *cancel, UmiBuildProfile *out);
#ifdef __cplusplus
}
#endif
#endif
