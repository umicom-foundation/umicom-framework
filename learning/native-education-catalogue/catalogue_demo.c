/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-education-catalogue/catalogue_demo.c
 * PURPOSE: Demonstrate portable HTML navigation from illustrative entries.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_catalogue.h"
#include <stdio.h>

int main(void)
{
    const UmiEducationCourseEntry examples[] = {
        {95U, "Save a training checkpoint", "lessons/95-save-checkpoint.html"},
        {96U, "Restore an approved model", "lessons/96-restore-model.html"}
    };
    char page[4096] = {0};
    size_t length = 0U;
    UmiStatus status = UmiEducationCourseCatalogueRender(
        "Native LLM Learning", examples, 2U, "index.html",
        page, sizeof(page), &length);
    if (status != UMI_STATUS_OK) return 1;
    if (fwrite(page, 1U, length, stdout) != length) return 1;
    return 0;
}
