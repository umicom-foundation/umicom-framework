/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_form_template.c
 *
 * PURPOSE:
 *   Validate describe reusable form templates and expected field/action counts.
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
#include "umicom/designer/visual_designer/form_template.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/form_template.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadFormTemplateTransferEqual(const UmiRadFormTemplate *a, const UmiRadFormTemplate *b)
{
    return strcmp(a->template_id, b->template_id) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->field_count == b->field_count &&
        a->action_count == b->action_count;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadFormTemplateTransferTails(UmiRadFormTemplate *value)
{
    (void)value;
    {
        size_t used = strlen(value->template_id) + 1U;
        memset(value->template_id + used, 0xa5, sizeof(value->template_id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadFormTemplateTransferMalformed(const UmiRadFormTemplate *sample)
{
    (void)sample;
    {
        UmiRadFormTemplate invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.template_id, 'x', sizeof(invalid.template_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_form_template_is_valid(&invalid)) ||
            umi_rad_form_template_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated template_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadFormTemplate invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_form_template_is_valid(&invalid)) ||
            umi_rad_form_template_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadFormTemplateTransferCases, UmiRadFormTemplate,
    umi_rad_form_template_archive_encode, umi_rad_form_template_archive_decode,
    UmiRadFormTemplateTransferEqual, UmiRadFormTemplateTransferTails, UmiRadFormTemplateTransferMalformed)

int main(void){UmiRadFormTemplate item;CHECK(umi_rad_form_template_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_form_template_is_valid(&item));
    if (UmiRadFormTemplateTransferCases(&item) != 0) return 1;
return 0;}
