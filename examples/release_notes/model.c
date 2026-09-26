/* Umicom Foundation | Sammy Hegab | MIT
 * A small DLL deliberately used by the Windows installed-startup exercise.
 * It is a teaching model, not a replacement for the Framework document model. */
#include "model.h"
int UmiReleaseNotesWordCount(const char *text, size_t length, uint32_t *outWords)
{
    if (!outWords) return 1;
    *outWords = 0;
    if (!text || length > 32768U) return 1;
    int inWord = 0;
    for (size_t i = 0; i < length; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == 0) { *outWords = 0; return 1; }
        int space = c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
        if (!space && !inWord) ++*outWords;
        inWord = !space;
    }
    return 0;
}
