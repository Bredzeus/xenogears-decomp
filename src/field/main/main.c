#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libetc.h"

extern int g_FrameDeltaTime;

INCLUDE_ASM("asm/field/nonmatchings/main/main", FieldInitializeControllers);

void FieldRenderSync(void) {
    DrawSync(0);
    Vsync(0);
}

INCLUDE_ASM("asm/field/nonmatchings/main/main", FieldLoadUITextures);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_800777DC);

void FieldUpdateDeltaTime(void) {
    g_FrameDeltaTime = Vsync(1);
}

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077844);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077884);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077AB4);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077C60);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077C88);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077D2C);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077DAC);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077E10);

INCLUDE_ASM("asm/field/nonmatchings/main/main", FieldMain);
