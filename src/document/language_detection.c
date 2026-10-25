/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/language_detection.c
 *
 * PURPOSE:
 *   Infer common development-language identities from stable extension data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/language_detection.h"

#include "umicom/platform/path.h"
#include <stdio.h>
#include <string.h>

typedef struct LanguageDefinition {
    const char *extension;
    const char *language_id;
    const char *mime_type;
    const char *icon_name;
} LanguageDefinition;

static const LanguageDefinition DEFINITIONS[] = {
    {"c", "c", "text/x-c", "text-x-csrc-symbolic"},
    {"h", "c", "text/x-chdr", "text-x-chdr-symbolic"},
    {"cc", "cpp", "text/x-c++", "text-x-c++src-symbolic"},
    {"cpp", "cpp", "text/x-c++", "text-x-c++src-symbolic"},
    {"hpp", "cpp", "text/x-c++hdr", "text-x-c++hdr-symbolic"},
    {"s", "asm", "text/x-asm", "text-x-generic-symbolic"},
    {"S", "asm", "text/x-asm", "text-x-generic-symbolic"},
    {"asm", "asm", "text/x-asm", "text-x-generic-symbolic"},
    {"py", "python3", "text/x-python", "text-x-python-symbolic"},
    {"js", "javascript", "application/javascript", "text-x-script-symbolic"},
    {"ts", "typescript", "application/typescript", "text-x-script-symbolic"},
    {"json", "json", "application/json", "text-x-generic-symbolic"},
    {"xml", "xml", "application/xml", "text-xml-symbolic"},
    {"html", "html", "text/html", "text-html-symbolic"},
    {"css", "css", "text/css", "text-css-symbolic"},
    {"md", "markdown", "text/markdown", "text-x-generic-symbolic"},
    {"cmake", "cmake", "text/x-cmake", "text-x-generic-symbolic"},
    {"sh", "sh", "application/x-shellscript", "text-x-script-symbolic"},
    {"ps1", "powershell", "text/x-powershell", "text-x-script-symbolic"},
    {"sql", "sql", "application/sql", "text-x-generic-symbolic"},
    {"yaml", "yaml", "application/yaml", "text-x-generic-symbolic"},
    {"yml", "yaml", "application/yaml", "text-x-generic-symbolic"}
};

/*
 * Provide the document language detect operation used by this module and its client
 * applications.
 */
/* Preserve the extension-only implementation for review. Filename identities
 * and bounded basename matching below let project files use the same language
 * contracts as ordinary source files, without inspecting their contents. */
#if 0
UmiStatus umi_document_language_detect(
    const char *path_or_name,
    UmiDocumentLanguageIdentity *out_identity)
{
    const char *extension;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path_or_name == NULL || out_identity == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_identity, 0, sizeof(*out_identity));
    (void)snprintf(out_identity->language_id, sizeof(out_identity->language_id), "%s", "plain");
    (void)snprintf(out_identity->mime_type, sizeof(out_identity->mime_type), "%s", "text/plain");
    (void)snprintf(out_identity->icon_name, sizeof(out_identity->icon_name), "%s", "text-x-generic-symbolic");
    out_identity->text = 1;
    extension = strrchr(path_or_name, '.');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (extension == NULL || extension[1] == '\0') return UMI_STATUS_OK;
    extension += 1U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(DEFINITIONS) / sizeof(DEFINITIONS[0]); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(DEFINITIONS[index].extension, extension) == 0) {
            (void)snprintf(out_identity->language_id, sizeof(out_identity->language_id),
                           "%s", DEFINITIONS[index].language_id);
            (void)snprintf(out_identity->mime_type, sizeof(out_identity->mime_type),
                           "%s", DEFINITIONS[index].mime_type);
            (void)snprintf(out_identity->icon_name, sizeof(out_identity->icon_name),
                           "%s", DEFINITIONS[index].icon_name);
            break;
        }
    }
    return UMI_STATUS_OK;
}
#endif

static int LanguageAsciiEqual(const char *a,const char *b)
{
    for(;*a!='\0'&&*b!='\0';++a,++b) {
        unsigned char x=(unsigned char)*a,y=(unsigned char)*b;
        if(x>='A'&&x<='Z')x=(unsigned char)(x+('a'-'A'));
        if(y>='A'&&y<='Z')y=(unsigned char)(y+('a'-'A'));
        if(x!=y)return 0;
    }
    return *a==*b;
}
UmiStatus umi_document_language_detect(const char *path_or_name,UmiDocumentLanguageIdentity *out)
{
    if(path_or_name==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    size_t length=0U;while(length<UMI_PATH_CAPACITY&&path_or_name[length]!='\0')++length;
    if(length==UMI_PATH_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *base=path_or_name;
    for(size_t i=0U;i<length;++i)if(path_or_name[i]=='/'||path_or_name[i]=='\\')base=path_or_name+i+1U;
    UmiDocumentLanguageIdentity next={0};
    (void)snprintf(next.language_id,sizeof next.language_id,"%s","plain");
    (void)snprintf(next.mime_type,sizeof next.mime_type,"%s","text/plain");
    (void)snprintf(next.icon_name,sizeof next.icon_name,"%s","text-x-generic-symbolic");
    next.text=1;
    const char *special=NULL;
    if(!strcmp(base,"CMakeLists.txt"))special="cmake";
    else if(!strcmp(base,"Makefile")||!strcmp(base,"makefile")||!strcmp(base,"GNUmakefile"))special="makefile";
    else if(!strcmp(base,".bashrc")||!strcmp(base,".bash_profile")||!strcmp(base,".profile")||!strcmp(base,".zshrc"))special="sh";
    if(special!=NULL){(void)snprintf(next.language_id,sizeof next.language_id,"%s",special);*out=next;return UMI_STATUS_OK;}
    static const LanguageDefinition additional[]={
    {"cxx", "cpp", "text/plain", "text-x-generic-symbolic"},
    {"hxx", "cpp", "text/plain", "text-x-generic-symbolic"},
    {"C", "cpp", "text/plain", "text-x-generic-symbolic"},
    {"cs", "c-sharp", "text/plain", "text-x-generic-symbolic"},
    {"java", "java", "text/plain", "text-x-generic-symbolic"},
    {"rs", "rust", "text/plain", "text-x-generic-symbolic"},
    {"go", "go", "text/plain", "text-x-generic-symbolic"},
    {"kt", "kotlin", "text/plain", "text-x-generic-symbolic"},
    {"kts", "kotlin", "text/plain", "text-x-generic-symbolic"},
    {"swift", "swift", "text/plain", "text-x-generic-symbolic"},
    {"jsonc", "jsonc", "text/plain", "text-x-generic-symbolic"},
    {"pyi", "python3", "text/plain", "text-x-generic-symbolic"},
    {"rb", "ruby", "text/plain", "text-x-generic-symbolic"},
    {"pl", "perl", "text/plain", "text-x-generic-symbolic"},
    {"r", "r", "text/plain", "text-x-generic-symbolic"},
    {"R", "r", "text/plain", "text-x-generic-symbolic"},
    {"lua", "lua", "text/plain", "text-x-generic-symbolic"},
    {"toml", "toml", "text/plain", "text-x-generic-symbolic"},
    {"ini", "ini", "text/plain", "text-x-generic-symbolic"},
    };
    const char *extension=strrchr(base,'.');
    if(extension!=NULL&&extension[1]!='\0') {
        ++extension;const LanguageDefinition *found=NULL;
        /* Exact matching preserves conventional .C and .S identities before
         * case-insensitive fallback for otherwise known suffixes. */
        for(size_t i=0U;i<sizeof additional/sizeof additional[0];++i)
            if(!strcmp(extension,additional[i].extension)){found=&additional[i];break;}
        for(size_t i=0U;found==NULL&&i<sizeof DEFINITIONS/sizeof DEFINITIONS[0];++i)
            if(!strcmp(extension,DEFINITIONS[i].extension))found=&DEFINITIONS[i];
        for(size_t i=0U;found==NULL&&i<sizeof DEFINITIONS/sizeof DEFINITIONS[0];++i)
            if(LanguageAsciiEqual(extension,DEFINITIONS[i].extension))found=&DEFINITIONS[i];
        for(size_t i=0U;found==NULL&&i<sizeof additional/sizeof additional[0];++i)
            if(LanguageAsciiEqual(extension,additional[i].extension))found=&additional[i];
        if(found!=NULL) {
            (void)snprintf(next.language_id,sizeof next.language_id,"%s",found->language_id);
            (void)snprintf(next.mime_type,sizeof next.mime_type,"%s",found->mime_type);
            (void)snprintf(next.icon_name,sizeof next.icon_name,"%s",found->icon_name);
        }
    }
    *out=next;return UMI_STATUS_OK;
}
