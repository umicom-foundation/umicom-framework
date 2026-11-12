/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_weight_log.c
 *
 * PURPOSE:
 *   Check exact input, calendar and unit admission without changing a failed output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/weight_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c); return 1; } } while (0)

static UmiStatus put_text(UmiStructuredForm *form, const char *field, const char *text)
{
    UmiFormAnswer answer={0};
    answer.state=UMI_FORM_KNOWN;
    UmiStatus status=umi_application_experience_value_text(&answer.value,text);
    if(status!=UMI_STATUS_OK)return status;
    return umi_structured_form_set(umi_weight_log_schema(),form,form->revision,field,&answer);
}
static UmiStatus put_mass(UmiStructuredForm *form, int64_t grams, const char *unit)
{
    UmiFormAnswer answer={0};
    answer.state=UMI_FORM_KNOWN;
    UmiStatus status=umi_application_experience_value_integer(&answer.value,grams);
    if(status!=UMI_STATUS_OK)return status;
    if(strlen(unit)>=sizeof(answer.unit_id))return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(answer.unit_id,unit,strlen(unit)+1U);
    return umi_structured_form_set(umi_weight_log_schema(),form,form->revision,
                                    "weight.mass_grams",&answer);
}
int main(int argc, char **argv)
{
    CHECK(argc==2);
    if(strcmp(argv[1],"decimal")==0) {
        static const struct {const char *text; uint32_t grams;} cases[]={
            {"80",80000U},{"80.2",80200U},{"80.25",80250U},{"80.250",80250U},
            {"0.001",1U},{"0000080.250",80250U},{"1000000.000",UMI_WEIGHT_LOG_MAX_GRAMS}
        };
        for(size_t i=0U;i<sizeof(cases)/sizeof(cases[0]);++i){
            uint32_t grams=19U;
            CHECK(umi_weight_log_parse_kilograms(cases[i].text,strlen(cases[i].text),&grams)==UMI_STATUS_OK);
            CHECK(grams==cases[i].grams);
        }
        const char bounded[]={'8','0','.','2','5','0','x'};
        uint32_t grams=0U;
        CHECK(umi_weight_log_parse_kilograms(bounded,6U,&grams)==UMI_STATUS_OK&&grams==80250U);
    } else if(strcmp(argv[1],"syntax")==0) {
        static const char *cases[]={"","+80","-80"," 80","80 ","80,2","8e1",".5","80.","80.0001","80kg","00000001"};
        for(size_t i=0U;i<sizeof(cases)/sizeof(cases[0]);++i){
            uint32_t grams=19U;
            CHECK(umi_weight_log_parse_kilograms(cases[i],strlen(cases[i]),&grams)==UMI_STATUS_PARSE_ERROR);
            CHECK(grams==19U);
        }
        const char embedded[]={'8','0',0,'1'};
        uint32_t grams=19U;
        CHECK(umi_weight_log_parse_kilograms(embedded,sizeof(embedded),&grams)==UMI_STATUS_PARSE_ERROR&&grams==19U);
        CHECK(umi_weight_log_parse_kilograms(NULL,1U,&grams)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_weight_log_parse_kilograms("1",1U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(strcmp(argv[1],"range")==0) {
        static const char *cases[]={"0","0.000","1000000.001","9999999.999"};
        for(size_t i=0U;i<sizeof(cases)/sizeof(cases[0]);++i){
            uint32_t grams=19U;
            CHECK(umi_weight_log_parse_kilograms(cases[i],strlen(cases[i]),&grams)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(grams==19U);
        }
    } else {
        UmiStructuredForm *form=malloc(sizeof(*form));
        CHECK(form!=NULL);
        CHECK(umi_structured_form_init(umi_weight_log_schema(),form)==UMI_STATUS_OK);
        UmiWeightLogEntry out, before;
        memset(&out,0x5a,sizeof(out));
        memcpy(&before,&out,sizeof(out));
        CHECK(umi_weight_log_read(form,&out)==UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&before,&out,sizeof(out))==0);
        CHECK(put_text(form,"weight.observed_on","2024-02-29")==UMI_STATUS_OK);
        CHECK(put_mass(form,80250,"mass.g")==UMI_STATUS_OK);
        if(strcmp(argv[1],"entry")==0){
            CHECK(umi_weight_log_read(form,&out)==UMI_STATUS_OK);
            CHECK(strcmp(out.observed_on,"2024-02-29")==0&&out.mass_grams==80250U&&out.note[0]==0);
            CHECK(put_text(form,"weight.note","Synthetic morning fixture")==UMI_STATUS_OK);
            CHECK(umi_weight_log_read(form,&out)==UMI_STATUS_OK);
            CHECK(strcmp(out.note,"Synthetic morning fixture")==0);
            CHECK(form->saved_revision==1U);
        } else if(strcmp(argv[1],"refusal")==0){
            uint64_t revision=form->revision;
            CHECK(put_text(form,"weight.observed_on","2023-02-29")!=UMI_STATUS_OK);
            CHECK(put_mass(form,80250,"mass.kg")!=UMI_STATUS_OK);
            CHECK(put_mass(form,0,"mass.g")!=UMI_STATUS_OK);
            CHECK(put_mass(form,1000000001,"mass.g")!=UMI_STATUS_OK);
            CHECK(form->revision==revision);
            form->schema_version++;
            CHECK(umi_weight_log_read(form,&out)!=UMI_STATUS_OK);
            CHECK(memcmp(&before,&out,sizeof(out))==0);
        } else if(strcmp(argv[1],"note")==0){
            char note[257];
            memset(note,'a',255U); note[255]=0;
            CHECK(put_text(form,"weight.note",note)==UMI_STATUS_OK);
            CHECK(umi_weight_log_read(form,&out)==UMI_STATUS_OK&&strlen(out.note)==255U);
            note[255]='a'; note[256]=0;
            uint64_t revision=form->revision;
            CHECK(put_text(form,"weight.note",note)!=UMI_STATUS_OK&&form->revision==revision);
        } else { free(form); CHECK(0); }
        free(form);
    }
    return 0;
}
