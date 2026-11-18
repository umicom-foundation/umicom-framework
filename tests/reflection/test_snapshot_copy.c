/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/reflection/test_snapshot_copy.c
 *
 * PURPOSE:
 *   Check pointer rebasing, independent lifetime, canonical spans and failure-atomic snapshot copies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/reflection/registry.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while(0)

/* Static test storage keeps the large catalogue snapshots off a small thread
 * stack. Each test process owns its fixtures; no library allocation is hidden. */
static UmiReflectionTypeSnapshot source, destination, before;
static void Initialize(size_t fields,size_t attributes)
{
    memset(&source,0,sizeof source);
    source.descriptor.structure_size=(uint32_t)sizeof source.descriptor;
    source.descriptor.schema_version=1U;
    strcpy(source.descriptor.type_id,"example.snapshot");
    strcpy(source.descriptor.module_id,"example.module");
    strcpy(source.descriptor.display_name,"Example");
    source.descriptor.kind=UMI_REFLECTION_TYPE_STRUCT;
    source.descriptor.size=UMI_REFLECTION_MAX_FIELDS;
    source.descriptor.alignment=1U;
    source.descriptor.field_count=fields;
    source.descriptor.attribute_count=attributes;
    source.descriptor.fields=fields?source.fields:NULL;
    source.descriptor.attributes=attributes?source.attributes:NULL;
    for(size_t i=0U;i<fields;++i) {
        UmiReflectionFieldDescriptor *f=&source.fields[i];
        snprintf(f->field_id,sizeof f->field_id,"field.%zu",i);
        strcpy(f->display_name,"Value");strcpy(f->type_id,"c.byte");
        f->value_kind=UMI_REFLECTION_VALUE_BYTES;
        f->offset=i;f->size=1U;f->element_count=1U;
    }
    for(size_t i=0U;i<attributes;++i) {
        snprintf(source.attributes[i].key,sizeof source.attributes[i].key,"attribute.%zu",i);
        strcpy(source.attributes[i].value,"original");
    }
}
static int Zero(const void *data,size_t count)
{
    const unsigned char *bytes=data;
    for(size_t i=0U;i<count;++i)if(bytes[i])return 0;
    return 1;
}
static int Refused(UmiStatus expected)
{
    memset(&destination,0xA5,sizeof destination);
    memcpy(&before,&destination,sizeof destination);
    CHECK(umi_reflection_type_snapshot_copy(&source,&destination)==expected);
    CHECK(!memcmp(&destination,&before,sizeof destination));
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    Initialize(1U,1U);
    if(!strcmp(mode,"reference") || !strcmp(mode,"lifetime")) {
        CHECK(umi_reflection_type_snapshot_copy(&source,&destination)==UMI_STATUS_OK);
        CHECK(destination.descriptor.fields==destination.fields);
        CHECK(destination.descriptor.attributes==destination.attributes);
        CHECK(!memcmp(&destination.fields[0],&source.fields[0],sizeof source.fields[0]));
        CHECK(!memcmp(&destination.attributes[0],&source.attributes[0],sizeof source.attributes[0]));
        if(!strcmp(mode,"lifetime")) {
            memset(&source,0,sizeof source);
            CHECK(!strcmp(destination.descriptor.fields[0].field_id,"field.0"));
            CHECK(!strcmp(destination.descriptor.attributes[0].value,"original"));
            CHECK(umi_reflection_type_descriptor_validate(&destination.descriptor)==UMI_STATUS_OK);
        }
    } else if(!strcmp(mode,"empty")) {
        for(size_t f=0U;f<=1U;++f)for(size_t a=0U;a<=1U;++a) {
            Initialize(f,a);
            CHECK(umi_reflection_type_snapshot_copy(&source,&destination)==UMI_STATUS_OK);
            CHECK(destination.descriptor.fields==(f?destination.fields:NULL));
            CHECK(destination.descriptor.attributes==(a?destination.attributes:NULL));
        }
    } else if(!strcmp(mode,"refusals")) {
        for(unsigned c=0U;c<9U;++c) {
            Initialize(1U,1U);
            if(c==0U)source.descriptor.fields=NULL;
            if(c==1U)source.descriptor.fields=source.fields+1;
            if(c==2U)source.descriptor.attributes=source.attributes+1;
            if(c==3U)source.descriptor.field_count=0U;
            if(c==4U)source.descriptor.attribute_count=0U;
            if(c==5U)source.descriptor.field_count=UMI_REFLECTION_MAX_FIELDS+1U;
            if(c==6U)source.descriptor.attribute_count=UMI_REFLECTION_MAX_ATTRIBUTES+1U;
            if(c==7U)source.fields[0].offset=source.descriptor.size;
            if(c==8U)memset(source.descriptor.display_name,'x',sizeof source.descriptor.display_name);
            CHECK(!Refused(UMI_STATUS_INVALID_ARGUMENT));
        }
        Initialize(2U,1U);strcpy(source.fields[1].field_id,"field.0");
        CHECK(!Refused(UMI_STATUS_ALREADY_EXISTS));
        memcpy(&before,&destination,sizeof destination);
        CHECK(umi_reflection_type_snapshot_copy(NULL,&destination)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&before,&destination,sizeof destination));
        CHECK(umi_reflection_type_snapshot_copy(&source,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(mode,"self")) {
        memcpy(&before,&source,sizeof source);
        CHECK(umi_reflection_type_snapshot_copy(&source,&source)==UMI_STATUS_OK);
        CHECK(!memcmp(&before,&source,sizeof source));
        source.descriptor.attributes=NULL;
        memcpy(&before,&source,sizeof source);
        CHECK(umi_reflection_type_snapshot_copy(&source,&source)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&before,&source,sizeof source));
    } else if(!strcmp(mode,"maximum")) {
        Initialize(UMI_REFLECTION_MAX_FIELDS,UMI_REFLECTION_MAX_ATTRIBUTES);
        CHECK(umi_reflection_type_snapshot_copy(&source,&destination)==UMI_STATUS_OK);
        CHECK(destination.descriptor.fields==destination.fields);
        CHECK(destination.descriptor.attributes==destination.attributes);
        CHECK(!memcmp(destination.fields,source.fields,sizeof source.fields));
        CHECK(!memcmp(destination.attributes,source.attributes,sizeof source.attributes));
    } else if(!strcmp(mode,"unused")) {
        memset(source.fields+1,0xD3,sizeof source.fields-sizeof source.fields[0]);
        memset(source.attributes+1,0xD4,sizeof source.attributes-sizeof source.attributes[0]);
        memset(&destination,0xA5,sizeof destination);
        CHECK(umi_reflection_type_snapshot_copy(&source,&destination)==UMI_STATUS_OK);
        CHECK(Zero(destination.fields+1,sizeof destination.fields-sizeof destination.fields[0]));
        CHECK(Zero(destination.attributes+1,sizeof destination.attributes-sizeof destination.attributes[0]));
    } else return 2;
    return 0;
}
