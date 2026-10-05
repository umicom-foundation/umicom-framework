/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_launch_receipts.c
 * PURPOSE: Check retained launch evidence without starting any process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/launch_receipts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(expression)                                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        if (!(expression))                                                                                   \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #expression);                                              \
            result = 1;                                                                                      \
            goto done;                                                                                       \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    UmiApplicationLaunchReceipts *owner = NULL;
    UmiApplicationLaunchReceipt receipt = {0}, before;
    uint64_t first = 0U, next = 0U;
    int result = 0;
    const char *test = argc > 1 ? argv[1] : "";
    CHECK(UmiApplicationLaunchReceiptsCreate(&owner) == UMI_STATUS_OK);
    CHECK(UmiApplicationLaunchReceiptsCount(owner) == 0U);
    CHECK(UmiApplicationLaunchReceiptsBegin(owner, "org.umicom.media-studio", "/suite/umicom-media", false,
                                            &first) == UMI_STATUS_OK);
    CHECK(UmiApplicationLaunchReceiptsFind(owner, first, &receipt) == UMI_STATUS_OK);
    CHECK(receipt.state == UMI_APPLICATION_LAUNCH_RECEIPT_PENDING);
    if (strcmp(test, "lifecycle") == 0)
    {
        CHECK(UmiApplicationLaunchReceiptsStarted(owner, first) == UMI_STATUS_OK);
        CHECK(UmiApplicationLaunchReceiptsExited(owner, first, 17) == UMI_STATUS_OK);
        CHECK(UmiApplicationLaunchReceiptsFind(owner, first, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.state == UMI_APPLICATION_LAUNCH_RECEIPT_EXITED && receipt.exit_code == 17);
    }
    else if (strcmp(test, "transition") == 0)
    {
        CHECK(UmiApplicationLaunchReceiptsExited(owner, first, 0) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiApplicationLaunchReceiptsStarted(owner, first) == UMI_STATUS_OK);
        CHECK(UmiApplicationLaunchReceiptsStarted(owner, first) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiApplicationLaunchReceiptsRejected(owner, first, UMI_STATUS_IO_ERROR) ==
              UMI_STATUS_INVALID_STATE);
    }
    else if (strcmp(test, "duplicate-pending") == 0 || strcmp(test, "duplicate-running") == 0 ||
             strcmp(test, "uncertain") == 0)
    {
        if (strcmp(test, "duplicate-pending") != 0)
            CHECK(UmiApplicationLaunchReceiptsStarted(owner, first) == UMI_STATUS_OK);
        if (strcmp(test, "uncertain") == 0)
            CHECK(UmiApplicationLaunchReceiptsUncertain(owner, first, UMI_STATUS_IO_ERROR) == UMI_STATUS_OK);
        next = 777U;
        CHECK(UmiApplicationLaunchReceiptsBegin(owner, "org.umicom.media-studio", "/suite/umicom-media",
                                                false, &next) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(next == 777U && UmiApplicationLaunchReceiptsCount(owner) == 1U);
    }
    else if (strcmp(test, "new-window") == 0)
    {
        CHECK(UmiApplicationLaunchReceiptsBegin(owner, "org.umicom.media-studio", "/suite/umicom-media", true,
                                                &next) == UMI_STATUS_OK);
        CHECK(next != first && UmiApplicationLaunchReceiptsCount(owner) == 2U);
    }
    else if (strcmp(test, "reopen-exit") == 0 || strcmp(test, "reopen-rejected") == 0)
    {
        if (strcmp(test, "reopen-exit") == 0)
        {
            CHECK(UmiApplicationLaunchReceiptsStarted(owner, first) == UMI_STATUS_OK);
            CHECK(UmiApplicationLaunchReceiptsExited(owner, first, 0) == UMI_STATUS_OK);
        }
        else
            CHECK(UmiApplicationLaunchReceiptsRejected(owner, first, UMI_STATUS_PERMISSION_DENIED) ==
                  UMI_STATUS_OK);
        CHECK(UmiApplicationLaunchReceiptsBegin(owner, "org.umicom.media-studio", "/suite/umicom-media",
                                                false, &next) == UMI_STATUS_OK);
        CHECK(next > first);
    }
    else if (strcmp(test, "capacity-active") == 0 || strcmp(test, "eviction") == 0)
    {
        for (size_t index = 1U; index < UMI_APPLICATION_LAUNCH_RECEIPT_CAPACITY; ++index)
            CHECK(UmiApplicationLaunchReceiptsBegin(owner, "org.umicom.media-studio", "/suite/umicom-media",
                                                    true, &next) == UMI_STATUS_OK);
        CHECK(UmiApplicationLaunchReceiptsBegin(owner, "org.umicom.media-studio", "/suite/umicom-media", true,
                                                &next) == UMI_STATUS_CAPACITY_EXCEEDED);
        if (strcmp(test, "eviction") == 0)
        {
            CHECK(UmiApplicationLaunchReceiptsRejected(owner, first, UMI_STATUS_IO_ERROR) == UMI_STATUS_OK);
            CHECK(UmiApplicationLaunchReceiptsBegin(owner, "org.umicom.music-studio", "/suite/umicom-music",
                                                    false, &next) == UMI_STATUS_OK);
            CHECK(UmiApplicationLaunchReceiptsFind(owner, first, &receipt) == UMI_STATUS_NOT_FOUND);
            CHECK(UmiApplicationLaunchReceiptsCount(owner) == UMI_APPLICATION_LAUNCH_RECEIPT_CAPACITY);
            CHECK(UmiApplicationLaunchReceiptsStarted(owner, first) == UMI_STATUS_NOT_FOUND);
        }
    }
    else if (strcmp(test, "late-exit") == 0)
    {
        CHECK(UmiApplicationLaunchReceiptsStarted(owner, first) == UMI_STATUS_OK);
        CHECK(UmiApplicationLaunchReceiptsBegin(owner, "org.umicom.media-studio", "/suite/umicom-media", true,
                                                &next) == UMI_STATUS_OK);
        CHECK(UmiApplicationLaunchReceiptsStarted(owner, next) == UMI_STATUS_OK);
        CHECK(UmiApplicationLaunchReceiptsExited(owner, first, 2) == UMI_STATUS_OK);
        CHECK(UmiApplicationLaunchReceiptsLatest(owner, "org.umicom.media-studio", &receipt) ==
              UMI_STATUS_OK);
        CHECK(receipt.id == next && receipt.state == UMI_APPLICATION_LAUNCH_RECEIPT_RUNNING);
    }
    else if (strcmp(test, "ordering") == 0)
    {
        CHECK(UmiApplicationLaunchReceiptsBegin(owner, "org.umicom.music-studio", "/suite/umicom-music",
                                                false, &next) == UMI_STATUS_OK);
        CHECK(UmiApplicationLaunchReceiptsAt(owner, 0U, &receipt) == UMI_STATUS_OK && receipt.id == next);
        CHECK(UmiApplicationLaunchReceiptsAt(owner, 1U, &receipt) == UMI_STATUS_OK && receipt.id == first);
        CHECK(UmiApplicationLaunchReceiptsAt(owner, 2U, &receipt) == UMI_STATUS_NOT_FOUND);
    }
    else if (strcmp(test, "text-bounds") == 0)
    {
        char id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
        memset(id, 'x', sizeof(id));
        CHECK(UmiApplicationLaunchReceiptsBegin(owner, id, "/path", false, &next) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationLaunchReceiptsBegin(owner, "", "/path", false, &next) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationLaunchReceiptsBegin(owner, "org.umicom.bank", "", false, &next) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationLaunchReceiptsCount(owner) == 1U);
    }
    else if (strcmp(test, "copied-text") == 0)
    {
        char id[] = "org.umicom.bank", path[] = "/suite/umicom-bank";
        CHECK(UmiApplicationLaunchReceiptsBegin(owner, id, path, false, &next) == UMI_STATUS_OK);
        id[0] = 'x';
        path[0] = 'x';
        CHECK(UmiApplicationLaunchReceiptsFind(owner, next, &receipt) == UMI_STATUS_OK);
        CHECK(strcmp(receipt.application_id, "org.umicom.bank") == 0 && receipt.executable_path[0] == '/');
    }
    else if (strcmp(test, "output-unchanged") == 0)
    {
        before = receipt;
        CHECK(UmiApplicationLaunchReceiptsFind(owner, 999U, &receipt) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&receipt, &before, sizeof(receipt)) == 0);
        CHECK(UmiApplicationLaunchReceiptsLatest(owner, "missing", &receipt) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&receipt, &before, sizeof(receipt)) == 0);
    }
    else if (strcmp(test, "description") == 0)
    {
        char text[256];
        for (int state = UMI_APPLICATION_LAUNCH_RECEIPT_PENDING;
             state <= UMI_APPLICATION_LAUNCH_RECEIPT_UNCERTAIN; ++state)
        {
            receipt.state = (UmiApplicationLaunchReceiptState)state;
            CHECK(UmiApplicationLaunchReceiptDescribe(&receipt, text, sizeof(text)) == UMI_STATUS_OK &&
                  text[0] != '\0');
        }
        receipt.state = UMI_APPLICATION_LAUNCH_RECEIPT_RUNNING;
        CHECK(UmiApplicationLaunchReceiptDescribe(&receipt, text, sizeof(text)) == UMI_STATUS_OK &&
              strstr(text, "unverified") != NULL);
    }
    else if (strcmp(test, "small-description") == 0)
    {
        char text[2] = "x";
        CHECK(UmiApplicationLaunchReceiptDescribe(&receipt, text, sizeof(text)) ==
                  UMI_STATUS_CAPACITY_EXCEEDED &&
              strcmp(text, "x") == 0);
    }
    else if (strcmp(test, "invalid-status") == 0)
    {
        CHECK(UmiApplicationLaunchReceiptsRejected(owner, first, UMI_STATUS_OK) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationLaunchReceiptsRejected(owner, first, (UmiStatus)999) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationLaunchReceiptsFind(owner, first, &receipt) == UMI_STATUS_OK &&
              receipt.state == UMI_APPLICATION_LAUNCH_RECEIPT_PENDING);
    }
    else if (strcmp(test, "null") == 0)
    {
        CHECK(UmiApplicationLaunchReceiptsCreate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationLaunchReceiptsBegin(NULL, "id", "/path", false, &next) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationLaunchReceiptsFind(owner, first, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiApplicationLaunchReceiptsCount(NULL) == 0U);
    }
    else
        CHECK(false);
done:
    UmiApplicationLaunchReceiptsDestroy(owner);
    return result;
}
