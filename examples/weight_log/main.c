/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/weight_log/main.c
 *
 * PURPOSE:
 *   Build a synthetic observation using shared native services and no platform UI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/weight_log.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiStructuredForm *form=malloc(sizeof(*form));
    if(form==NULL)return 1;
    const UmiStructuredFormSchema *schema=umi_weight_log_schema();
    UmiStatus status=umi_structured_form_init(schema,form);
    UmiFormAnswer date={0},mass={0};
    uint32_t grams=0U;
    if(status==UMI_STATUS_OK)
        status=umi_weight_log_parse_kilograms("80.250",6U,&grams);
    date.state=mass.state=UMI_FORM_KNOWN;
    if(status==UMI_STATUS_OK)
        status=umi_application_experience_value_text(&date.value,"2024-02-29");
    if(status==UMI_STATUS_OK)
        status=umi_application_experience_value_integer(&mass.value,(int64_t)grams);
    memcpy(mass.unit_id,"mass.g",7U);
    if(status==UMI_STATUS_OK)
        status=umi_structured_form_set(schema,form,form->revision,"weight.observed_on",&date);
    if(status==UMI_STATUS_OK)
        status=umi_structured_form_set(schema,form,form->revision,"weight.mass_grams",&mass);
    UmiWeightLogEntry entry;
    if(status==UMI_STATUS_OK)status=umi_weight_log_read(form,&entry);
    /* Only this known synthetic fixture is printed. Personal observations,
     * request bodies and notes must not become ordinary diagnostic output. */
    if(status==UMI_STATUS_OK)
        printf("Synthetic observation: %s, %" PRIu32 " g\n",entry.observed_on,entry.mass_grams);
    else fprintf(stderr,"Observation refused: %s\n",umi_status_text(status));
    free(form);
    return status==UMI_STATUS_OK?0:1;
}
