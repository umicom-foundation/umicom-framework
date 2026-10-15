/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_object_section.c
 *
 * PURPOSE:
 *   Regression coverage for describe object-file sections, flags, alignments and planned byte sizes before backend encoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/object_section.h"
#include "../value_archive/construction_cases.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted object section value. */
static int UmiNativeObjectSectionConstructionCases(void)
{
    UmiNativeObjectSection *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_nc_object_section_init(value, ".text", UMI_NC_SECTION_TEXT, 16U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_object_section_init(value, oversized, UMI_NC_SECTION_TEXT, 16U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_nc_object_section_init(value, value->name, UMI_NC_SECTION_TEXT, 16U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, ".text") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiNativeObjectSectionConstructionCases() != 0) return 1;
UmiNativeObjectSection s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_object_section_init(&s,".text",UMI_NC_SECTION_TEXT,16U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_object_section_reserve(&s,17U)!=UMI_STATUS_OK||umi_nc_object_section_aligned_size(&s)!=32U||!s.executable)return 2;return 0;}
