#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"

// Light data stuff
INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_8006FDEC);

void FieldLZSSDecompress(void* _unused, void* pCompressed, void* pDecompressed) {
    LZSSDecompress(pCompressed, pDecompressed);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_800700B0);

void FieldLoadTIMWithClut(u_long *pTimData, short x, short y, short clutX, short clutY, short clutWidth, short clutHeight) {
    TIM_IMAGE* pTIM;
    TIM_IMAGE tim;   

    OpenTIM(pTimData);
    pTIM = ReadTIM(&tim);

    if (pTIM) {
        if (tim.caddr) {
            if (clutY != -1) {
                tim.crect->x = clutX;
                tim.crect->y = clutY;
            }

            if (clutWidth)
                tim.crect->w = clutWidth;
            
            if (clutHeight)
                tim.crect->h = clutHeight;

            LoadImage(tim.crect, tim.caddr);
        }

        if (tim.paddr) {
            tim.prect->x = x;
        }
        tim.prect->y = y;
        LoadImage(tim.prect, tim.paddr);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_80070488);

INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_80070508);

INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_80070560);

void func_80070594(MATRIX* dest) {
    SVECTOR rotation;

    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    RotMatrix(&rotation, dest);
    dest->t[2] = 0;
    dest->t[1] = 0;
    dest->t[0] = 0;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_800705DC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_80070C84);

// Yolo
INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_80070CC8);

