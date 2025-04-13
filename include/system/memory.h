#ifndef _MEMORY_H
#define _MEMORY_H


#define HEAP_NUM_USERS 0xA

extern s32 D_80059FCC[3];

extern u32 __attribute__((noreturn)) func_80019ACC(u32);
extern void func_80031FF8();

extern void func_80032BDC(char*, ...);

extern char** D_80050110;
extern char** g_HeapContentTypeNames[];

// Pointers to Content type name arrays for specific heap users
extern char** g_HeapUserContentNames[HEAP_NUM_USERS];

// 0x80050110 - Block user flags
#define HEAP_USER_NONE 0x0
#define HEAP_USER_END 0x1
#define HEAP_USER_HIG 0x2
#define HEAP_USER_KAZM 0x3
#define HEAP_USER_MASA 0x4
#define HEAP_USER_MIYA 0x5
#define HEAP_USER_SUGI 0x6
#define HEAP_USER_SUZU 0x7
#define HEAP_USER_YOSI 0x8
#define HEAP_USER_SIMA 0x9
#define HEAP_USER_UNKNOWN 0xA
#define HEAP_USER_TEST 0xB

// 0x80050140 - Block content flags
#define HEAP_CONTENT_NONE 0x0
#define HEAP_CONTENT_FREE 0x1
#define HEAP_CONTENT_FAKE_MALLOC 0x2
#define HEAP_CONTENT_FAKE_CALLOC 0x3
#define HEAP_CONTENT_MODEL_DATA 0x4
#define HEAP_CONTENT_MODEL_PACKET 0x5
#define HEAP_CONTENT_MODEL_LIGHT 0x6
#define HEAP_CONTENT_CD_CHACE 0x7
#define HEAP_CONTENT_MES_IMAGE 0x8
#define HEAP_CONTENT_MES_WORK 0x9
#define HEAP_CONTENT_MES_CUE 0xA
#define HEAP_CONTENT_MIME_WORK 0xB
#define HEAP_CONTENT_MIME_VERTEX 0xC
#define HEAP_CONTENT_MIME_NORMAL 0xD
#define HEAP_CONTENT_SYMBOL_DATA 0xE
#define HEAP_CONTENT_SOUND 0xF
#define HEAP_CONTENT_MES_FONT 0x10
#define HEAP_CONTENT_MES_SYSDATA 0x11
#define HEAP_CONTENT_LS_FONT 0x12
#define HEAP_CONTENT_DELAY_FREE 0x13

#define HEAP_DEFAULT_CONTENT_TYPES 0x20

// Error flags
#define ERR_HEAP_OUT_OF_MEMORY 0x82
#define ERR_HEAP_FREE_NULL 0x83

// Debug flags
#define HEAP_DEBUG_PRINT_NO 0x1
#define HEAP_DEBUG_PRINT_MCB 0x2
#define HEAP_DEBUG_PRINT_ADDRESS 0x4
#define HEAP_DEBUG_PRINT_SIZE 0x8
#define HEAP_DEBUG_PRINT_USER 0x10
#define HEAP_DEBUG_PRINT_GETADD 0x20
#define HEAP_DEBUG_PRINT_FUNCTION 0x40
#define HEAP_DEBUG_PRINT_CONTENTS 0x80
#define HEAP_DEBUG_PRINT_TOTAL_FREE_SIZE 0x8000

typedef struct {
    void* pNext;
    u32 sourceAddress: 21;
    u32 userTag: 4;
    u32 isPinned: 1;
    u32 contentTag: 6;
} HeapBlock;

int HeapLoadSymbols(char* pSymbolFilePath);
void HeapInit(void* heapStart, void* heapEnd);
void HeapRelocate(void* pNewStartAddress);
u16 HeapGetCurrentUser(void);
void HeapSetCurrentUser(u16 userTag);
void HeapGetAllocInformation(u32* pAllocSourceAddr, u32* pAllocSize);
void* HeapAlloc(u32 allocSize, u32 allocFlags);
void HeapConsolidate(void);
void HeapPinBlock(HeapBlock* pBlock);
void HeapUnpinBlock(HeapBlock* pBlock);
void HeapUnpinBlockCopy(HeapBlock* pBlock);
u32 HeapFree(void* pMem);
void HeapFreeBlocksWithFlag(u8 targetFlag);
void HeapFreeAllBlocks(void);
void HeapForceFreeAllBlocks(void);
u32 HeapGetTotalFreeSize(void);
u32 HeapWalkUntilEnd(void);
u32 HeapGetLargestFreeBlockSize(void);
void HeapChangeCurrentUser(u32 userTag, char** pContentTypes);
void HeapSetCurrentContentType(u16 contentTag);
void HeapGetSymbolNameFromAddress(u32 address, u8* pString);
void HeapDebugPrintBlock(HeapBlock* pBlockHeader, void* pBlockMem, u32 blockSize, s32 debugFlags);

#endif