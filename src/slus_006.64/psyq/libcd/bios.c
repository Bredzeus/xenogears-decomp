#include "common.h"
#include "psyq/libdma.h"
#include "psyq/libcd.h"
#include "psyq/stdio.h"

/**
 * Internal types
 */
typedef struct {
    unsigned char sync;
    unsigned char ready;
    unsigned char c;
} Intr_t;

typedef struct {
    int unk0;
    int unk4;
} Result_t;

typedef struct {
    int unk0;
    int unk4;
    char* unk8;
} Alarm_t;

/**
 * Callbacks
 */
extern CdlCB CD_cbsync;
extern CdlCB CD_cbready;

/**
 * State
 */
extern int CD_status;
extern int CD_status1;
extern unsigned char CD_mode;
extern unsigned char CD_com;
extern CdlLOC CD_pos;
extern int CD_debug;
extern volatile Intr_t Intr;
extern volatile Alarm_t Alarm;
extern int ComAttr[];
extern Result_t Result[3];

/**
 * CD Drive + DMA Registers
 */
extern volatile u8* reg0; // CD Index/Status.
extern volatile u8* reg1;
extern volatile u8* reg2;
extern volatile u8* reg3; // CD Interrupt Enable.
extern volatile u32* com_delay;
extern volatile u32* cdrom_delay;
extern volatile u32* d_pcr;
extern volatile u32* d3_madr;
extern volatile u32* d3_bcr;
extern volatile u32* d3_chcr;
extern volatile u32* _spu;

/**
 * Strings
 */
extern int CD_comstr[];
//  char* CD_comstr[32] = {"CdlSync",    "CdlNop",
//      "CdlSync",    "CdlNop",
//      "CdlSetloc",  "CdlPlay",
//      "CdlForward", "CdlBackward",
//      "CdlReadN",   "CdlStandby",
//      "CdlStop",    "CdlPause",
//      "CdlReset",   "CdlMute",
//      "CdlDemute",  "CdlSetfilter",
//      "CdlSetmode", "?",
//      "CdlGetlocL", "CdlGetlocP",
//      "?",          "CdlGetTN",
//      "CdlGetTD",   "CdlSeekL",
//      "CdlSeekP",   "?",
//      "?",          "?",
//      "?",          "CdlReadS",
//      "?",          "?",
//      "?",          "?",
//  };

extern char* CD_intstr[];
//  char* CD_intstr[8] = {
//      "NoIntr",  "DataReady", "Complete", "Acknowledge",
//      "DataEnd", "DiskError", "?",        "?",
//  };

extern char* CD_str0;    // "CD timeout: "
extern char* CD_str1;    // "%s:(%s) Sync=%s, Ready=%s\n"
extern char* CD_str2;    // "%s...\n"
extern char* CD_str3;    // "%s: no param\n"
extern char* CD_str4;    // "CD_cw"
extern char* D_80018F3C; // "<NULL>"
extern char D_80018F18;
extern char D_80018F24;

/**
 * Unknown
 */
extern int D_8005678C;

// TODO: Get defs from .h file instead of extern.
extern int Vsync(int mode);
extern int CheckCallback();

/**
 * Fn Foward Decls
 */
void callback();
int getintr();

static inline void _memcpy(void* _dst, void* _src, u32 _size) {
    char* pDst = (char*)_dst;
    char* pSrc = (char*)_src;

    if (pDst == 0)
        return;

    while (_size--) {
        *pDst++ = *pSrc++;
    }
}

static inline void _callback() {
    u8 masked;
    u32 status;

    masked = *reg0 & 0x3;
    while ((status = getintr())) {
        if ((status & 0x4) != 0 && CD_cbready) {
            CD_cbready(Intr.ready, (void*)&Result[1]);
        }
        if ((status & 0x2) != 0 && CD_cbsync) {
            CD_cbsync(Intr.sync, (void*)&Result);
        }
    }
    *reg0 = masked;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcd/bios", getintr);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcd/bios", CD_sync);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcd/bios", CD_ready);

int CD_cw(u8 arg0, u8* arg1, u8* arg2, int arg3) {
    int i;
    int res;

    if (!(CD_debug < 2)) {
        printf((char*)&CD_str2, CD_comstr[arg0]);
    }

    if ((ComAttr[0x40 + arg0] != 0) && (arg1 == 0)) {
        if (CD_debug > 0) {
            printf((char*)&CD_str3, CD_comstr[arg0]);
        }
        return -2;
    }

    CD_sync(0, 0);

    if (arg0 == CdlSetloc) {
        for (i = 0; i < 0x4; ++i) {
            *(u8*)((u32)&CD_pos + i) = arg1[i];
        }
    }

    if (arg0 == CdlSetmode) {
        CD_mode = *arg1;
    }

    Intr.sync = 0;
    if (ComAttr[arg0] != 0) {
        Intr.ready = 0;
    }

    *reg0 = 0;
    for (i = 0; i < ComAttr[0x40 + arg0]; ++i) {
        *reg2 = arg1[i];
    }
    CD_com = arg0;
    *reg1 = CD_com;

    if (arg3 != 0)
        return 0;

    ((Alarm_t*)&Alarm)->unk0 = Vsync(-1) + 0x3C0;
    ((Alarm_t*)&Alarm)->unk4 = 0;
    ((Alarm_t*)&Alarm)->unk8 = (char*)&CD_str4;

    while (Intr.sync == 0) {
        if (((Alarm_t*)&Alarm)->unk0 < Vsync(-1) ||
            ((Alarm_t*)&Alarm)->unk4++ > 0x3C0000) {
            puts((char*)&CD_str0);
            printf((char*)&CD_str1, ((Alarm_t*)&Alarm)->unk8, CD_comstr[CD_com],
                   CD_intstr[Intr.sync], CD_intstr[Intr.ready]);
            CD_flush();
            res = -1;
        } else {
            res = 0;
        }

        if (res != 0) {
            return -1;
        }

        if (CheckCallback() != 0) {
            _callback();
        }
    }

    _memcpy(arg2, &Result, sizeof(Result[0]));
    return -!(Intr.sync ^ 0x5);
}

int CD_vol(CdlATV* vol) {
    *reg0 = 2;
    *reg2 = vol->val0;
    *reg3 = vol->val1;
    *reg0 = 3;
    *reg1 = vol->val2;
    *reg2 = vol->val3;
    *reg3 = 0x20;
    return 0;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcd/bios", CD_flush);

int CD_initvol(void) {
    CdlATV vol;
    // 0x1F801c00 + 0x1b8 = 0x1F801db8 (current main vol left).
    if (*((volatile u16*)((u32)_spu + 0x1b8)) == 0 &&
        *((volatile u16*)((u32)_spu + 0x1ba)) == 0) {
        // 0x1F801c00 + 0x180 = 0x1F801d80 (main vol left/right).
        *((volatile u16*)((u32)_spu + 0x180)) = 0x3fff;
        *((volatile u16*)((u32)_spu + 0x182)) = 0x3fff;
    }
    // 0x1F801c00 + 0x1b0 = 0x1F801db0 (CD volume left/right).
    *((volatile u16*)((u32)_spu + 0x1b0)) = 0x3fff;
    *((volatile u16*)((u32)_spu + 0x1b2)) = 0x3fff;
    // Enable spu, unmute spu and enable cd audio.
    *((volatile u16*)((u32)_spu + 0x1aa)) = (1 << 15) | (1 << 14) | (1 << 0);
    vol.val0 = vol.val2 = 0x80;
    vol.val1 = vol.val3 = 0;
    *reg0 = 2;
    *reg2 = vol.val0;
    *reg3 = vol.val1;
    *reg0 = 3;
    *reg1 = vol.val2;
    *reg2 = vol.val3;
    *reg3 = 0x20;
    return 0;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcd/bios", CD_initintr);

int CD_init(void) {
    puts(&D_80018F18);
    printf(&D_80018F24, &D_8005678C);
    CD_com = 0;
    CD_mode = 0;
    CD_cbready = 0;
    CD_cbsync = 0;
    CD_status1 = 0;
    CD_status = 0;
    ResetCallback();
    InterruptCallback(2, &callback);

    *reg0 = 1;
    while (*reg3 & 7) {
        *reg0 = 1;
        *reg3 = 7;
        *reg2 = 7;
    }

    Intr.ready = Intr.c = 0;
    Intr.sync = 2;

    *reg0 = 0;
    *reg3 = 0;
    *com_delay = 0x1325;

    CD_cw(CdlNop, 0, 0, 0);
    if (CD_status & CdlStatShellOpen) {
        CD_cw(CdlNop, 0, 0, 0);
    }

    // NOTE: 0xA is the init command.
    if (CD_cw(0xA, 0, 0, 0) != 0) {
        return -1;
    }

    if (CD_cw(CdlDemute, 0, 0, 0) != 0) {
        return -1;
    }

    if (CD_sync(0, 0) != 2) {
        return -1;
    }

    return 0;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcd/bios", CD_datasync);

int CD_getsector(void* madr, size_t size) {
    *reg0 = 0;
    *reg3 = 0x80;
    *cdrom_delay = 0x20943;
    *com_delay = 0x1323;
    // DMA3 Master Enable.
    *d_pcr |= 0x8000;
    *d3_madr = (u32)madr;
    *d3_bcr = size | 0x10000;
    while (!(*reg0 & 0x40))
        ;
    // Start DMA transfer.
    *d3_chcr = DMA_CHCR_CDROM_NORMAL;
    // Wait until transfer is complete.
    while (*d3_chcr & DMA_CHCR_START_BUSY_ENABLED)
        ;
    *com_delay = 0x1325;
    return 0;
}

int CD_getsector2(void* madr, size_t size) {
    volatile u32 sp0;
    *reg0 = 0;
    *reg3 = 0x80;
    *cdrom_delay = 0x21020843;
    *com_delay = 0x1325;
    *d_pcr |= 0x8000;
    *d3_madr = (u32)madr;
    *d3_bcr = size | 0x10000;
    while (!(*reg0 & 0x40))
        ;
    // Start DMA transfer w/ "chopping" (cycle stealing by CPU).
    *d3_chcr = DMA_CHCR_CDROM_CHOPPED;
    sp0 = *d3_chcr;
    return 0;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcd/bios", func_80042C98);

void callback() {
    return _callback();
}
