/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/target_catalogue.h
 * PURPOSE: Read configured CMake targets and review their executable paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_PROJECT_TARGET_CATALOGUE_H
#define UMICOM_DEVELOPER_PROJECT_TARGET_CATALOGUE_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/build/profile.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* Heap-backed discovery replaces the small fixed-array budget. The earlier
 * limits remain here for review; existing public names retain their purpose. */
#if 0
#define UMI_PROJECT_TARGET_LIMIT 256U
#define UMI_PROJECT_TARGET_DOCUMENT_LIMIT 65535U
#endif
#define UMI_PROJECT_TARGET_LIMIT 32768U
#define UMI_PROJECT_TARGET_DOCUMENT_LIMIT (16U * 1024U * 1024U)
    typedef struct UmiProjectTargetChoice
    {
        char name[UMI_BUILD_NAME_CAPACITY];
        char identity[512];
        char type[32];
        char program[UMI_BUILD_PATH_CAPACITY];
        bool executable;
    } UmiProjectTargetChoice;
    typedef struct UmiProjectTargetSummary
    {
        char source_directory[UMI_BUILD_PATH_CAPACITY];
        char build_directory[UMI_BUILD_PATH_CAPACITY];
        char configuration[UMI_BUILD_NAME_CAPACITY];
        char index_file[UMI_BUILD_PATH_CAPACITY];
        size_t count;
    } UmiProjectTargetSummary;
    typedef struct UmiProjectTargetCatalogue UmiProjectTargetCatalogue;
    typedef enum UmiProjectTargetSelection
    {
        UMI_PROJECT_TARGET_BUILD,
        UMI_PROJECT_TARGET_PROGRAM
    } UmiProjectTargetSelection;
    /* Read the newest CMake file-API index and its referenced codemodel and targets.
 * Both directories must be absolute. The configured roots and configuration
 * must match; a single unnamed configuration is also accepted. Each JSON file
 * has the byte limit above and the shared parser's token/depth limits. No
 * partial catalogue is published. Output is NULL on failure; destroy after use.
 * Existing file-API replies are required. This call creates no query, launches
 * no CMake process, and does not establish that an executable has been built.
 * Use a worker: directory enumeration and regular-file reads can block. Parent
 * aliases may be followed; the reader is not filesystem confinement. */
    UmiStatus UmiProjectTargetCatalogueRead(const char *sourceDirectory, const char *buildDirectory,
                                            const char *configuration, UmiProjectTargetCatalogue **out);
    /* The same complete-snapshot contract with cooperative cancellation.
     * Checks occur during directory traversal, between files and while parsing.
     * An individual OS read, memory allocation or duplicate sort can still
     * finish before cancellation is observed. Keep the token alive for the
     * call. NULL preserves ordinary reads; cancellation publishes no rows. */
    UmiStatus UmiProjectTargetCatalogueReadCancellable(const char *sourceDirectory,
        const char *buildDirectory, const char *configuration,
        const UmiCancellationToken *cancel, UmiProjectTargetCatalogue **out);
    void UmiProjectTargetCatalogueDestroy(UmiProjectTargetCatalogue *catalogue);
    UmiStatus UmiProjectTargetCatalogueSummary(const UmiProjectTargetCatalogue *catalogue,
                                               UmiProjectTargetSummary *out);
    UmiStatus UmiProjectTargetCatalogueAt(const UmiProjectTargetCatalogue *catalogue, size_t index,
                                          UmiProjectTargetChoice *out);
    /* Copy one reviewed value only, preserving every other profile field. Selection
 * rechecks the profile roots/configuration against the loaded snapshot. Build
 * presets own their targets, so manual target selection refuses a build preset.
 * Program selection requires an EXECUTABLE with exactly one artifact matching
 * nameOnDisk and no launcher requirement. No save, file-existence check, trust grant or process is performed.
 * Disk data may change after the read. Failure leaves out unchanged; aliasing
 * profile and out is supported. */
    UmiStatus UmiProjectTargetCatalogueSelect(const UmiProjectTargetCatalogue *catalogue, size_t index,
                                              UmiProjectTargetSelection selection,
                                              const UmiBuildProfile *profile, UmiBuildProfile *out);
#ifdef __cplusplus
}
#endif
#endif
