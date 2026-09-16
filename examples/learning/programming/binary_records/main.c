/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/binary_records/main.c
 *
 * PURPOSE:
 *   Serialisation without dumping structures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Record { uint16_t score; } Record;
static bool Decode(const unsigned char *bytes, size_t count, Record *out)
{
    if (bytes==NULL || out==NULL || count!=3U || bytes[0]!=1U) return false;
    uint16_t score=(uint16_t)(((uint16_t)bytes[1]<<8U)|(uint16_t)bytes[2]);
    if (score>100U) return false;
    *out=(Record){score}; return true;
}
int main(void)
{
    const unsigned char valid[]={1U,0U,80U}, invalid[]={1U,0U,101U}, future[]={2U,0U,80U};
    Record record={9U};
    if (!Decode(valid,sizeof(valid),&record) || record.score!=80U) return EXIT_FAILURE;
    if (Decode(invalid,sizeof(invalid),&record) || record.score!=80U ||
        Decode(future,sizeof(future),&record) || Decode(valid,2U,&record)) return EXIT_FAILURE;
    puts("version 1 record decoded; wrong version/length/range rejected");
    puts("PASS: binary_records");
    return EXIT_SUCCESS;
}
