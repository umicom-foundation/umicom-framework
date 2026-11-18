/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/reflection/test_resource_media_type.c
 *
 * PURPOSE:
 *   Verify bounded media-type resource metadata, legacy identifiers and unchanged registry state on refusal.
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
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"media metadata check failed: %d\n",__LINE__); return 1; } } while (0)
static UmiReflectionResourceDescriptor Resource(const char *media)
{
    UmiReflectionResourceDescriptor d={0};
    d.structure_size=(uint32_t)sizeof d;
    strcpy(d.resource_id,"lesson.preview"); strcpy(d.module_id,"lesson.media");
    strcpy(d.display_name,"Preview"); strcpy(d.resource_uri,"memory://lesson/preview");
    d.kind=UMI_REFLECTION_RESOURCE_DOCUMENTATION;
    snprintf(d.media_type,sizeof d.media_type,"%s",media);
    return d;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);
    const char *mode=argv[1];
    if(!strcmp(mode,"legacy") || !strcmp(mode,"standard") || !strcmp(mode,"suffix")) {
        const char *legacy[]={"umicom.layout","text","c_uint32","legacy-42"};
        const char *standard[]={"application/json","text/plain","image/svg+xml","video/mp4","audio/ogg","Application/JSON"};
        const char *suffix[]={"application/vnd.example.lesson+json","application/x.lesson+xml","x-example/a!b#c$d&e-f^g_h.i+j","a/0"};
        const char **items=legacy;size_t count=sizeof legacy/sizeof legacy[0];
        if(!strcmp(mode,"standard")){items=standard;count=sizeof standard/sizeof standard[0];}
        if(!strcmp(mode,"suffix")){items=suffix;count=sizeof suffix/sizeof suffix[0];}
        for(size_t i=0;i<count;++i){UmiReflectionResourceDescriptor d=Resource(items[i]);CHECK(umi_reflection_resource_descriptor_validate(&d)==UMI_STATUS_OK);}
    } else if(!strcmp(mode,"invalid")) {
        const char *items[]={"","/","/json","image/","image//png","image/png/extra","*/png","image/*","image/.png","-image/png","image/p ng","image/png; charset=utf-8"," image/png","image/png ","image/p%ng","image/p'ng","image/p(ng","image/p:ng","image/p=ng","image/p?ng","image/p@ng","image/p\\ng","image/p[ng","image/p{ng","image/png\r\nX: injected"};
        for(size_t i=0;i<sizeof items/sizeof items[0];++i){UmiReflectionResourceDescriptor d=Resource(items[i]);CHECK(umi_reflection_resource_descriptor_validate(&d)==UMI_STATUS_INVALID_ARGUMENT);}
    } else if(!strcmp(mode,"capacity")) {
        UmiReflectionResourceDescriptor d=Resource("a/b");
        memset(d.media_type+2,'b',sizeof d.media_type-3U);d.media_type[sizeof d.media_type-1U]='\0';
        CHECK(strlen(d.media_type)==127U);CHECK(umi_reflection_resource_descriptor_validate(&d)==UMI_STATUS_OK);
        d.media_type[127]='b';CHECK(umi_reflection_resource_descriptor_validate(&d)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(mode,"bytes")) {
        /* Every forbidden control and high byte is tested inside a type/subtype.
         * The metadata API admits ASCII names, not encoded display text. */
        for(unsigned b=1U;b<256U;++b) if(b<33U || b>=127U) {
            UmiReflectionResourceDescriptor d=Resource("image/png");d.media_type[7]=(char)(unsigned char)b;
            CHECK(umi_reflection_resource_descriptor_validate(&d)==UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if(!strcmp(mode,"registry")) {
        UmiReflectionRegistry *registry=NULL;UmiReflectionRegistryConfig config=umi_reflection_registry_config_default();
        config.type_capacity=1U;config.module_capacity=1U;config.resource_capacity=2U;
        CHECK(umi_reflection_registry_create(&config,&registry)==UMI_STATUS_OK);
        UmiReflectionResourceDescriptor d=Resource("image/svg+xml"),out;
        UmiStatus status=umi_reflection_registry_register_resource(registry,&d);
        if(status!=UMI_STATUS_OK){umi_reflection_registry_destroy(registry);CHECK(status==UMI_STATUS_OK);}
        strcpy(d.media_type,"changed");
        status=umi_reflection_registry_get_resource(registry,"lesson.preview",&out);
        int copied=status==UMI_STATUS_OK && !strcmp(out.media_type,"image/svg+xml");
        strcpy(d.media_type,"image/png;invalid");int replaced=71;
        status=umi_reflection_registry_upsert_resource(registry,&d,&replaced);
        int refused=status==UMI_STATUS_INVALID_ARGUMENT;
        status=umi_reflection_registry_get_resource(registry,"lesson.preview",&out);
        int preserved=status==UMI_STATUS_OK && !strcmp(out.media_type,"image/svg+xml");
        umi_reflection_registry_destroy(registry);
        CHECK(copied && refused && preserved);
    } else if(!strcmp(mode,"unchanged")) {
        UmiReflectionResourceDescriptor d=Resource("application/json"),before=d;
        CHECK(umi_reflection_resource_descriptor_validate(&d)==UMI_STATUS_OK);
        CHECK(!memcmp(&d,&before,sizeof d));
        strcpy(d.media_type,"text/plain;param=1");before=d;
        CHECK(umi_reflection_resource_descriptor_validate(&d)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&d,&before,sizeof d));
    } else return 2;
    return 0;
}
