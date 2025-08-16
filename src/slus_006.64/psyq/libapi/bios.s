/**
 * @file bios.s
 *
 * PSX BIOS / custom function stubs for libapi
 *
 * Contains:
 *   - Standard system call wrappers (DEFINE_SYS_FN).
 *   - A-, B-, and C-Function wrappers.
 *
 * @see https://psx-spx.consoledev.net/kernelbios/#bios-function-summary
 */

.macro DEFINE_FN fn_name, call_code, fn_num
    .set noat
    .set noreorder
    .global \fn_name
    .type \fn_name, @function
    \fn_name:
      addiu   $t2, $zero, \call_code
      jr      $t2
      addiu   $t1, $zero, \fn_num
    .size \fn_name, . - \fn_name
      nop
.endm

.macro DEFINE_SYS_FN fn_name, fn_num
    .set noreorder
    .set noat
    .global \fn_name
    .type \fn_name, @function
    \fn_name:
      addiu   $a0, $zero, \fn_num
      syscall 0
      jr      $ra
      nop
    .size \fn_name, . - \fn_name
.endm

#ifdef FUNC_1
  DEFINE_SYS_FN EnterCriticalSection, 0x1
  /* Expands to:
      .set noreorder
      .set noat
      .global EnterCriticalSection
      .type EnterCriticalSection, @function
  EnterCriticalSection:
      addiu   $a0, $zero, 0x1
      syscall 0
      jr      $ra
      nop
      .size EnterCriticalSection, . - EnterCriticalSection
  */
#endif

#ifdef FUNC_2
  DEFINE_SYS_FN ExitCriticalSection, 0x2
#endif

#ifdef FUNC_A44
  DEFINE_FN FlushCache, 0xA0, 0x44
  /* Expands to:
      .set noat
      .set noreorder
      .global FlushCache
      .type FlushCache, @function
  FlushCache:
      addiu   $t2, $zero, 0xA0
      jr      $t2
      addiu   $t1, $zero, 0x44
      .size FlushCache, . - FlushCache
      nop
  */
#endif

#ifdef FUNC_A70
  DEFINE_FN _bu_init, 0xA0, 0x70
#endif

#ifdef FUNC_B07
  DEFINE_FN DeliverEvent, 0xB0, 0x7
#endif

#ifdef FUNC_B08
  DEFINE_FN OpenEvent, 0xB0, 0x8
#endif

#ifdef FUNC_B09
  DEFINE_FN CloseEvent, 0xB0, 0x9
#endif

#ifdef FUNC_B0B
  DEFINE_FN TestEvent, 0xB0, 0xB
#endif

#ifdef FUNC_B0C
  DEFINE_FN EnableEvent, 0xB0, 0xC
#endif

#ifdef FUNC_B0D
  DEFINE_FN DisableEvent, 0xB0, 0xD
#endif

#ifdef FUNC_B20
  DEFINE_FN UnDeliverEvent, 0xB0, 0x20
#endif

#ifdef FUNC_B32
  DEFINE_FN open, 0xB0, 0x32
#endif

#ifdef FUNC_B34
  DEFINE_FN read, 0xB0, 0x34
#endif

#ifdef FUNC_B35
  DEFINE_FN write, 0xB0, 0x35
#endif

#ifdef FUNC_B36
  DEFINE_FN close, 0xB0, 0x36
#endif

#ifdef FUNC_B41
  DEFINE_FN format, 0xB0, 0x41
#endif

#ifdef FUNC_B42
  DEFINE_FN firstfile, 0xB0, 0x42
#endif

#ifdef FUNC_B43
  DEFINE_FN nextfile, 0xB0, 0x43
#endif

#ifdef FUNC_B44
  DEFINE_FN rename, 0xB0, 0x44
#endif

#ifdef FUNC_B45
  DEFINE_FN erase, 0xB0, 0x45
#endif

#ifdef FUNC_B51
  DEFINE_FN Krom2RawAdd, 0xB0, 0x51
#endif
