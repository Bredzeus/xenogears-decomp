#include "common.h"
#include "field/main.h"
#include "field/actor.h"
#include "system/memory.h"
#include "psyq/libgpu.h"
#include "field/effects.h"

//
INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A55B8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A55C8);

// Set PolyFT4 color for PolyFT4s of next swap chain
void func_800A5600(u_char color) {
    int i;
    POLY_FT4* pPoly;
    int nOffset;
    
    for (i = 0; i < 5; i++) {
        nOffset = i * 2;
        (g_FieldZoomFadeEffect.polygons + nOffset + ((g_FieldCurRenderContextIndex + 1) & 1))->r0 = color;
        (g_FieldZoomFadeEffect.polygons + nOffset + ((g_FieldCurRenderContextIndex + 1) & 1))->g0 = color;
        (g_FieldZoomFadeEffect.polygons + nOffset + ((g_FieldCurRenderContextIndex + 1) & 1))->b0 = color;
    }
}

// Field Fade Effect
// ---------------------------------------------------
void func_800A56A8(int duration) {
    g_FieldEffects.fades[0].b0 = 0xFF00;
    g_FieldEffects.fades[0].g0 = 0xFF00;
    g_FieldEffects.fades[0].r0 = 0xFF00;
    g_FieldEffects.fades[0].duration = duration + 1;
    g_FieldEffects.fades[0].isVisible = 1;
    g_FieldEffects.fades[0].semitransparency = 1;
    g_FieldEffects.fades[0].blueDelta = -0x10000 / duration;
    g_FieldEffects.fades[0].greenDelta = -0x10000 / duration;
    g_FieldEffects.fades[0].redDelta = -0x10000 / duration;
}

void func_800A5710(int duration) {
    g_FieldEffects.fades[0].b0 = 0x0;
    g_FieldEffects.fades[0].g0 = 0x0;
    g_FieldEffects.fades[0].r0 = 0x0;
    g_FieldEffects.fades[0].duration = duration + 1;
    g_FieldEffects.fades[0].isVisible = 1;
    g_FieldEffects.fades[0].semitransparency = 1;
    g_FieldEffects.fades[0].blueDelta = 0x10000 / duration;
    g_FieldEffects.fades[0].greenDelta = 0x10000 / duration;
    g_FieldEffects.fades[0].redDelta = 0x10000 / duration;
}
// ---------------------------------------------------

void func_800A5774(int x, int y, int h) {
    RECT rect;
    int nSize;
    int i;
    u_long* pImageBuffer;
    u_long* pWorkBuffer;

    rect.x = x;
    rect.y = y;
    rect.w = 0x40;
    rect.h = h;
    pImageBuffer = HeapAlloc(h * 0x80, 0x1);
    StoreImage( &rect, pImageBuffer);
    DrawSync(0);
    
    pWorkBuffer = pImageBuffer;
    nSize = h * (0x80 / sizeof(u_long));
    for (i = 0; i < nSize; i += 8) {
        pWorkBuffer[0] |= 0x80008000;
        pWorkBuffer[1] |= 0x80008000;
        pWorkBuffer[2] |= 0x80008000;
        pWorkBuffer[3] |= 0x80008000;
        pWorkBuffer[4] |= 0x80008000;
        pWorkBuffer[5] |= 0x80008000;
        pWorkBuffer[6] |= 0x80008000;
        pWorkBuffer[7] |= 0x80008000; 
        pWorkBuffer += 8;
    }
    
    LoadImage(&rect, pImageBuffer);
    DrawSync(0);
    HeapFree(pImageBuffer);
}

// Zoom fade effect stuff
void func_800A5884(void) {
    int i;
    int nCurX;

    FieldZoomFadeEffectInitialize();
    for (i = 0; i < 2; i++) {
        FieldClearAndSwapOTag();
        FieldZoomFadeEffectUpdate();
        FieldDisplay();
    }

    nCurX = 0x2C0;
    for (i = 0; i < 5; i++) {
        func_800A5774(nCurX, 0x100, 0xE0);
        nCurX += 0x40;
    }

    for (i = 0; i < 2; i++) {
        FieldClearAndSwapOTag();
        FieldZoomFadeEffectUpdate();
        FieldDisplay();
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A5924);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A5C40);

// Or Draw?
INCLUDE_ASM("asm/field/nonmatchings/main/misc5", FieldZoomFadeEffectUpdate);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", FieldZoomFadeEffectInitialize);

void FieldDisplay(void) {
    DrawSync(0);
    Vsync(2);
    ClearImage(&g_FieldCurRenderContext->drawEnvs[0].clip, 0x0, 0x0, 0x0);
    PutDrawEnv(&g_FieldCurRenderContext->drawEnvs[0]);
    PutDispEnv(&g_FieldCurRenderContext->dispEnv);
    DrawOTag(g_FieldCurRenderContext->ot3 + 7);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A6998);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A6C40);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A6E70);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A7064);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A708C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A7120);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A7218);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A732C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A7394);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A73E8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A74F8);

extern int g_FieldPixelIndex;
extern u_long g_FieldCurPixel;
extern u_long* g_Field24BitImageData;
extern u_long* g_Field15BitImageData;

u_int FieldImageConvert24BitColorTo15Bit(void) {
    u_long nPixel;
    u_int nLSB;

    // Are we done reading RGB channels?
    if (!(g_FieldPixelIndex & 3)) {
        nPixel = *g_Field24BitImageData;
        g_Field24BitImageData += 1;
        g_FieldCurPixel = nPixel;
    }
    
    g_FieldPixelIndex++;
    
    nLSB = g_FieldCurPixel & 0xFF;
    g_FieldCurPixel >>= 8;
    
    if (nLSB) {
        nLSB = nLSB >> 3;
        if (nLSB == 0)
            nLSB = 1;
    }
    
    return nLSB;
}

void FieldImageConvert24BitTo15Bit(void) {
    RECT rect;
    int i;
    int j;
    u_long* pImage24Bit;
    u_long* pImage15Bit;
    u_long n15BitPixels;

    pImage24Bit = HeapAlloc(0xA800, 0x0);
    pImage15Bit = HeapAlloc(0x7000, 0x0);
    
    for (i = 0; i < 5; i++) {
        rect.x = i * 0x60;
        rect.y = 0;
        rect.w = 0x60;
        rect.h = 0xE0;
        StoreImage(&rect, pImage24Bit);
        DrawSync(0);
        g_Field24BitImageData = pImage24Bit;
        g_Field15BitImageData = pImage15Bit;
        g_FieldPixelIndex = 0;
        
        for (j = 0; j < 0x1C00; j++) {
            // 15 Bit: RGBRGB
            n15BitPixels = FieldImageConvert24BitColorTo15Bit();
            n15BitPixels |= FieldImageConvert24BitColorTo15Bit() << 0x5;
            n15BitPixels |= FieldImageConvert24BitColorTo15Bit() << 0xA;
            n15BitPixels |= FieldImageConvert24BitColorTo15Bit() << 0x10;
            n15BitPixels |= FieldImageConvert24BitColorTo15Bit() << 0x15;
            n15BitPixels |= FieldImageConvert24BitColorTo15Bit() << 0x1A;
            *g_Field15BitImageData = n15BitPixels;
            g_Field15BitImageData += 1;
        }
        
        rect.x = i << 6;
        rect.y = 0x100;
        rect.w = 0x40;
        rect.h = 0xE0;
        LoadImage(&rect, pImage15Bit);
        DrawSync(0);
    }
    HeapFree(pImage24Bit);
    HeapFree(pImage15Bit);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A7948);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A7C58);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A8314);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A83B4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A8408);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A84C0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A8BA4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A8EAC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A90B4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A915C);

/*
Almost matches

extern int D_800ADB34;
extern RECT g_FieldStoredImageDest;
extern u_long* D_800AFC70;

void func_800A915C(void) {
    if (D_800ADB34 != 1) {
        D_800ADB34 = 1;
        HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
        D_800AFC70 = HeapAlloc(0x8000, 0x1);
        g_FieldStoredImageDest.x = 0x3C0;
        g_FieldStoredImageDest.y = 0x100;
        g_FieldStoredImageDest.w = 0x40;
        g_FieldStoredImageDest.h = 0x100;
        StoreImage(&g_FieldStoredImageDest, D_800AFC70);
        DrawSync(0);
    }
}
*/


INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A91F0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A9274);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A92AC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A9374);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A93CC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A9460);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A94A4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A9688);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A987C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A98B4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A98E8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A99A8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A9B1C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A9B54);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A9F18);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AA6B4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AA9DC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AAA74);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AABD8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AAC08);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AADC8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AAE4C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AAF80);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AB328);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AB378);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AB748);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AB808);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800ABA98);




INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800ABD18);
/*
extern DR_MODE D_800B0188[10]; // 2 * 5
extern SPRT D_800B0200[10];

void func_800ABD18(void) {
    RECT rect;
    DR_MODE* pDrawMode;
    SPRT* pSprt;
    int nTexPageX;
    int i;

    pDrawMode = &D_800B0188;
    pSprt = &D_800B0200;
    nTexPageX = 0x280;
    rect.x = 0x0;
    rect.y = 0x0;
    rect.w = 0xFF;
    rect.h = 0xFF;

    for (i = 0; i < 5; i++) {
        SetDrawMode(pDrawMode, 0, 0, GetTPage(1, 0, nTexPageX, 0), &rect);
        SetDrawMode(pDrawMode + 1, 0, 0, GetTPage(1, 0, nTexPageX, 0), &rect);

        SetSprt(pSprt);
        setRGB0(pSprt, 0x80, 0x80, 0x80);
        setXY0(pSprt, i * 0x80, 0x0);
        setUV0(pSprt, 0x0, 0x0);
        setWH(pSprt, 0x80, 0xE0);
        SetSemiTrans(pSprt, 0);
        pSprt->clut = GetClut(0x0, 0xE8);

        *(pSprt + 1) = *pSprt;

        pSprt += 2;
        pDrawMode += 2;
        nTexPageX += 0x40;
    }
}
*/

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800ABEC8);





INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800ABFDC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AC03C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AC0F0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AC308);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AC3AC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800AC99C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800ACB90);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800ACC58);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800ACCB0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800ACCF4);

short func_800ACD7C(int offset) {
    u_char* pData;

    pData = g_FieldScriptVMCurScriptData + (g_FieldScriptVMCurActor->scriptInstructionPointer + offset);
    return (pData[0] + (pData[1] << 8));
}

int FieldScriptVMGetInstructionArgument(int argumentIndex) {
    u_char* pData;

    pData = g_FieldScriptVMCurScriptData + (g_FieldScriptVMCurActor->scriptInstructionPointer + argumentIndex);
    return *pData | *(pData + 1) <<  8;
}

int FieldScriptVMGetArgument(int index) {
    int nArgument;

    nArgument = FieldScriptVMGetInstructionArgument(index);
    if (!(nArgument & 0x8000)) 
        return FieldScriptVMReadArgumentFromMemory(nArgument & 0xFFFF);
    return nArgument & 0x7FFF;
}
