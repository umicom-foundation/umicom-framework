/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_pack_internal.h
 *
 * PURPOSE:
 *   Keep immutable learning-pack storage private to Framework implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LEARNING_PACK_INTERNAL_H
#define UMICOM_LEARNING_PACK_INTERNAL_H
#include "umicom/teacher/learning_pack.h"
#include "umicom/platform/path.h"
struct UmiLearningPack {
    char root[UMI_PATH_CAPACITY];
    UmiLearningPackInfo info;
    UmiLearningPackResource *resources;
};
#endif
