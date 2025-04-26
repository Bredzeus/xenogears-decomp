#include "common.h"
#include "system/archive.h"
#include "psyq/pc.h"
#include "psyq/libcd.h"

// Temp
extern int func_800286CC(void);
extern void func_8002A498(int);
extern int func_80028AAC(void);


void ArchiveInit(u32 pArchiveTable, u32 pHeaderTable, u32 pDebugTable) {
    D_8005A488 = 0;
    D_8005A48C = 0;
    D_8005A490 = 0;
    D_8005A494 = 0;
    D_8005A498 = 0;
    D_8005A49C = 0;
    D_8005A4A4 = 0;
    D_8005A4A8 = 0;
    D_8005A4B4 = 0;
    
    if ( (pDebugTable == NULL) || (pDebugTable == -1) ) {
        while (CdInit() == 0);
        CdSetDebug(0);
        CdDataSyncCallback(0);
        CdSyncCallback(NULL);
        CdReadyCallback(NULL);
        CdControl(7, 0, &D_80059F1C);
        ArchiveCdSetMode(0xA0);
        ArchiveCdDataSync(0);
        Vsync(3);
    } else {
        PCinit();
    }

    if (pDebugTable != -1) {
        g_ArchiveDebugTable = pDebugTable;
    } else {
        g_ArchiveDebugTable = NULL;
    }
    
    g_ArchiveTable = pArchiveTable;
    g_ArchiveHeader = pHeaderTable;
    g_CurArchiveOffset = 0;
    D_8004FDFC = 0;
    g_ArchiveCurFileSize = 0;
    g_ArchiveCdDriveState = 0;
    D_8004FE4C = -1;
    
    if (!pDebugTable) {
        func_8002954C(0x18, pArchiveTable, (0x10 * CD_SECTOR_SIZE), 0, 0);
        ArchiveCdDataSync(0);
        func_8002954C(0x28, g_ArchiveHeader, 0x7A, 0, 0);
        ArchiveCdDataSync(0);
    }
}

void ArchiveReset(void) {
    func_8002A498(0);
    ArchiveCdDataSync(0);
    if (g_ArchiveDebugTable == 0) {
        while (CdControlB(9, 0, &D_80059F1C) == 0);
        ArchiveCdSetMode(0xA0);
        ArchiveCdDataSync(0);
        Vsync(3);
    }
    CdDataSyncCallback(0);
    CdSyncCallback(NULL);
    CdReadyCallback(NULL);
    D_8004FDFC = 0;
    g_ArchiveCurFileSize = 0;
    g_ArchiveCdDriveState = 0;
}

int ArchiveSetIndex(int sectionIndex, int entryIndex) {
    s32 nOffset;

    nOffset = ((u_short*)g_ArchiveHeader)[sectionIndex + entryIndex] - 1;
    g_CurArchiveOffset = nOffset;
    if (nOffset < 0) {
        g_CurArchiveOffset = 0;
        return -1;
    }
    return nOffset;
}

int ArchiveGetArchiveOffsetIndices(int* alignedIndex, int* remainder) {
    int nAligned;
    int nIndex;
    u_short* pArchiveHeaderEntry;

    pArchiveHeaderEntry = g_ArchiveHeader;
    nIndex = 0;
    
    while(nIndex < ARCHIVE_MAX_SECTIONS) {
        if (*pArchiveHeaderEntry == g_CurArchiveOffset + 1) {
            // Round down to 4-byte boundary
            nAligned = (nIndex / 4) * 4;
            
            *alignedIndex = nAligned;
            *remainder = nIndex - nAligned;
            break;
        }
        
        nIndex += 1;
        pArchiveHeaderEntry += 1;
    }
    
    if (nIndex == ARCHIVE_MAX_SECTIONS) {
        *alignedIndex = 0;
        *remainder = 0;
    }
    
    return g_CurArchiveOffset;
}

unsigned short ArchiveGetDiscNumber(void) {
    return *((unsigned short*)(g_ArchiveHeader + 0x78));
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_80028548);

// Only matches on GCC 2.6.X
char* ArchiveReadHddFile(char* pFilePath, int* pFileSize) {
    int nFileSize;
    int hFile;
    int i;
    int nBytesRead;
    char* pFileBuffer;

    for (i = 0; i < RETRY_COUNT; i++) {
        hFile = PCopen(pFilePath, 0, O_RDONLY);
        if (hFile != -1)
            break;
    }

    if (hFile == -1) {
        pFileBuffer = NULL;
        goto error;
    }

    nFileSize = PClseek(hFile, 0, SEEK_END);
    if (pFileSize)
        *pFileSize = nFileSize;
    PClseek(hFile, 0, SEEK_SET);
    
    pFileBuffer = (char*) HeapAlloc(nFileSize, 0x0);
    nBytesRead = 0;
    if (pFileBuffer) {
        for (i = 0; i < RETRY_COUNT; i++) {
            nBytesRead = PCread(hFile, pFileBuffer, nFileSize);
            if (nBytesRead != 0)
                break;
        }
    } 
    
    if (nBytesRead == 0) {
        if (pFileBuffer)
            HeapFree(pFileBuffer);
        pFileBuffer = NULL;
    }

    i = 0;
    while (PCclose(hFile) != 0) {
        i += 1;
        if (i >= RETRY_COUNT) {
            if (pFileBuffer)
                HeapFree(pFileBuffer);
            pFileBuffer = NULL;
            break;
        }        
    }

    error:
    return pFileBuffer;
}

int ArchiveGetCurFileSize(void) {
    return g_ArchiveCurFileSize;
}

int ArchiveDataSync(void) {
    int nResult;

    nResult = D_8004FDFC;
    if (nResult == 0) {
        if (g_ArchiveDebugTable == NULL && CdDataSync(1)) {
            return 1;
        }
        if (g_ArchiveCdDriveState != 0) {
            return 1;
        }
    }
    return nResult;
}

int ArchiveDecodeSize(int entryIndex) {
    char* pFilepath;
    int hArchiveFile;
    int nFileSize;
    u8* pArchiveEntry;
    u32 nOffset;
    
    if (g_ArchiveDebugTable) {
        pFilepath = ArchiveGetFilePath(entryIndex);
        hArchiveFile = PCopen(pFilepath, O_RDONLY, 0);
        nFileSize = PClseek(hArchiveFile, 0, SEEK_END);
        PCclose(hArchiveFile);
        if (nFileSize > 0) {
            goto exit;
        }
    }
    
    nOffset = (entryIndex + g_CurArchiveOffset - 1) * ARCHIVE_HEADER_ENTRY_SIZE;
    pArchiveEntry = nOffset + g_ArchiveTable;
    nFileSize = (
        (pArchiveEntry[6] << 0x18) + 
        (pArchiveEntry[5] << 0x10) + 
        (pArchiveEntry[4] << 0x8) + 
        pArchiveEntry[3]
    );

    exit:
    return nFileSize;
}

// Uses D_8004FE18 as archive offset
int ArchiveDecodeSizeAligned(int entryIndex) {
    char* pFilepath;
    int hArchiveFile;
    int nFileSize;
    u8* pArchiveEntry;
    u32 nOffset;
    
    if (g_ArchiveDebugTable) {
        pFilepath = ArchiveGetFilePath(entryIndex);
        hArchiveFile = PCopen(pFilepath, O_RDONLY, 0);
        nFileSize = PClseek(hArchiveFile, 0, SEEK_END);
        PCclose(hArchiveFile);
        if (nFileSize > 0) {
            goto exit;
        }
    }
    
    nOffset = (entryIndex + D_8004FE18 - 1) * ARCHIVE_HEADER_ENTRY_SIZE;
    pArchiveEntry = nOffset + g_ArchiveTable;
    nFileSize = (
        (pArchiveEntry[6] << 0x18) + 
        (pArchiveEntry[5] << 0x10) + 
        (pArchiveEntry[4] << 0x8) + 
        pArchiveEntry[3]
    );

    exit:
    nFileSize = ((nFileSize + 3) / 4) * 4;
    return nFileSize;
}

// Only matches on GCC 2.6.X
int ArchiveDecodeAlignedSize(unsigned int entryIndex) {
    s32 nSize;
    s32 nAlignedSize;
    
    nSize = ArchiveDecodeSize(entryIndex);
    nAlignedSize = ((nSize + 3 ) / 4);
    return nAlignedSize * 4;
}

int ArchiveDecodeSizeAbsolute(int entryIndex) {
    int nFileSize;
    u8* pArchiveEntry;
    u32 nOffset;
    
    nOffset = (entryIndex + g_CurArchiveOffset - 1) * ARCHIVE_HEADER_ENTRY_SIZE;
    pArchiveEntry = nOffset + g_ArchiveTable;
    nFileSize = (
        (pArchiveEntry[6] << 0x18) + 
        (pArchiveEntry[5] << 0x10) + 
        (pArchiveEntry[4] << 0x8) + 
        pArchiveEntry[3]
    );

    if (nFileSize >= 0)
        nFileSize = 0;
    else
        nFileSize = (s16) (nFileSize * -1);

    return nFileSize;
}

char* ArchiveGetFilePath(int entryIndex) {
    unsigned int nOffset;
    
    if (g_ArchiveDebugTable) {
        nOffset = (entryIndex + g_CurArchiveOffset - 1) * ARCHIVE_ENTRY_NAME_BUFFER_SIZE;
        return nOffset + g_ArchiveDebugTable;
    }

    return NULL;
}

int ArchiveDecodeSector(int entryIndex) {
    u_char* pArchiveEntry;
    unsigned int nOffset;

    nOffset = (entryIndex + g_CurArchiveOffset - 1) * ARCHIVE_HEADER_ENTRY_SIZE;
    pArchiveEntry = nOffset + g_ArchiveTable;
    return (pArchiveEntry[2] << 0x10) + (pArchiveEntry[1] << 0x8) + pArchiveEntry[0];
}

// Same as ArchiveDecodeSector, but uses D_8004FE18 as current archive offset
int func_80028A18(int entryIndex) {
    u_char* pArchiveEntry;
    unsigned int nOffset;

    nOffset = (entryIndex + D_8004FE18 - 1) * ARCHIVE_HEADER_ENTRY_SIZE;
    pArchiveEntry = nOffset + g_ArchiveTable;
    return (pArchiveEntry[2] << 0x10) + (pArchiveEntry[1] << 0x8) + pArchiveEntry[0];
}

void ArchiveCdDataSync(int mode) {
    int nResult;

    if (mode == 0) {
        do {
            nResult = ArchiveDataSync();
        } while (0 < nResult);
    }
    ArchiveDataSync();
}

void* ArchiveChangeStreamingFile(void* pStreamFile) {
    void* pPrevStreamFile;

    pPrevStreamFile = g_ArchiveCurStreamFile;
    g_ArchiveCurStreamFile = pStreamFile;
    return pPrevStreamFile;
}


INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_80028AAC);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_80028B14);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_80028E60);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_80028ECC);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_80028F30);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_8002945C);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_800294B4);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_8002954C);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_800295D8);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", ArchiveReadFile);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_80029AFC);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_80029EB0);

int* ArchiveAllocStreamFile(int numEntries, int allocMode) {
    int* pStreamFile;

    if (numEntries > 0) {
        pStreamFile = HeapAlloc((numEntries * (CD_SECTOR_SIZE + sizeof(ArchiveStreamFileSectorHeader))) + STREAM_FILE_HEADER_SIZE, allocMode);
        if (pStreamFile) {
            *pStreamFile = numEntries;
            ArchiveChangeStreamingFile(pStreamFile);
            func_80028AAC();
            return pStreamFile;
        }
    }
    return NULL;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_8002A2D0);

void ArchiveCdSeekToFile(int entryIndex) {
    u_char nCommand;
    u_char* nParam;
    int nSector;
    CdlLOC* pCdLocation;

    if (entryIndex > 0) {
        pCdLocation = &g_ArchiveCdCurLocation;
        nSector = ArchiveDecodeSector(entryIndex);
        CdIntToPos(nSector, pCdLocation);
        g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_SEEK;
        CdSyncCallback(&ArchiveCdDriveCommandHandler);
        nCommand = CdlSetloc;
        nParam = &g_ArchiveCdCurLocation;
    } else {
        g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_DONE;
        CdSyncCallback(&ArchiveCdDriveCommandHandler);
        nCommand = CdlPause;
        nParam = NULL;
    }

    CdControlF(nCommand, nParam);
}

void ArchiveCdSetMode(u_char mode) {
    int i;
    u_char* pParam;

    g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_RESET_MODE;
    CdSyncCallback(&ArchiveCdDriveCommandHandler);
    for (i = 3, pParam = &D_80059F1B; i >= 0; i -= 1) {
        *pParam-- = 0;
    }
    
    D_80059F18[0] = mode;
    CdControlF(CdlSetmode, &D_80059F18);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_8002A498);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_8002A524);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/archive", func_8002A57C);

void ArchiveCdDriveCommandHandler(u_char status, u_char* pResult) {
    switch (g_ArchiveCdDriveState) {
        case ARCHIVE_CD_DRIVE_READ_SECTOR:
            if (status == CdlComplete) {
                D_8005A48C += 1;
                g_ArchiveCdDriveState += 1;
                CdControlF(CdlReadN, NULL);
            } else {
                D_8005A490 += 1;
                D_80059F08 = CdReadyCallback(NULL);
                g_ArchiveCdDriveError = ARCHIVE_CD_DRIVE_ERR_READ;
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
                CdControlF(CdlNop, NULL);
            }
            break;

        case ARCHIVE_CD_DRIVE_READ_DONE:
            if (status == CdlComplete) {
                D_8005A48C += 1;
                CdSyncCallback(NULL);
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_IDLE;
                return;
            }
            D_8005A490 += 1;
            D_80059F08 = CdReadyCallback(NULL);
            g_ArchiveCdDriveError = ARCHIVE_CD_DRIVE_ERR_READ;
            g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
            CdControlF(CdlNop, NULL);
            break;

        case ARCHIVE_CD_DRIVE_SEEK:
            if (status == CdlComplete) {
                g_ArchiveCdDriveState += 1;
                CdControlF(CdlSeekL, NULL);
            } else {
                g_ArchiveCdDriveError = ARCHIVE_CD_DRIVE_ERR_SEEK;
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
                CdControlF(CdlNop, NULL);
            }
            break;
        
        case ARCHIVE_CD_DRIVE_SEEK_DONE:
            if (status == CdlComplete) {
                CdSyncCallback(NULL);
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_IDLE;
                return;
            }
            
            g_ArchiveCdDriveError = ARCHIVE_CD_DRIVE_ERR_SEEK;
            g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
            CdControlF(CdlNop, NULL);
            break;

        case ARCHIVE_CD_DRIVE_DONE:
            if (status == CdlComplete) {
                CdSyncCallback(NULL);
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_IDLE;
                return;
            }
            
            g_ArchiveCdDriveError = ARCHIVE_CD_DRIVE_ERR_GENERIC;
            g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
            CdControlF(CdlNop, NULL);
            break;

        case ARCHIVE_CD_DRIVE_READ_DATA_SECTOR:
            if (status == CdlComplete) {
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_READ_SECTOR;
                D_8005A488 += 1;
                D_8005A494 += 1;
                CdReadyCallback(D_80059F08);
                CdControlF(CdlSetloc, &g_ArchiveCdCurLocation);
            } else {
                g_ArchiveCdDriveError = ARCHIVE_CD_DRIVE_ERR_READ;
                D_8005A498 += 1;
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
                CdControlF(CdlNop, NULL);
            }
            break;
        
        case ARCHIVE_CD_DRIVE_HALT:
            if (status == CdlComplete) {
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_READ_DATA_SECTOR;
                D_8005A4A8 += 1;
                CdControlF(CdlPause, NULL);
            } else {
                g_ArchiveCdDriveError = ARCHIVE_CD_DRIVE_ERR_HALT;
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
                D_8005A4B4 += 1;
                CdControlF(CdlNop, NULL);
            }
            break;

        case ARCHIVE_CD_DRIVE_SET_ADPCM_PLAY_CHANNEL:
            if (status == CdlComplete) {
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_READ_ADPCM_SECTOR;
                D_80059F15 = D_8004FE38;
                D_80059F14[0] = 1;
                CdControlF(CdlSetfilter, &D_80059F14);
            } else {
                g_ArchiveCdDriveError = ARCHIVE_CD_DRIVE_ERR_ADPCM;
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
                CdControlF(CdlNop, NULL);
            }
            break;

        case ARCHIVE_CD_DRIVE_READ_ADPCM_SECTOR:
            if (status == CdlComplete) {
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_READ_SECTOR;
                D_8005A488 += 1;
                D_8005A494 += 1;
                CdReadyCallback(D_80059F08);
                CdControlF(CdlSetloc, &g_ArchiveCdCurLocation);
            } else {
                g_ArchiveCdDriveError = ARCHIVE_CD_DRIVE_ERR_ADPCM;
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
                D_8005A498 += 1;
                CdControlF(CdlNop, NULL);
            }
            break;

        case ARCHIVE_CD_DRIVE_RESET_MODE:
            if (status == CdlComplete) {
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_DONE;
                CdControlF(CdlPause, NULL);
            } else {
                g_ArchiveCdDriveError = ARCHIVE_CD_DRIVE_ERR_RESET_MODE;
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
                D_8005A4B4 += 1;
                CdControlF(CdlNop, NULL);
            }
            break;

        case ARCHIVE_CD_DRIVE_ERROR:
            if (status == CdlComplete && !(*pResult & CdlStatShellOpen)) {
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_HANDLE_ERROR;
                CdControlF(CdlGetTN, NULL);
            } else {
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
                CdControlF(CdlNop, NULL);
            }
            break;
        
        case ARCHIVE_CD_DRIVE_HANDLE_ERROR:
            // Error handler
            if (status == CdlComplete) {
                switch (g_ArchiveCdDriveError) {
                    case ARCHIVE_CD_DRIVE_ERR_SEEK:
                        // Set the target location and retry
                        g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_SEEK;
                        CdControlF(CdlSetloc, &g_ArchiveCdCurLocation);
                        break;
                    case ARCHIVE_CD_DRIVE_ERR_GENERIC:
                        g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_DONE;
                        CdControlF(CdlPause, NULL);
                        break;
                    case ARCHIVE_CD_DRIVE_ERR_READ:
                        // Pause the read and retry
                        g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_READ_DATA_SECTOR;
                        CdControlF(CdlPause, NULL);
                        break;
                    case ARCHIVE_CD_DRIVE_ERR_HALT:
                        g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_HALT;
                        CdControlF(CdlStop, NULL);
                        break;
                    case ARCHIVE_CD_DRIVE_ERR_ADPCM:
                        g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_SET_ADPCM_PLAY_CHANNEL;
                        CdControlF(CdlSetmode, &D_80059F18);
                        break;
                    case ARCHIVE_CD_DRIVE_ERR_RESET_MODE:
                        g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_RESET_MODE;
                        CdControlF(CdlSetmode, &D_80059F18);
                        break;
                    default:
                        return;
                }
            } else {
                g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_ERROR;
                CdControlF(CdlNop, NULL);
            }
            break;
        
        case ARCHIVE_CD_DRIVE_IDLE:
        default:
            return;
    }
}