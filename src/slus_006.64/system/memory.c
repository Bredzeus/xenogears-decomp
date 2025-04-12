#include "common.h"
#include "system/memory.h"


u16 D_80059318;
u16 D_8005931C;
void* g_Heap;
u32 g_HeapNeedsConsolidation;
s32 D_80059330;
s32 D_80059334; // Symbols?
s32 D_80059338; // Num symbols?
u32 D_8005933C;
u32 D_80059340;


void HeapInit(void* heapStart, void* heapEnd) {
    HeapBlock* startBlock = (HeapBlock*)((u32)heapStart & -4);
    HeapBlock* endBlock = (HeapBlock*)((u32)heapEnd & -4);
    g_Heap = &startBlock[1];
    startBlock->pNext = endBlock;
    
    D_80059318 = 0x20;
    D_8005931C = 0xa;
    g_HeapNeedsConsolidation = 0;
    D_80059334 = 0;
    D_80059338 = 0;
    
    startBlock->userTag = HEAP_USER_NONE;
    startBlock->contentTag = 0x21;
    
    endBlock[-1].pNext = endBlock;
    endBlock[-1].userTag = HEAP_USER_END;
    endBlock[-1].contentTag = 0x20;

    D_80059FCC[0] = 0;
    func_80031A30();
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/memory", func_80031B10);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/memory", func_80031B9C);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/memory", func_80031BA8);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/memory", func_80031BB4);

void HeapGetAllocInformation(u32* pAllocSourceAddr, u32* pAllocSize) {
    *pAllocSourceAddr = D_80059340;
    *pAllocSize = D_8005933C;
}

void* HeapAlloc(u32 allocSize, u32 allocFlags) {
    u32 nCallerAddr;
    u32 nFreeSize;
    s32 nRemainingSize;
    u32 bOutOfMemory;
    u32 nMinFreeSize;
    HeapBlock* pSmallBlock;
    HeapBlock* pCurBlockHeader;
    HeapBlock* pFreeBlock;
    HeapBlock* pFreeBlockHeader;
    HeapBlock* pNewBlock;
    HeapBlock* pNewBlock2;
    HeapBlock* pCurBlock;

    // Save address the called HeapAlloc
    asm volatile(
        "move $t7, %0\n\t"
        "sw $ra, 0($t7)\n\t"
    :: "r"(&nCallerAddr));
    
    nCallerAddr -= 8;
    D_80059340 = nCallerAddr;
    nCallerAddr = ((nCallerAddr << 7) >> 9);
    
    if (g_HeapNeedsConsolidation != 0) {
        HeapConsolidate();
    }

    bOutOfMemory = 1; 
    D_8005933C = allocSize;
    allocSize = (allocSize + 3);
    allocSize &= -4;
    pFreeBlock = NULL;
    nMinFreeSize = 0x800000;
    pCurBlock = g_Heap;
    pSmallBlock = pFreeBlockHeader = NULL;
    pCurBlockHeader = pCurBlock - 1;
    
    while (1) {
        while (pCurBlockHeader->userTag != HEAP_USER_NONE) {
            if (pCurBlockHeader->userTag == HEAP_USER_END) {
                goto l_0x1CC_EndOfHeap;
            }

            pCurBlock = pCurBlockHeader->pNext;
            pCurBlockHeader = pCurBlock - 1;
        }
        
        nFreeSize = (u32)pCurBlockHeader->pNext - (u32)pCurBlockHeader - 0x10;
        nRemainingSize = nFreeSize - allocSize;

        if (nRemainingSize == 4 || nRemainingSize == 0) {
            bOutOfMemory = 0;

            // Small block allocation
            if (allocFlags == 1) {
                pSmallBlock = pCurBlockHeader; 
            } else {
                l_0xFC_AllocSmallBlock:
                pCurBlockHeader->userTag = D_8005931C;
                pCurBlockHeader->contentTag = D_80059318;
                pCurBlockHeader->isPinned = 0;
                pCurBlockHeader->sourceAddress = nCallerAddr;
                D_80059318 = 0x20;
                return pCurBlockHeader + 1;
            }
        } else if (nRemainingSize >= 5) {
            bOutOfMemory = 0;
    
            if (allocFlags != 1) {
                if (allocFlags != 2) {
                    goto l_0x1B4;
                }

                // Have we found a smaller block?
                if (nFreeSize < nMinFreeSize) {
                    pFreeBlock = pCurBlock;
                    pFreeBlockHeader = pCurBlockHeader;
                    nMinFreeSize = nFreeSize;
                }
                 goto l_0x1C0;
            }
            pFreeBlockHeader = pCurBlockHeader;
            goto l_0x1C0;
    
            l_0x1B4:
            pFreeBlock = pCurBlock;
            pFreeBlockHeader = pCurBlockHeader;
            goto l_0x28C;
        }

        l_0x1C0:
        pCurBlock = pCurBlockHeader->pNext;
        pCurBlockHeader = pCurBlock - 1;
    }

    l_0x1CC_EndOfHeap:
    if (bOutOfMemory != 0) {
        if (D_80059330 != 0) {
            return 0;
        }

        // Goes into error handler, does not return.
        func_80019ACC(ERR_HEAP_OUT_OF_MEMORY);
    }

    if (allocFlags == 1) {
        // Small block allocation
        if (pFreeBlockHeader < pSmallBlock) {
            pCurBlockHeader = pSmallBlock;
            goto l_0xFC_AllocSmallBlock;
        }

        pNewBlock = pFreeBlockHeader->pNext - (allocSize + 8);
        pNewBlock[-1].pNext = pFreeBlockHeader->pNext;
        pNewBlock[-1].userTag = D_8005931C;
        pNewBlock[-1].contentTag = D_80059318;
        pNewBlock[-1].isPinned = 0;
        pNewBlock[-1].sourceAddress = nCallerAddr;
        pFreeBlockHeader->pNext = pNewBlock;
        D_80059318 = 0x20;
        return pNewBlock;
    }
    
    l_0x28C:
    pNewBlock2 = (HeapBlock*)((u32)pFreeBlock + allocSize); 
    pNewBlock2->pNext = pFreeBlockHeader->pNext;
    pNewBlock2->userTag = pFreeBlockHeader->userTag;
    pNewBlock2->contentTag = pFreeBlockHeader->contentTag;
    pNewBlock2->isPinned = pFreeBlockHeader->isPinned;
    pNewBlock2->sourceAddress = pFreeBlockHeader->sourceAddress;
    
    pFreeBlockHeader->pNext = pNewBlock2 + 1;
    pFreeBlockHeader->contentTag = D_80059318;
    D_80059318 = 0x20;
    pFreeBlockHeader->userTag = D_8005931C;
    pFreeBlockHeader->isPinned = 0;
    pFreeBlockHeader->sourceAddress = nCallerAddr;
    return pFreeBlock;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/memory", func_80031F70);

void HeapConsolidate(void) {
    HeapBlock* pCurrent;
    HeapBlock* pOther;
    
    pCurrent = (HeapBlock*)g_Heap - 1;

    while (pCurrent->userTag != HEAP_USER_END) {
        if (pCurrent->userTag == HEAP_USER_NONE) {
            pOther = pCurrent->pNext;
            while (
                (pCurrent->userTag == HEAP_USER_NONE) &&
                (pOther[-1].userTag == HEAP_USER_NONE)
            ) {
                pOther = pOther[-1].pNext;
                pCurrent->pNext = pOther;
            }
        }
        pCurrent = (HeapBlock*)pCurrent->pNext - 1;
    }
    
    g_HeapNeedsConsolidation = 0;
}

void HeapPinBlock(HeapBlock* pBlock) {
    pBlock[-1].isPinned = 1;
}

void HeapUnpinBlock(HeapBlock* pBlock) {
    pBlock[-1].isPinned = 0;
}

void HeapUnpinBlockCopy(HeapBlock* pBlock) {
    pBlock[-1].isPinned = 0;
}

u32 HeapFree(void* pMem) {
    u32 nCallerAddr;
    HeapBlock* pBlock;  

    if (pMem == NULL) {
        if (D_80059330 != 0) {
            return 1;
        }
        
        asm volatile(
            "move $t7, %0\n\t"
            "sw $ra, 0($t7)\n\t"
        :: "r"(&nCallerAddr));
        D_8005933C = 0;
        D_80059340 = nCallerAddr - 8;
        func_80019ACC(ERR_HEAP_FREE_NULL);
    }

    pBlock = ((HeapBlock*)pMem);
    if (pBlock[-1].isPinned == 0) {
        pBlock[-1].contentTag = 0x21;
        pBlock[-1].userTag = 0x0;
        pBlock[-1].sourceAddress = 0;
        g_HeapNeedsConsolidation = 1;
        return 0;
    } else if (pBlock[-1].isPinned == 1) {
        return -1;
    }
}

void HeapFreeBlocksWithFlag(u8 targetFlag) {
    void* pMem;
    HeapBlock* pCurBlock;

    pCurBlock = (HeapBlock*)g_Heap - 1;
    while (pCurBlock->userTag != HEAP_USER_END) {
        if (pCurBlock->userTag == targetFlag) {
            pMem = pCurBlock;
            pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
            HeapFree(pMem + sizeof(HeapBlock));
        } else {
            pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
        }
    }
}

void HeapFreeAllBlocks(void) {
    HeapBlock* pCurBlock;
    void* pMem;

    pCurBlock = (HeapBlock*)g_Heap - 1;
    while (pCurBlock->userTag != HEAP_USER_END) {
        pMem = pCurBlock;
        pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
        HeapFree(pMem + 8);
    }
}

void HeapForceFreeAllBlocks(void) {
    HeapBlock* pCurBlock;
    void* pMem;

    pCurBlock = (HeapBlock*)g_Heap - 1;
    while (pCurBlock->userTag != HEAP_USER_END) {
        pMem = pCurBlock;
        pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
        HeapUnpinBlock(pMem + 8);
        HeapFree(pMem + 8);
    }
}

u32 HeapGetTotalFreeSize(void) {
    u32 nTotalFreeSize;
    HeapBlock* pCurBlock;

    nTotalFreeSize = 0;
    pCurBlock = (HeapBlock*)g_Heap - 1;
    while (pCurBlock->userTag != HEAP_USER_END) {
        if (pCurBlock->userTag == HEAP_USER_NONE) {
            nTotalFreeSize += ((u32)pCurBlock->pNext - (u32)pCurBlock) - sizeof(HeapBlock)*2;
        }
        pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
    }
    return nTotalFreeSize;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/memory", func_800323B4);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/memory", func_80032404);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/memory", func_80032498);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/memory", func_800324B8);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/memory", func_800324C4);


extern char* D_80059248; // "%06x"
extern char* D_80059250; // "%6x"
extern char* D_80059258; // "%s "
extern char* D_8005925C; // "%s"
extern char* D_80059260; // " / "
extern char* D_80059264; // "\n"

void HeapDebugPrintBlock(HeapBlock* pBlockHeader, void* pBlockMem, u32 blockSize, s32 debugFlags) {
    char sFunctionName[0x40];
    s32 nContentType;
    char* sContentType;
    u32 nContentFlag;

    if (debugFlags & HEAP_DEBUG_PRINT_MCB) {
        func_80032BDC(&D_80059248, (u32)pBlockHeader & 0xFFFFFF);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_ADDRESS) {
        func_80032BDC(&D_80059248, (u32)pBlockMem & 0xFFFFFF);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_SIZE) {
        func_80032BDC(&D_80059250, blockSize);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_USER) {
        func_80032BDC(&D_80059258, *(&D_80050110 + pBlockHeader->userTag));
    }
    if (debugFlags & HEAP_DEBUG_PRINT_GETADD) {
        func_80032BDC(&D_80059248, (pBlockHeader->sourceAddress * 4));
    }
    if ((debugFlags & HEAP_DEBUG_PRINT_FUNCTION) && 
        (pBlockHeader->userTag != HEAP_USER_NONE)
    ) {

        // Addr => Function name?
        func_800324C4((pBlockHeader->sourceAddress * 4) - 0x80000000, sFunctionName);
        
        func_80032BDC(&D_8005925C, sFunctionName);
        if (debugFlags & HEAP_DEBUG_PRINT_CONTENTS) {
            if (pBlockHeader->contentTag & 0x1F) {
                func_80032BDC(&D_80059260);
            }
        }
    } 
    
    if (debugFlags & HEAP_DEBUG_PRINT_CONTENTS) {
        nContentFlag = pBlockHeader->contentTag;
        nContentType = nContentFlag & 0x1F;
        if (nContentType != HEAP_CONTENT_NONE) {
            if (nContentFlag & 0x20) {
                sContentType = *(&D_80050140 + nContentType);
            } else {
                sContentType = *(nContentFlag + *(&D_80059FA4 + pBlockHeader->userTag));
            }
            func_80032BDC(&D_8005925C, sContentType);
        }
    }

    func_80032BDC(&D_80059264);
}