/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/reflection/test_empty_spans.c
 *
 * PURPOSE:
 *   Verify empty reflection spans do not retain borrowed addresses and nonempty snapshots own their data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/reflection/reflection.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); result=1; goto cleanup; } } while(0)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    int result=0;
    UmiReflectionRegistry *registry=NULL;
    UmiReflectionFieldDescriptor field={0};
    UmiReflectionAttribute attribute={0};
    UmiReflectionTypeDescriptor type={0};
    UmiReflectionTypeSnapshot snapshot;
    const char *mode=argv[1];
    int has_fields=!strcmp(mode,"fields") || !strcmp(mode,"both") || !strcmp(mode,"lifetime");
    int has_attributes=!strcmp(mode,"attributes") || !strcmp(mode,"both") || !strcmp(mode,"lifetime");
    if(!has_fields && !has_attributes && strcmp(mode,"empty"))return 2;
    UmiReflectionRegistryConfig config=umi_reflection_registry_config_default();
    config.type_capacity=2U;config.module_capacity=1U;config.resource_capacity=1U;
    CHECK(umi_reflection_registry_create(&config,&registry)==UMI_STATUS_OK);
    type.structure_size=(uint32_t)sizeof type;type.schema_version=1U;
    type.kind=UMI_REFLECTION_TYPE_STRUCT;type.size=sizeof(uint32_t);
    type.alignment=_Alignof(uint32_t);
    strcpy(type.type_id,"lesson.record");strcpy(type.module_id,"lesson.module");
    strcpy(type.display_name,"Owned record");
    strcpy(field.field_id,"value");strcpy(field.display_name,"Value");strcpy(field.type_id,"c.uint32");
    field.value_kind=UMI_REFLECTION_VALUE_UNSIGNED_INTEGER;
    field.size=sizeof(uint32_t);field.element_count=1U;
    strcpy(attribute.key,"purpose");strcpy(attribute.value,"original");
    /* Empty spans may have nonnull input pointers. The owned descriptor must
     * not retain those addresses merely because it needs no allocation. */
    type.fields=&field;type.field_count=has_fields?1U:0U;
    type.attributes=&attribute;type.attribute_count=has_attributes?1U:0U;
    CHECK(umi_reflection_registry_register_type(registry,&type)==UMI_STATUS_OK);
    strcpy(field.display_name,"Changed source");strcpy(attribute.value,"changed source");
    CHECK(umi_reflection_registry_get_type(registry,"lesson.record",&snapshot)==UMI_STATUS_OK);
    CHECK(snapshot.descriptor.field_count==type.field_count);
    CHECK(snapshot.descriptor.attribute_count==type.attribute_count);
    if(has_fields) {
        CHECK(snapshot.descriptor.fields==snapshot.fields);
        CHECK(!strcmp(snapshot.fields[0].display_name,"Value"));
    } else CHECK(snapshot.descriptor.fields==NULL);
    if(has_attributes) {
        CHECK(snapshot.descriptor.attributes==snapshot.attributes);
        CHECK(!strcmp(snapshot.attributes[0].value,"original"));
    } else CHECK(snapshot.descriptor.attributes==NULL);
    if(!strcmp(mode,"lifetime")) {
        CHECK(umi_reflection_registry_unregister_type(registry,"lesson.record")==UMI_STATUS_OK);
        umi_reflection_registry_destroy(registry);registry=NULL;
        /* Embedded pointers refer to this snapshot, not an arbitrary by-value
         * copy. Its owned arrays survive unregister and registry destruction. */
        CHECK(!strcmp(snapshot.descriptor.fields[0].display_name,"Value"));
        CHECK(!strcmp(snapshot.descriptor.attributes[0].value,"original"));
    }
cleanup:
    umi_reflection_registry_destroy(registry);
    return result;
}
