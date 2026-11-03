/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_module.c
 *
 * PURPOSE:
 *   Prepare content modules without coupling their lifetime to a product window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/teacher/learning_module.h"
#include "umicom/platform/path.h"
#include "umicom/platform/rooted_files.h"
#include <string.h>

UmiStatus UmiLearningModuleRoot(const char *executable, const char *name,
    char *out_root, size_t capacity)
{
    if (executable == NULL || name == NULL || name[0] == '\0' || out_root == NULL ||
        !umi_path_is_absolute(executable)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; name[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)name[i];
        if (i >= 63U || !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_')) return UMI_STATUS_INVALID_ARGUMENT;
    }
    char parent[UMI_PATH_CAPACITY], modules[UMI_PATH_CAPACITY], result[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_parent(executable, parent, sizeof(parent));
    if (status == UMI_STATUS_OK) status = umi_path_join(parent, "learning-modules", modules, sizeof(modules));
    if (status == UMI_STATUS_OK) status = umi_path_join(modules, name, result, sizeof(result));
    if (status != UMI_STATUS_OK) return status;
    if (strlen(result) >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out_root, result, strlen(result) + 1U);
    return UMI_STATUS_OK;
}

UmiStatus UmiLearningModuleOpen(const UmiLearningShelf *shelf, const char *directory,
    const char *pack_id, const char *product, const char *resource_id,
    UmiVersion host, size_t maximum_bytes, UmiLearningShelf **out_shelf,
    UmiLearningReader **out_reader)
{
    if (out_shelf != NULL) *out_shelf = NULL;
    if (out_reader != NULL) *out_reader = NULL;
    if (shelf == NULL || directory == NULL || pack_id == NULL || product == NULL ||
        resource_id == NULL || out_shelf == NULL || out_reader == NULL || maximum_bytes == 0U ||
        pack_id[0] == '\0' || product[0] == '\0' || resource_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiRootedFileValidatePath(directory, "learning-pack.json");
    if (status != UMI_STATUS_OK) return status;
    UmiLearningShelf *candidate = NULL;
    UmiLearningReader *reader = NULL;
    UmiLearningPack *pack = NULL;
    UmiLearningPackInfo info;
    UmiLearningPackResource resource;
    status = UmiLearningShelfClone(shelf, &candidate);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfCopyPack(candidate, pack_id, &pack);
    int loaded = status == UMI_STATUS_NOT_FOUND;
    if (loaded) status = UmiLearningPackLoad(directory, "learning-pack.json", host, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningPackCopyInfo(pack, &info);
    if (status == UMI_STATUS_OK && (strcmp(info.id, pack_id) != 0 || strcmp(info.product, product) != 0))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiLearningPackFindResource(pack, resource_id, &resource);
    if (status == UMI_STATUS_OK && loaded) status = UmiLearningShelfAdopt(candidate, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningReaderCreateFromShelf(candidate, info.id,
        info.revision, resource.id, resource.sha256, maximum_bytes, &reader);
    UmiLearningPackDestroy(pack);
    if (status != UMI_STATUS_OK) {
        UmiLearningReaderDestroy(reader);
        UmiLearningShelfDestroy(candidate);
        return status;
    }
    *out_shelf = candidate;
    *out_reader = reader;
    return UMI_STATUS_OK;
}
