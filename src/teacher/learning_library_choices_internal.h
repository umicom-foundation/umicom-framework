/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_library_choices_internal.h
 *
 * PURPOSE:
 *   Keep bounded remembered-library storage and admission private to Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LEARNING_LIBRARY_CHOICES_INTERNAL_H
#define UMICOM_LEARNING_LIBRARY_CHOICES_INTERNAL_H
#include "umicom/teacher/learning_library_choices.h"
struct UmiLearningLibraryChoices {
    size_t count;
    UmiLearningLibraryChoice entries[UMI_LEARNING_LIBRARY_CHOICES_MAX];
};
UmiStatus LearningLibraryChoiceAdd(UmiLearningLibraryChoices *choices,
    const UmiLearningLibraryChoice *entry);
#endif
