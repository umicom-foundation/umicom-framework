/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_practice.c
 *
 * PURPOSE:
 *   Separate verified example selection from explicit non-overwriting file creation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_practice.h"

UmiStatus UmiLearningPracticeResolveExample(const UmiLearningPack *pack,
    const char *selected, UmiLearningPackResource *out)
{
    if (pack == NULL || selected == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningPackResource resource;
    UmiStatus status = UmiLearningPackFindResource(pack, selected, &resource);
    if (status != UMI_STATUS_OK) return status;
    if (resource.kind != UMI_LEARNING_RESOURCE_EXAMPLE) {
        if (resource.example_id[0] == '\0') return UMI_STATUS_NOT_FOUND;
        UmiLearningPackResource example;
        status = UmiLearningPackFindResource(pack, resource.example_id, &example);
        if (status != UMI_STATUS_OK) return status;
        resource = example;
    }
    /* Manifest admission checks this too. Keep the boundary explicit for
     * future pack providers so an arbitrary document cannot become code. */
    if (resource.kind != UMI_LEARNING_RESOURCE_EXAMPLE) return UMI_STATUS_INVALID_STATE;
    *out = resource;
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningPracticeCopy(const UmiLearningPack *pack, const char *selected,
    const char *root, const char *relative, size_t maximum, UmiLearningPracticeResult *result)
{
    if (result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *result = (UmiLearningPracticeResult){0};
    result->status = UMI_STATUS_INVALID_ARGUMENT;
    if (pack == NULL || selected == NULL || maximum == 0U || maximum > UMI_LEARNING_PRACTICE_MAX_BYTES)
        return result->status;
    UmiStatus status = UmiRootedFileValidatePath(root, relative);
    if (status == UMI_STATUS_OK)
        status = UmiLearningPracticeResolveExample(pack, selected, &result->example);
    unsigned char *bytes = NULL;
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiLearningPackReadResource(pack, result->example.id, maximum, &bytes, &size);
    if (status == UMI_STATUS_OK) {
        result->source_verified = 1;
        result->source_size = size;
        result->destination_attempted = 1;
        /* Pass the exact verified buffer to rooted exclusive creation. Reopening
         * the source by pathname here could copy different unverified bytes. */
        status = UmiRootedFileCreate(root, relative, bytes, size, &result->destination);
    }
    UmiLearningPackFreeBytes(bytes);
    result->status = status;
    return status;
}
