#include "common.h"
#include "psyq/libapi.h"
#include "psyq/libcd.h"

extern char D_80018CE4; // "none"
extern char D_80018CCC;

void def_cbsync(void);
void def_cbready(void);
void def_cbread(void);

typedef void (*VoidCallback_t)(void);

int CdInit(void) {
    int i;
    int nStatus;
    int nResult;

    i = 4;
    while (i != -1) {
        nStatus = CdReset(1);
        i--;
        if (nStatus == 1) {
            CdSyncCallback(&def_cbsync);
            CdReadyCallback(&def_cbready);
            CdReadCallback(&def_cbread);
            CdReadMode(0);
            nResult = 1;
            break;
        }

        if (i != -1) {
            continue;
        }

        printf(&D_80018CCC);
        nResult = 0;
        break;
    }

    return nResult;
}

void def_cbsync(void) {
    DeliverEvent(0xF0000003, 0x20);
}

void def_cbready(void) {
    DeliverEvent(0xF0000003, 0x40);
}

void def_cbread() {
    DeliverEvent(0xF0000003, 0x40);
}
