/* Umicom Foundation | MIT | An intentionally small Notes record. */
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

typedef struct UmiLearningNote {
    char title[40];
    size_t length;
} UmiLearningNote;

bool UmiLearningNoteSetTitle(UmiLearningNote *note, const char *title)
{
    if (note == 0 || title == 0) return false;
    size_t length = strlen(title);
    if (length >= sizeof(note->title)) return false;
    memcpy(note->title, title, length + 1U);
    note->length = length;
    return true;
}
