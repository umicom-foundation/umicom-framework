/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/browser/test_reading_images.c
 *
 * PURPOSE:
 *   Verify inert image metadata, complete target ownership and preserved alternative text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/browser/reading_document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#value); failed=1; goto cleanup; } } while (0)
typedef struct ImageCase {
    const char *name, *html, *target;
    UmiStatus target_status;
    int missing, decorative;
} ImageCase;
static const ImageCase cases[] = {
    {"local","<img src='diagram.png' alt='Owned pixels'>","diagram.png",UMI_STATUS_OK,0,0},
    {"remote","<img src='https://invalid.example/image' alt='Remote'>","https://invalid.example/image",UMI_STATUS_OK,0,0},
    {"entity","<img src='diagram&#46;png' alt='A &amp; B'>","diagram.png",UMI_STATUS_OK,0,0},
    {"missing","<img alt='No source'>","",UMI_STATUS_NOT_FOUND,0,0},
    {"empty","<img src='' alt='No source'>","",UMI_STATUS_NOT_FOUND,0,0},
    {"decorative","<img src='diagram.png' alt=''>","diagram.png",UMI_STATUS_OK,0,1},
    {"missing-alt","<img src='diagram.png'>","diagram.png",UMI_STATUS_OK,1,0},
    {"duplicate-source","<img src='first.png' SRC='second.png' alt='Ambiguous'>","",UMI_STATUS_INVALID_ARGUMENT,0,0},
    {"invalid-entity","<img src='bad&#0;.png' alt='Readable'>","",UMI_STATUS_INVALID_ARGUMENT,0,0},
    {"owned-metadata","<p>Before</p><img src='diagram.png' alt='Owned'>","diagram.png",UMI_STATUS_OK,0,0},
    {"range","<p>\xce\xa9 before</p><img src='diagram.png' alt='\xce\xa9 diagram'>","diagram.png",UMI_STATUS_OK,0,0}
};
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    int failed=0,matched=0;
    char *storage=NULL;
    UmiBrowserReadingDocument *document=NULL;
    UmiBrowserReadingSummary summary;
    UmiBrowserReadingImage image={0};
    for(size_t i=0U;i<sizeof(cases)/sizeof(cases[0]);++i) {
        const ImageCase *item=&cases[i];if(strcmp(argv[1],item->name)!=0)continue;
        matched=1;size_t bytes=strlen(item->html);
        storage=malloc(bytes+1U);CHECK(storage!=NULL);memcpy(storage,item->html,bytes+1U);
        CHECK(UmiBrowserReadingDocumentCreate(storage,bytes,&document)==UMI_STATUS_OK);
        memset(storage,'x',bytes);free(storage);storage=NULL;
        CHECK(UmiBrowserReadingDocumentSummary(document,&summary)==UMI_STATUS_OK && summary.image_descriptions==1U);
        CHECK(UmiBrowserReadingDocumentImageAt(document,0U,&image)==UMI_STATUS_OK);
        CHECK(image.target_status==item->target_status && strcmp(image.target,item->target)==0);
        CHECK(image.description_missing==item->missing && image.decorative==item->decorative);
        CHECK(image.byte_start<=summary.text_bytes && image.byte_length<=summary.text_bytes-image.byte_start);
        const char *text=UmiBrowserReadingDocumentText(document);
        CHECK(strncmp(text+image.byte_start,"[Image: ",8U)==0 && text[image.byte_start+image.byte_length-1U]==']');
        if(strcmp(item->name,"range")==0)CHECK(image.byte_start==(size_t)(strstr(text,"[Image:")-text));
        UmiBrowserReadingDocumentDestroy(document);document=NULL;
        CHECK(strcmp(image.target,item->target)==0); /* The copied target has independent storage. */
        goto cleanup;
    }
    if(strcmp(argv[1],"oversized")==0) {
        matched=1;storage=malloc(4096U);CHECK(storage!=NULL);
        strcpy(storage,"<img src='");size_t offset=strlen(storage);
        memset(storage+offset,'a',UMI_BROWSER_READING_TARGET_CAPACITY);
        strcpy(storage+offset+UMI_BROWSER_READING_TARGET_CAPACITY,"' alt='Still readable'>");
        CHECK(UmiBrowserReadingDocumentCreate(storage,strlen(storage),&document)==UMI_STATUS_OK);
        CHECK(UmiBrowserReadingDocumentImageAt(document,0U,&image)==UMI_STATUS_OK &&
            image.target_status==UMI_STATUS_CAPACITY_EXCEEDED && image.target[0]=='\0');
        CHECK(strcmp(UmiBrowserReadingDocumentText(document),"[Image: Still readable]")==0);
    } else if(strcmp(argv[1],"hidden")==0) {
        matched=1;const char *html="<div hidden><img src='hidden.png'></div><img hidden src='hidden2.png'>Visible";
        CHECK(UmiBrowserReadingDocumentCreate(html,strlen(html),&document)==UMI_STATUS_OK);
        CHECK(UmiBrowserReadingDocumentSummary(document,&summary)==UMI_STATUS_OK && summary.image_descriptions==0U);
        CHECK(UmiBrowserReadingDocumentImageAt(document,0U,&image)==UMI_STATUS_NOT_FOUND);
    } else if(strcmp(argv[1],"capacity")==0) {
        matched=1;storage=calloc(1U,32768U);CHECK(storage!=NULL);
        for(size_t i=0U;i<UMI_BROWSER_READING_SPANS_MAX;++i)strcat(storage,"<img alt=''>");
        CHECK(UmiBrowserReadingDocumentCreate(storage,strlen(storage),&document)==UMI_STATUS_OK);
        CHECK(UmiBrowserReadingDocumentImageAt(document,UMI_BROWSER_READING_SPANS_MAX-1U,&image)==UMI_STATUS_OK);
        CHECK(image.decorative && image.target_status==UMI_STATUS_NOT_FOUND);
        UmiBrowserReadingDocumentDestroy(document);document=NULL;
        strcat(storage,"<img alt=''>");
        CHECK(UmiBrowserReadingDocumentCreate(storage,strlen(storage),&document)==UMI_STATUS_CAPACITY_EXCEEDED && document==NULL);
    } else if(strcmp(argv[1],"arguments")==0) {
        matched=1;CHECK(UmiBrowserReadingDocumentCreate("text",4U,&document)==UMI_STATUS_OK);
        image.byte_start=99U;image.byte_length=88U;strcpy(image.target,"unchanged");
        CHECK(UmiBrowserReadingDocumentImageAt(NULL,0U,&image)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrowserReadingDocumentImageAt(document,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrowserReadingDocumentImageAt(document,0U,&image)==UMI_STATUS_NOT_FOUND);
        CHECK(image.byte_start==99U && image.byte_length==88U && strcmp(image.target,"unchanged")==0);
    }
    CHECK(matched);
cleanup:
    free(storage);UmiBrowserReadingDocumentDestroy(document);return failed;
}
