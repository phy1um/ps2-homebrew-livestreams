/**
 * @file
 * GS Privileged Registers
 */

#ifndef __GS_PRIVILEGED_H__
#define __GS_PRIVILEGED_H__

#include <tamtypes.h>

// These are the privileged GS registers mapped to main ram
// These are modified directly without the use of dma

/** PCRTC Mode Setting */
#define GS_REG_PMODE            (volatile u64 *)0x12000000
/** VHP,VCKSEL,SLCK2,NVCK,CLKSEL,PEVS,PEHS,PVS,PHS,GCONT,SPML,PCK2,XPCK,SINT,
 * PRST,EX,CMOD,SLCK,T1248,LC,RC */
#define GS_REG_SMODE1           (volatile u64 *)0x12000010
/** Setting For Modes Related to Video Synchronization */
#define GS_REG_SMODE2           (volatile u64 *)0x12000020
/** DRAM Refresh Settings */
#define GS_REG_SRFSH            (volatile u64 *)0x12000030
/** HS,HSVS,HSEQ,HBP,HFP */
#define GS_REG_SYNCH1           (volatile u64 *)0x12000040
/** HB,HF */
#define GS_REG_SYNCH2           (volatile u64 *)0x12000050
/** VS,VDP,VBPE,VBP,VFPE,VFP */
#define GS_REG_SYNCHV           (volatile u64 *)0x12000060
/** Setting For Rectangular Area Read Output Circuit 1 */
#define GS_REG_DISPFB1          (volatile u64 *)0x12000070
/** Setting For Rectangular Area Read Output Circuit 1 */
#define GS_REG_DISPLAY1 (volatile u64 *)0x12000080
/** Setting For Rectangular Area Read Output Circuit 2 */
#define GS_REG_DISPFB2          (volatile u64 *)0x12000090
/** Setting For Rectangular Area Read Output Circuit 2 */
#define GS_REG_DISPLAY2 (volatile u64 *)0x120000A0
/** Setting For Feedback Buffer Write Buffer */
#define GS_REG_EXTBUF           (volatile u64 *)0x120000B0
/** Feedback Write Setting */
#define GS_REG_EXTDATA          (volatile u64 *)0x120000C0
/** Feedback Write Function Control */
#define GS_REG_EXTWRITE (volatile u64 *)0x120000D0
/** Background Color Setting */
#define GS_REG_BGCOLOR          (volatile u64 *)0x120000E0
/** System Status */
#define GS_REG_CSR                      (volatile u64 *)0x12001000
/** Interrupt Mask Control */
#define GS_REG_IMR                      (volatile u64 *)0x12001010
/** Host I/F Bus Switching */
#define GS_REG_BUSDIR           (volatile u64 *)0x12001040
/** Signal ID Value Read */
#define GS_REG_SIGLBLID (volatile u64 *)0x12001080

#endif /* __GS_PRIVILEGED_H__ */
