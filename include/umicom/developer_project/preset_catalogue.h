/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/preset_catalogue.h
 * PURPOSE: Read declared project preset choices without executing build tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_PROJECT_PRESET_CATALOGUE_H
#define UMICOM_DEVELOPER_PROJECT_PRESET_CATALOGUE_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/build/profile.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_PROJECT_PRESET_LIMIT 128U
#define UMI_PROJECT_PRESET_FILE_LIMIT 65535U
    typedef enum UmiProjectPresetStage
    {
        UMI_PROJECT_PRESET_CONFIGURE,
        UMI_PROJECT_PRESET_BUILD,
        UMI_PROJECT_PRESET_TEST
    } UmiProjectPresetStage;
    typedef struct UmiProjectPresetChoice
    {
        UmiProjectPresetStage stage;
        char name[UMI_BUILD_NAME_CAPACITY];
        char display_name[256];
        char description[512];
        char configure_preset[UMI_BUILD_NAME_CAPACITY];
        char binary_directory[UMI_BUILD_PATH_CAPACITY];
        bool from_user_file, hidden, condition_false, condition_deferred, inherits;
    } UmiProjectPresetChoice;
    typedef struct UmiProjectPresetSummary
    {
        size_t count;
        bool project_file, user_file, includes_not_read;
    } UmiProjectPresetSummary;
    typedef struct UmiProjectPresetCatalogue UmiProjectPresetCatalogue;
    /* Parse complete bounded UTF-8 JSON, then copy direct declarations from the two
 * standard documents. NULL with zero bytes means absent; an empty present file
 * is invalid. Each document is at most FILE_LIMIT bytes and shares the existing
 * JSON reader's token/depth bounds. Duplicate names within a stage are rejected
 * across both files, including hidden choices. At least one file is required.
 * Output is NULL on failure. The catalogue owns every returned value.
 * This is metadata discovery, not complete CMake schema/availability validation:
 * includes, inheritance, conditions and macros are not evaluated. */
    UmiStatus UmiProjectPresetCatalogueCreate(const void *projectBytes, size_t projectSize,
                                              const void *userBytes, size_t userSize,
                                              UmiProjectPresetCatalogue **out);
    /* Read only CMakePresets.json and CMakeUserPresets.json in an absolute project
 * directory using the shared bounded regular-file reader. No process, include
 * traversal, environment expansion or file write occurs. A missing file is
 * optional; other read/parse errors reject the whole snapshot. I/O can block:
 * interactive callers must use a worker. The two reads are not an atomic disk
 * snapshot, and names may change after reading; refresh before selecting. */
    UmiStatus UmiProjectPresetCatalogueRead(const char *projectDirectory, UmiProjectPresetCatalogue **out);
    void UmiProjectPresetCatalogueDestroy(UmiProjectPresetCatalogue *catalogue);
    UmiStatus UmiProjectPresetCatalogueSummary(const UmiProjectPresetCatalogue *catalogue,
                                               UmiProjectPresetSummary *out);
    UmiStatus UmiProjectPresetCatalogueAt(const UmiProjectPresetCatalogue *catalogue, size_t index,
                                          UmiProjectPresetChoice *out);
    /* Copy one selected name into its stage field. Preserve all other profile
 * values; reject a shared legacy preset, hidden entry or literal false
 * condition. Deferred conditions can still be rejected by CMake when run.
 * Failure leaves out unchanged; selecting does not save, trust or execute. */
    UmiStatus UmiProjectPresetCatalogueSelect(const UmiProjectPresetCatalogue *catalogue, size_t index,
                                              const UmiBuildProfile *profile, UmiBuildProfile *out);
/* Expanded discovery is an explicit read of referenced local files. It keeps
 * the direct-document APIs above unchanged. At most 64 distinct documents,
 * 2 MiB per document, 8 MiB total and PRESET_LIMIT choices are accepted.
 * Literal relative/absolute include paths and the sourceDir/fileDir macros are
 * supported when the document format permits them. Environment and other
 * include macros return NOT_IMPLEMENTED; no partial catalogue is published.
 *
 * Resolve inherited configurePreset, binaryDir and condition metadata only.
 * Names, hidden state, display names and descriptions are never inherited.
 * Condition objects and macros inside preset values remain deferred to CMake.
 * This is discovery, not complete schema validation or a build plan evaluator.
 * Selected directories are trusted; included paths may be outside the project.
 * Final symlink/device leaves are refused by the shared bounded reader.
 * Files are separate snapshots, not a transaction across the filesystem.
 * Cancellation is cooperative between reads and JSON traversal. Keep the token
 * alive until return and use a worker for interactive calls. */
typedef struct UmiProjectPresetOrigin {
    char file_path[UMI_BUILD_PATH_CAPACITY];
    bool included_file;
    bool inherited_metadata;
} UmiProjectPresetOrigin;
typedef struct UmiProjectPresetReadReport {
    size_t file_count, total_bytes;
    char problem_file[UMI_BUILD_PATH_CAPACITY];
} UmiProjectPresetReadReport;
UmiStatus UmiProjectPresetCatalogueReadExpanded(const char *projectDirectory,
    const UmiCancellationToken *cancel, UmiProjectPresetCatalogue **out,
    UmiProjectPresetReadReport *report);
/* Origin is available only for expanded discovery. A direct-document catalogue
 * returns NOT_FOUND and leaves out unchanged. All strings are owned copies. */
UmiStatus UmiProjectPresetCatalogueOriginAt(const UmiProjectPresetCatalogue *catalogue,
    size_t index, UmiProjectPresetOrigin *out);

#ifdef __cplusplus
}
#endif
#endif
