/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/bundle.c
 *
 * PURPOSE:
 *   Validate bounded model component selections using canonical model identity and native path services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/local_models/bundle.h"
#include "internal.h"
#include "umicom/platform/output_file.h"
#include "../base/value_construction_internal.h"
#include <stdio.h>
#include <string.h>

static unsigned char bundle_fold(unsigned char c)
{
    return c >= 'A' && c <= 'Z' ? (unsigned char)(c + ('a' - 'A')) : c;
}
static bool bundle_equal(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        if (bundle_fold((unsigned char)*left) != bundle_fold((unsigned char)*right)) return false;
        ++left; ++right;
    }
    return *left == *right;
}
static bool bundle_path_conflict(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        if (bundle_fold((unsigned char)*left) != bundle_fold((unsigned char)*right)) return false;
        ++left; ++right;
    }
    return (*left == '\0' && (*right == '\0' || *right == '/')) ||
           (*right == '\0' && *left == '/');
}
static bool bundle_suffix(const char *filename, const char *suffix)
{
    size_t length = strlen(filename), ending = strlen(suffix);
    return length > ending && strcmp(filename + length - ending, suffix) == 0;
}
static bool bundle_format(const UmiModelBundleFile *file)
{
    const char *name = strrchr(file->filename, '/');
    name = name == NULL ? file->filename : name + 1;
    switch (file->format) {
    case UMI_MODEL_ARTIFACT_GGUF:
        return file->bytes >= 24U && bundle_suffix(name, ".gguf");
    case UMI_MODEL_ARTIFACT_SAFETENSORS:
        return file->bytes >= 10U && bundle_suffix(name, ".safetensors");
    case UMI_MODEL_ARTIFACT_JSON:
        return bundle_suffix(name, ".json");
    case UMI_MODEL_ARTIFACT_TEXT:
        return bundle_suffix(name, ".txt");
    case UMI_MODEL_ARTIFACT_TOKENIZER:
        return bundle_suffix(name, ".model") || bundle_suffix(name, ".tiktoken") ||
            bundle_suffix(name, ".vocab") || bundle_suffix(name, ".merges");
    case UMI_MODEL_ARTIFACT_DOCUMENTATION:
        return bundle_equal(name, "README.md") || bundle_equal(name, "LICENSE") ||
            bundle_equal(name, "LICENCE") || bundle_equal(name, "LICENSE.md") ||
            bundle_equal(name, "LICENCE.md");
    default:
        return false;
    }
}
static UmiStatus bundle_identity(const char *repository, const char *revision)
{
    if (!UmiModelNameValid(repository, 128U, true) || !UmiModelDigestValid(revision, 40U))
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *slash = strchr(repository, '/');
    return slash != NULL && strchr(slash + 1, '/') == NULL
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
static UmiStatus bundle_files(const UmiModelBundleFile *files, size_t count,
    uint64_t *total)
{
    if (count > UMI_MODEL_BUNDLE_MAX_FILES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (files == NULL || count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t sum = 0U;
    for (size_t i = 0U; i < count; ++i) {
        const UmiModelBundleFile *file = &files[i];
        if (!UmiModelNameValid(file->filename, sizeof(file->filename), true) ||
            !UmiModelDigestValid(file->sha256, 64U) || file->bytes == 0U ||
            file->bytes > UMI_MODEL_MAX_BYTES || !bundle_format(file))
            return UMI_STATUS_INVALID_ARGUMENT;
        /* Validate lexical host syntax through the existing platform owner.
         * This synthetic prefix is never opened and is not a destination. */
        char syntax_path[sizeof(file->filename) + 4U];
#ifdef _WIN32
        const char *prefix = "C:/";
#else
        const char *prefix = "/";
#endif
        int written = snprintf(syntax_path, sizeof(syntax_path), "%s%s", prefix, file->filename);
        if (written < 0 || (size_t)written >= sizeof(syntax_path))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        UmiStatus status = UmiOutputFileValidatePath(syntax_path);
        if (status != UMI_STATUS_OK) return status;
        for (size_t previous = 0U; previous < i; ++previous)
            if (bundle_path_conflict(files[previous].filename, file->filename))
                return UMI_STATUS_ALREADY_EXISTS;
        if (file->bytes > UMI_MODEL_MAX_BYTES - sum) return UMI_STATUS_CAPACITY_EXCEEDED;
        sum += file->bytes;
    }
    *total = sum;
    return UMI_STATUS_OK;
}
static UmiStatus model_bundle_create_value(const char *repository, const char *revision,
    const UmiModelBundleFile *files, size_t count, UmiModelBundlePlan *candidate)
{
    UmiStatus status = bundle_identity(repository, revision);
    if (status != UMI_STATUS_OK) return status;
    uint64_t total;
    status = bundle_files(files, count, &total);
    if (status != UMI_STATUS_OK) return status;
    memset(candidate, 0, sizeof(*candidate));
    memcpy(candidate->repository, repository, strlen(repository) + 1U);
    memcpy(candidate->revision, revision, strlen(revision) + 1U);
    memcpy(candidate->files, files, count * sizeof(*files));
    candidate->count = count;
    candidate->total_bytes = total;
    return UMI_STATUS_OK;
}
UMI_DEFINE_STAGED_VALUE_INIT(UmiModelBundlePlanCreate, UmiModelBundlePlan, out_plan,
    (const char *repository, const char *revision, const UmiModelBundleFile *files,
     size_t count, UmiModelBundlePlan *out_plan),
    model_bundle_create_value(repository, revision, files, count, candidate))

UmiStatus UmiModelBundlePlanValidate(const UmiModelBundlePlan *plan)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = bundle_identity(plan->repository, plan->revision);
    if (status != UMI_STATUS_OK) return status;
    uint64_t total;
    status = bundle_files(plan->files, plan->count, &total);
    if (status != UMI_STATUS_OK) return status;
    return total == plan->total_bytes ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
UmiStatus UmiModelBundleGgufSourceAt(const UmiModelBundlePlan *plan, size_t index,
    UmiModelSource *out_source)
{
    if (out_source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiModelBundlePlanValidate(plan);
    if (status != UMI_STATUS_OK) return status;
    if (index >= plan->count) return UMI_STATUS_NOT_FOUND;
    if (plan->files[index].format != UMI_MODEL_ARTIFACT_GGUF) return UMI_STATUS_NOT_IMPLEMENTED;
    UmiModelSource source = {0};
    memcpy(source.repository, plan->repository, sizeof(source.repository));
    memcpy(source.revision, plan->revision, sizeof(source.revision));
    memcpy(source.filename, plan->files[index].filename, sizeof(source.filename));
    memcpy(source.sha256, plan->files[index].sha256, sizeof(source.sha256));
    source.bytes = plan->files[index].bytes;
    status = UmiModelSourceValidate(&source);
    if (status == UMI_STATUS_OK) memcpy(out_source, &source, sizeof(source));
    return status;
}
UmiStatus UmiModelBundleUrlAt(const UmiModelBundlePlan *plan, size_t index,
    char *out_url, size_t capacity)
{
    if (out_url == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiModelBundlePlanValidate(plan);
    if (status != UMI_STATUS_OK) return status;
    if (index >= plan->count) return UMI_STATUS_NOT_FOUND;
    char url[UMI_MODEL_URL_CAPACITY];
    int written = snprintf(url, sizeof(url), "https://huggingface.co/%s/resolve/%s/%s",
        plan->repository, plan->revision, plan->files[index].filename);
    if (written < 0 || (size_t)written >= sizeof(url) || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out_url, url, (size_t)written + 1U);
    return UMI_STATUS_OK;
}
