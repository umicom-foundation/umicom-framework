/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/tool_catalogue.c
 * PURPOSE: Capture one immutable, non-executing inventory of project developer tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/tool_catalogue.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process_search_path.h"
#include "umicom/toolchain/discovery.h"
#include <stdlib.h>
#include <string.h>
struct UmiProjectToolCatalogue
{
    size_t count;
    UmiProjectToolFile files[8];
};
typedef struct ProjectToolDefinition
{
    UmiProjectToolKind kind;
    const char *program;
    const char *name;
} ProjectToolDefinition;
/* Extend this table and its bounded storage together. These rows describe common
 * capabilities, not a claim that every project requires every listed tool. */
static const ProjectToolDefinition PROJECT_TOOLS[] = {
    {UMI_PROJECT_TOOL_CMAKE, "cmake", "CMake"},
    {UMI_PROJECT_TOOL_CTEST, "ctest", "CTest"},
    {UMI_PROJECT_TOOL_CPACK, "cpack", "CPack"},
    {UMI_PROJECT_TOOL_NINJA, "ninja", "Ninja"},
    {UMI_PROJECT_TOOL_COMPILER, "", "C compiler"},
    {UMI_PROJECT_TOOL_GDB, "gdb", "GDB debugger"},
    {UMI_PROJECT_TOOL_LLDB, "lldb-dap", "LLDB debug adapter"},
    {UMI_PROJECT_TOOL_CLANGD, "clangd", "Clangd language server"}};
static_assert(sizeof PROJECT_TOOLS / sizeof PROJECT_TOOLS[0] ==
                  sizeof(((UmiProjectToolCatalogue *)0)->files) / sizeof(UmiProjectToolFile),
              "Every tool definition needs one bounded catalogue row.");
const char *UmiProjectToolName(UmiProjectToolKind kind)
{
    for (size_t index = 0U; index < sizeof PROJECT_TOOLS / sizeof PROJECT_TOOLS[0]; ++index)
        if (PROJECT_TOOLS[index].kind == kind)
            return PROJECT_TOOLS[index].name;
    return "Unknown tool";
}
/* A compiler field is a path or one program name, never a shell command. The
 * ordinary profile still owns compiler semantics during CMake configuration. */
static int ProjectToolSimpleName(const char *name)
{
    if (name[0] == '\0' || strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        return 0;
    for (const unsigned char *p = (const unsigned char *)name; *p != 0U; ++p)
        if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') ||
              *p == '_' || *p == '-' || *p == '.' || *p == '+'))
            return 0;
    return 1;
}
static UmiStatus ProjectToolInspect(const UmiBuildProfile *profile, const char *path,
                                    const ProjectToolDefinition *definition,
                                    UmiProjectToolFile *out)
{
    out->kind = definition->kind;
    const char *program =
        definition->kind == UMI_PROJECT_TOOL_COMPILER ? profile->compiler : definition->program;
    memcpy(out->requested, program, strlen(program) + 1U);
    if (program[0] == '\0')
    {
        out->selection = UMI_PROJECT_TOOL_AUTOMATIC;
        out->status = UMI_STATUS_NOT_IMPLEMENTED;
        return UMI_STATUS_OK;
    }
    if (umi_path_is_absolute(program))
    {
        out->selection = UMI_PROJECT_TOOL_EXPLICIT_FILE;
        memcpy(out->resolved, program, strlen(program) + 1U);
        out->status = umi_fs_is_file(program) ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
        return UMI_STATUS_OK;
    }
    out->selection = profile->tool_directory[0] != '\0' ? UMI_PROJECT_TOOL_FROM_DIRECTORY
                                                        : UMI_PROJECT_TOOL_FROM_PATH;
    if (!ProjectToolSimpleName(program))
    {
        out->status = UMI_STATUS_INVALID_ARGUMENT;
        return UMI_STATUS_OK;
    }
    const char *search = profile->tool_directory[0] != '\0' ? profile->tool_directory : path;
    out->status =
        UmiToolchainFindInSearchPath(program, search, out->resolved, sizeof out->resolved);
    /* A missing file is useful evidence. Allocation/capacity errors must instead
     * refuse the complete capture so a partial inventory cannot masquerade as full. */
    return out->status == UMI_STATUS_OUT_OF_MEMORY || out->status == UMI_STATUS_CAPACITY_EXCEEDED
               ? out->status
               : UMI_STATUS_OK;
}
UmiStatus UmiProjectToolCatalogueRead(const UmiBuildProfile *profile,
                                      const UmiCancellationToken *cancel,
                                      UmiProjectToolCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiProjectToolCatalogue *catalogue = calloc(1U, sizeof *catalogue);
    if (catalogue == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    char *path = NULL;
    if (profile->tool_directory[0] == '\0')
        status = UmiProcessSearchPathRead(&path);
    for (size_t index = 0U;
         status == UMI_STATUS_OK && index < sizeof PROJECT_TOOLS / sizeof PROJECT_TOOLS[0]; ++index)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = ProjectToolInspect(profile, path, &PROJECT_TOOLS[index], &catalogue->files[index]);
        if (status == UMI_STATUS_OK)
            ++catalogue->count;
    }
    UmiProcessSearchPathFree(path);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(catalogue);
        return status;
    }
    *out = catalogue;
    return UMI_STATUS_OK;
}
size_t UmiProjectToolCatalogueCount(const UmiProjectToolCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
UmiStatus UmiProjectToolCatalogueAt(const UmiProjectToolCatalogue *catalogue, size_t index,
                                    UmiProjectToolFile *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->files[index];
    return UMI_STATUS_OK;
}
void UmiProjectToolCatalogueDestroy(UmiProjectToolCatalogue *catalogue) { free(catalogue); }
