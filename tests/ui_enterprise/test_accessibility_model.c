/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_accessibility_model.c
 *
 * PURPOSE:
 *   Exercise the accessibility model enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/enterprise/accessibility_model.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/accessibility_model.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntAccessibilityModelTransferEqual(const UmiUiEntAccessibilityModel *a, const UmiUiEntAccessibilityModel *b)
{
    return strcmp(a->role, b->role) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        strcmp(a->description, b->description) == 0 &&
        a->row_index == b->row_index &&
        a->column_index == b->column_index &&
        a->set_size == b->set_size &&
        a->position_in_set == b->position_in_set;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntAccessibilityModelTransferTails(UmiUiEntAccessibilityModel *value)
{
    (void)value;
    {
        size_t used = strlen(value->role) + 1U;
        memset(value->role + used, 0xa5, sizeof(value->role) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->description) + 1U;
        memset(value->description + used, 0xa5, sizeof(value->description) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntAccessibilityModelTransferMalformed(const UmiUiEntAccessibilityModel *sample)
{
    (void)sample;
    {
        UmiUiEntAccessibilityModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.role, 'x', sizeof(invalid.role));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_accessibility_model_validate(&invalid)) ||
            umi_ui_ent_accessibility_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated role was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntAccessibilityModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_accessibility_model_validate(&invalid)) ||
            umi_ui_ent_accessibility_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntAccessibilityModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.description, 'x', sizeof(invalid.description));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_accessibility_model_validate(&invalid)) ||
            umi_ui_ent_accessibility_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated description was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntAccessibilityModelTransferCases, UmiUiEntAccessibilityModel,
    umi_ui_ent_accessibility_model_archive_encode, umi_ui_ent_accessibility_model_archive_decode,
    UmiUiEntAccessibilityModelTransferEqual, UmiUiEntAccessibilityModelTransferTails, UmiUiEntAccessibilityModelTransferMalformed)

int main(void){UmiUiEntAccessibilityModel v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_accessibility_model_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.role,sizeof v.role,"id")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.name,sizeof v.name,"Row")!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_accessibility_model_validate(&v))return 9;
    if (UmiUiEntAccessibilityModelTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
