/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/teacher/learning_library.h
 * PURPOSE: Resolve an explicitly selected lesson library independently of project and application
 * data folders. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEACHER_LEARNING_LIBRARY_H
#define UMICOM_TEACHER_LEARNING_LIBRARY_H
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @brief Locate the lessons below an absolute, user-selected directory.
     * @param selected Applications, Framework, docs, or the learning directory itself.
     * @param out_directory Receives the normalised directory containing welcome.html.
     * @param capacity Size of the output buffer, including its terminator.
     * @return OK, NOT_FOUND, INVALID_ARGUMENT or CAPACITY_EXCEEDED.
     * No directory is created, downloaded or inferred from the working directory.
     * Failure leaves output unchanged. A successful selection does not grant build trust.
     */
    UmiStatus UmiLearningLibrarySelect(const char *selected, char *out_directory, size_t capacity);
    /**
     * @brief Resolve an existing curriculum resource inside the chosen library.
     * @param directory The absolute learning directory returned by Select.
     * @param resource_path An exact resource path from the foundations curriculum.
     * @param out_file Receives the absolute regular-file path on success.
     * @param capacity Size of the output buffer, including its terminator.
     * @return OK or a validation/filesystem status; failure leaves output unchanged.
     * The catalogue is the allow-list. Arbitrary paths, parent traversal and remote
     * URLs cannot become lesson resources. This is a local user library, not a sandbox
     * for hostile symlinks; its files are opened as documentation, never executed.
     */
    UmiStatus UmiLearningLibraryResource(const char *directory, const char *resource_path,
                                         char *out_file, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
