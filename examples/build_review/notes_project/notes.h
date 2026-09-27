/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/build_review/notes_project/notes.h
 *
 * PURPOSE:
 *   A small Notes word-count exercise for the Studio build-and-test lesson.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_NOTES_PRACTICE_H
#define UMICOM_NOTES_PRACTICE_H
#include <stddef.h>
/* Counts byte-separated words. ASCII space, tab, CR and LF delimit words.
 * UTF-8 non-ASCII bytes belong to a word; this is not Unicode word segmentation.
 * Input is exactly length readable bytes, with no embedded NUL. Returns 0 on
 * success, 1 on invalid input. Failure leaves outCount unchanged. */
int NotesWordCount(const char *text, size_t length, size_t *outCount);
#endif
