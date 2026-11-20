/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/cvm_file_translation.c
 *
 * PURPOSE:
 *   Keep local file admission and exclusive translation outputs in Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/cvm_file_translation.h"

#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"

UmiStatus UmiCvmTranslateFile(UmcTranslationDirection direction, const char *input_path,
    const char *output_path, UmcDiagnostic *diagnostic)
{
    if (diagnostic != NULL) {
        *diagnostic = (UmcDiagnostic){0};
    }
    if (direction != UMC_TRANSLATE_C_TO_JAVA && direction != UMC_TRANSLATE_JAVA_TO_C) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = UmiOutputFileValidatePath(output_path);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    unsigned char *source = NULL;
    size_t source_size = 0U;
    status = UmiInputFileRead(input_path, UMC_SOURCE_LIMIT, &source, &source_size);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    size_t output_size = 0U;
    /* Keep installed expression-only SDKs working. A compiler with the
     * function profile owns checking and emission for the larger draft. */
#ifdef UMC_TRANSLATION_FUNCTION_OUTPUT_CAPACITY
    char output[UMC_TRANSLATION_FUNCTION_OUTPUT_CAPACITY];
    const UmcStatus translation_status = umc_translate_integer_function(direction,
        (const char *)source, source_size, output, sizeof(output), &output_size, diagnostic);
#else
    char output[UMC_TRANSLATION_OUTPUT_CAPACITY];
    const UmcStatus translation_status = umc_translate_integer_answer(direction,
        (const char *)source, source_size, output, sizeof(output), &output_size, diagnostic);
#endif
    UmiInputFileFree(source);
    if (translation_status != UMC_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }

    /* Complete the translation before creating a file and use the platform
     * owner to enforce exclusive creation for every native frontend. */
    UmiOutputFile *destination = NULL;
    status = UmiOutputFileCreate(output_path, &destination);
    if (status == UMI_STATUS_OK) {
        status = UmiOutputFileWrite(destination, output, output_size);
        const UmiStatus close_status = UmiOutputFileClose(destination);
        if (status == UMI_STATUS_OK) {
            status = close_status;
        }
        UmiOutputFileDestroy(destination);
    }
    return status;
}
