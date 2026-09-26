/* Umicom Foundation | Sammy Hegab | MIT
 * The release laboratory keeps one real native DLL boundary visible. The
 * function counts ASCII whitespace-separated words in an explicit byte span.
 * It does not allocate, save a file or retain the caller's text. */
#ifndef UMICOM_RELEASE_NOTES_MODEL_H
#define UMICOM_RELEASE_NOTES_MODEL_H
#include <stddef.h>
#include <stdint.h>
#if defined(_WIN32)
# if defined(UMICOM_RELEASE_NOTES_EXPORTS)
#  define UMI_RELEASE_NOTES_API __declspec(dllexport)
# else
#  define UMI_RELEASE_NOTES_API __declspec(dllimport)
# endif
#else
# define UMI_RELEASE_NOTES_API
#endif
#ifdef __cplusplus
extern "C" {
#endif
UMI_RELEASE_NOTES_API int UmiReleaseNotesWordCount(const char *text, size_t length, uint32_t *outWords);
#ifdef __cplusplus
}
#endif
#endif
