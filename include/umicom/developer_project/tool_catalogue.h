/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/tool_catalogue.h
 * PURPOSE: Inspect project-selected developer tool files without executing them.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_PROJECT_TOOL_CATALOGUE_H
#define UMICOM_DEVELOPER_PROJECT_TOOL_CATALOGUE_H
#include "umicom/build/profile.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiProjectToolKind
    {
        UMI_PROJECT_TOOL_CMAKE,
        UMI_PROJECT_TOOL_CTEST,
        UMI_PROJECT_TOOL_CPACK,
        UMI_PROJECT_TOOL_NINJA,
        UMI_PROJECT_TOOL_COMPILER,
        UMI_PROJECT_TOOL_GDB,
        UMI_PROJECT_TOOL_LLDB,
        UMI_PROJECT_TOOL_CLANGD
    } UmiProjectToolKind;
    typedef enum UmiProjectToolSelection
    {
        UMI_PROJECT_TOOL_FROM_PATH,
        UMI_PROJECT_TOOL_FROM_DIRECTORY,
        UMI_PROJECT_TOOL_EXPLICIT_FILE,
        UMI_PROJECT_TOOL_AUTOMATIC
    } UmiProjectToolSelection;
    typedef struct UmiProjectToolFile
    {
        UmiProjectToolKind kind;
        UmiProjectToolSelection selection;
        UmiStatus status;
        char requested[UMI_BUILD_PATH_CAPACITY];
        char resolved[UMI_BUILD_PATH_CAPACITY];
    } UmiProjectToolFile;
    typedef struct UmiProjectToolCatalogue UmiProjectToolCatalogue;
    /**
     * Inspect the common build, debugger and C language-server tool files for a
     * validated profile. Capture inherited PATH once; an explicit tools folder never
     * falls back to it. A blank compiler is reported as automatic because CMake may
     * select it from a preset or existing cache. No compiler is guessed.
     * Call on a worker: filesystem metadata may block. Cancellation is checked
     * between bounded file lookups, not inside the host filesystem implementation.
     * Missing optional tools are row results, not a failure of the whole catalogue.
     * No tool or project script runs; existence proves neither executable permission
     * nor architecture, dependencies, DAP/LSP compatibility or workspace trust.
     * Inputs are borrowed until return. Failure clears out; success transfers an
     * immutable owned catalogue. The caller serialises destruction with readers.
     */
    UmiStatus UmiProjectToolCatalogueRead(const UmiBuildProfile *profile,
                                          const UmiCancellationToken *cancel,
                                          UmiProjectToolCatalogue **out);
    /** Return the number of captured tool rows; NULL contains no rows. */
    size_t UmiProjectToolCatalogueCount(const UmiProjectToolCatalogue *catalogue);
    /** Copy one row. Invalid input or an absent index leaves out unchanged. */
    UmiStatus UmiProjectToolCatalogueAt(const UmiProjectToolCatalogue *catalogue, size_t index,
                                        UmiProjectToolFile *out);
    /** Return a static display name; unknown kinds return "Unknown tool". */
    const char *UmiProjectToolName(UmiProjectToolKind kind);
    /** Release the immutable catalogue; NULL is accepted. */
    void UmiProjectToolCatalogueDestroy(UmiProjectToolCatalogue *catalogue);
#ifdef __cplusplus
}
#endif
#endif
