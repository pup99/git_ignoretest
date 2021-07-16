/**
 * \file    w5300.c
 * Implementation of W5300 I/O fucntions
 *
 * This file implements the basic I/O fucntions that access a register of W5300( IINCHIP_REG).
 * 
 * Revision History :
 * ----------  -------  -----------  ----------------------------
 * Date        Version  Author       Description
 * ----------  -------  -----------  ----------------------------
 * 24/03/2008  1.0.0    MidnightCow  Release with W5300 launching
 * ----------  -------  -----------  ----------------------------
 * 01/05/2008  1.0.1    MidnightCow  Modify a logical error in iinchip_irq(). Refer to M_01052008
 * ----------  -------  -----------  ----------------------------
 * 15/05/2008  1.1.0    MidnightCow  Refer to M_15052008
 *                                   Delete getSn_DPORTR() because \ref Sn_DPORTR is write-only. 
 *                                   Replace 'Sn_DHAR2' with 'Sn_DIPR' in \ref getSn_DIPR().
 * ----------  -------  -----------  ----------------------------
 * 08/08/2008  1.2.0    MidnightCow  Refer to M_08082008
 *                                   Add IINCHIP_CRITICAL_SECTION_ENTER() & IINCHIP_CRITICAL_SECTION_EXIT() to wiz_read_buf() and wiz_write_buf().
 *                                   Modify the description of \ref Sn_SSR and \ref close().</td>
 * ----------  -------  -----------  ----------------------------
 * 15/03/2012  1.2.2    Dongeun      Modify a ARP error.
 *                                   Modify function ( getSUBR(), setSUBR(), ApplySUBNET(), ClearSUBNET() )
 * ----------  -------  -----------  ----------------------------
 * 12/08/2015	1.2.6	Eunkyoung	 modify setSUBR() fuctions
 *									 when setSUBR use, it dosen't save the subnetmask value.
 * ----------  -------	-----------  -----------------------------------------------
  */

#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"

//#include "w5300.h"
//#include "lstring.h"
//#include <string.h>
//
//#include "md5.h"


static uint8 SUBN_VAR[4];
static uint8 IP_VAR[4];


/** 
 * TX memory size variables
 */
uint32 TXMEM_SIZE[MAX_SOCK_NUM];

/** 
 * RX memory size variables
 */
uint32 RXMEM_SIZE[MAX_SOCK_NUM];

/** 
 * The variables for SOCKETn interrupt
 */
uint8 SOCK_INT[MAX_SOCK_NUM];


/***********************
 * Basic I/O  Function *
 ***********************/
#pragma CODE_SECTION(w5300_IINCHIP_READ, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_IINCHIP_WRITE, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getMR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setMR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getIR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setIR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getIMR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setIMR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSHAR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSHAR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getGAR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setGAR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSUBR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSUBR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_applySUBR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_clearSUBR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSIPR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSIPR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getRTR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setRTR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getRCR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setRCR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getPATR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getPTIMER, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setPTIMER, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getPMAGICR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setPMAGICR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getPSIDR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getPDHAR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getUIPR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getUPORTR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getFMTUR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getPn_BRDYR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setPn_BRDYR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getPn_BDPTHR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setPn_BDPTHR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getIDR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_MR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_MR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_CR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_CR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_IMR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_IMR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_IR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_IR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_SSR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_DHAR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_DHAR, ".TI.ramfunc");
//#pragma CODE_SECTION(getSn_DPORTR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_DPORTR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_DIPR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_DIPR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_MSSR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_MSSR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_KPALVTR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_KPALVTR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_TX_WRSR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_TX_WRSR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_TX_FSR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_RX_RSR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_TX_FIFOR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_RX_FIFOR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_PROTOR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_PROTOR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_TOSR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_TOSR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_TTLR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_TTLR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getSn_FRAGR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_setSn_FRAGR, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_iinchip_init, ".TI.ramfunc");
//#pragma CODE_SECTION(iinchip_irq, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_sysinit, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_wiz_write_buf, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_wiz_read_buf, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getIINCHIP_TxMAX, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_getIINCHIP_RxMAX, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_wait_1us, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_wait_1ms, ".TI.ramfunc");
#pragma CODE_SECTION(w5300_wait_10ms, ".TI.ramfunc");



uint16   w5300_IINCHIP_READ(uint32 addr)
{
#if (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_DIRECT_MODE__)   
      return (*((vuint16*)addr));
#else
      vuint16  data;  
      IINCHIP_CRITICAL_SECTION_ENTER(); 
      *((vuint16*)IDM_AR) = (uint16)addr;
      data = *((vuint16*)IDM_DR);   
      IINCHIP_CRITICAL_SECTION_EXIT();
      return data;
#endif
}
void     w5300_IINCHIP_WRITE(uint32 addr,uint16 data)
{
#if (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_DIRECT_MODE__)   
      (*((vuint16*)addr)) = data;

#else
      IINCHIP_CRITICAL_SECTION_ENTER();
      *((vuint16*)IDM_AR) = addr;
      *((vuint16*)IDM_DR) = data;   
      IINCHIP_CRITICAL_SECTION_EXIT();

#endif   
}

uint16   w5300_getMR(void)
{
   return *((vint16*)MR);
}
void     w5300_setMR(uint16 val)
{
   *((vint16*)MR) = val;   
}


/***********************************
 * COMMON Register Access Function *
 ***********************************/

/* Interrupt */ 

uint16   w5300_getIR(void)
{
   return w5300_IINCHIP_READ(IR);
}
void     w5300_setIR(uint16 val)
{
	w5300_IINCHIP_WRITE(IR, val);
}

uint16   w5300_getIMR(void)
{
   return w5300_IINCHIP_READ(IMR);
}
void     w5300_setIMR(uint16 mask)
{
	w5300_IINCHIP_WRITE(IMR, mask);
}


/* Network Information */

void     w5300_getSHAR(uint8 * addr)
{
   addr[0] = 0x00FF & (w5300_IINCHIP_READ(SHAR)>>8);
   addr[1] = 0x00FF & w5300_IINCHIP_READ(SHAR);
   addr[2] = 0x00FF & (w5300_IINCHIP_READ(SHAR2)>>8);
   addr[3] = 0x00FF & w5300_IINCHIP_READ(SHAR2);
   addr[4] = 0x00FF & (w5300_IINCHIP_READ(SHAR4)>>8);
   addr[5] = 0x00FF & w5300_IINCHIP_READ(SHAR4);
}
void     w5300_setSHAR(uint8 * addr)
{
	w5300_IINCHIP_WRITE(SHAR,(((uint16)addr[0])<<8)+addr[1]);
	w5300_IINCHIP_WRITE(SHAR2,((uint16)addr[2]<<8)+addr[3]);
	w5300_IINCHIP_WRITE(SHAR4,((uint16)addr[4]<<8)+addr[5]);
}

void     w5300_getGAR(uint8 * addr)
{
	addr[0] = 0x00FF & (w5300_IINCHIP_READ(GAR)>>8);
	addr[1] = 0x00FF & w5300_IINCHIP_READ(GAR);
	addr[2] = 0x00FF & (w5300_IINCHIP_READ(GAR2)>>8);
	addr[3] = 0x00FF & w5300_IINCHIP_READ(GAR2);
}
void     w5300_setGAR(uint8 * addr)
{
	w5300_IINCHIP_WRITE(GAR, ((uint16)addr[0]<<8)+(uint16)addr[1]);
	w5300_IINCHIP_WRITE(GAR2,((uint16)addr[2]<<8)+(uint16)addr[3]);
}

void     w5300_getSUBR(uint8 * addr)
{
	addr[0] = 0x00FF & SUBN_VAR[0];
	addr[1] = 0x00FF & SUBN_VAR[1];
	addr[2] = 0x00FF & SUBN_VAR[2];
	addr[3] = 0x00FF & SUBN_VAR[3];
}
void     w5300_setSUBR(uint8 * addr)
{
	SUBN_VAR[0] = addr[0];
	SUBN_VAR[1] = addr[1];
	SUBN_VAR[2] = addr[2];
	SUBN_VAR[3] = addr[3];
	w5300_applySUBR();
}
void	w5300_applySUBR(void)
{
	w5300_IINCHIP_WRITE(SUBR, ((uint16)SUBN_VAR[0]<<8)+(uint16)SUBN_VAR[1]);
	w5300_IINCHIP_WRITE(SUBR2,((uint16)SUBN_VAR[2]<<8)+(uint16)SUBN_VAR[3]);
}
void	w5300_clearSUBR(void)
{
	w5300_getSIPR(IP_VAR);
	if(IP_VAR[0] ==0 &&IP_VAR[1] ==0 &&IP_VAR[2] ==0&&IP_VAR[3] ==0)
    {   
		w5300_IINCHIP_WRITE(SUBR, ((uint16)0));
		w5300_IINCHIP_WRITE(SUBR2,((uint16)0));
    }
}
void     w5300_getSIPR(uint8 * addr)
{
	addr[0] = 0x00FF & (w5300_IINCHIP_READ(SIPR)>>8);
	addr[1] = 0x00FF & w5300_IINCHIP_READ(SIPR);
	addr[2] = 0x00FF & (w5300_IINCHIP_READ(SIPR2)>>8);
	addr[3] = 0x00FF & w5300_IINCHIP_READ(SIPR2);
}
void     w5300_setSIPR(uint8 * addr)
{
	w5300_IINCHIP_WRITE(SIPR,((uint16)addr[0]<<8)+(uint16)addr[1]);
	w5300_IINCHIP_WRITE(SIPR2,((uint16)addr[2]<<8)+(uint16)addr[3]);
}


/* Retransmittion */

uint16   w5300_getRTR(void)
{
   return w5300_IINCHIP_READ(RTR);
}
void     setRTR(uint16 timeout)
{
	w5300_IINCHIP_WRITE(RTR,timeout);
}

uint8    w5300_getRCR(void)
{
   return 0x00FF & w5300_IINCHIP_READ(RCR);
}
void     w5300_setRCR(uint8 retry)
{
	w5300_IINCHIP_WRITE(RCR,retry);
}

/* PPPoE */
uint16   w5300_getPATR(void)
{
   return w5300_IINCHIP_READ(PATR);
}

uint8    w5300_getPTIMER(void)
{
   return 0x00FF & w5300_IINCHIP_READ(PTIMER);
}
void     w5300_setPTIMER(uint8 time)
{
	w5300_IINCHIP_WRITE(PTIMER,time);
}

uint8    w5300_getPMAGICR(void)
{
   return 0x00FF & w5300_IINCHIP_READ(PMAGICR);
}
void     w5300_setPMAGICR(uint8 magic)
{
	w5300_IINCHIP_WRITE(PMAGICR,magic);
}

uint16   w5300_getPSIDR(void)
{
   return w5300_IINCHIP_READ(PSIDR);
}

void     w5300_getPDHAR(uint8* addr)
{
   addr[0] = 0x00FF & (w5300_IINCHIP_READ(PDHAR) >> 8);
   addr[1] = 0x00FF & w5300_IINCHIP_READ(PDHAR);
   addr[2] = 0x00FF & (w5300_IINCHIP_READ(PDHAR2) >> 8);
   addr[3] = 0x00FF & w5300_IINCHIP_READ(PDHAR2);
   addr[4] = 0x00FF & (w5300_IINCHIP_READ(PDHAR4) >> 8);
   addr[5] = 0x00FF & w5300_IINCHIP_READ(PDHAR4);
}


/* ICMP packets */

void     w5300_getUIPR(uint8* addr)
{
   addr[0] = 0x00FF & (w5300_IINCHIP_READ(UIPR) >> 8);
   addr[1] = 0x00FF & w5300_IINCHIP_READ(UIPR);
   addr[2] = 0x00FF & (w5300_IINCHIP_READ(UIPR2) >> 8);
   addr[3] = 0x00FF & w5300_IINCHIP_READ(UIPR2);
}

uint16   w5300_getUPORTR(void)
{
   return w5300_IINCHIP_READ(UPORTR);
}

uint16   w5300_getFMTUR(void)
{
   return w5300_IINCHIP_READ(FMTUR);
}


/* PIN "BRYDn" */

uint8    w5300_getPn_BRDYR(uint8 p)
{
   return 0x00FF & w5300_IINCHIP_READ(Pn_BRDYR(p));
}
void     w5300_setPn_BRDYR(uint8 p, uint8 cfg)
{
	w5300_IINCHIP_WRITE(Pn_BRDYR(p),cfg);
}


uint16   w5300_getPn_BDPTHR(uint8 p)
{
   return w5300_IINCHIP_READ(Pn_BDPTHR(p));
}
void     w5300_setPn_BDPTHR(uint8 p, uint16 depth)
{
	w5300_IINCHIP_WRITE(Pn_BDPTHR(p),depth);
}


/* IINCHIP ID */
uint16   w5300_getIDR(void)
{
   return w5300_IINCHIP_READ(IDR);
}


/***********************************
 * SOCKET Register Access Function *
 ***********************************/

/* SOCKET control */

uint16   w5300_getSn_MR(SOCKET s)
{
   return w5300_IINCHIP_READ(Sn_MR(s));
}
void     w5300_setSn_MR(SOCKET s, uint16 mode)
{
	w5300_IINCHIP_WRITE(Sn_MR(s),mode);
}

uint8    w5300_getSn_CR(SOCKET s)
{
   return w5300_IINCHIP_READ(Sn_CR(s));
}
void     w5300_setSn_CR(SOCKET s, uint16 com)
{
	w5300_IINCHIP_WRITE(Sn_CR(s),com);
   while(w5300_IINCHIP_READ(Sn_CR(s))); // wait until Sn_CR is cleared.
}

uint8    w5300_getSn_IMR(SOCKET s)
{
   return 0x00FF & w5300_IINCHIP_READ(Sn_IMR(s));
}
void     w5300_setSn_IMR(SOCKET s, uint8 mask)
{
	w5300_IINCHIP_WRITE(Sn_IMR(s),mask);
}

uint8    w5300_getSn_IR(SOCKET s)
{
   #ifdef __DEF_IINCHIP_INT__    // In case of using ISR routine of iinchip
      return SOCK_INT[s];
   #else                         // In case of processing directly
      return 0x00FF & w5300_IINCHIP_READ(Sn_IR(s));
   #endif   
}
void     w5300_setSn_IR(SOCKET s, uint8 ir)
{
   #ifdef __DEF_IINCHIP_INT__    // In case of using ISR routine of iinchip
      SOCK_INT[s] = SOCK_INT[s] & ~(ir);
   #else                         // In case of processing directly
      w5300_IINCHIP_WRITE(Sn_IR(s),ir);
   #endif   
}


/* SOCKET information */

uint8    w5300_getSn_SSR(SOCKET s)
{
   uint8 ssr, ssr1;
   ssr = 0x00FF & w5300_IINCHIP_READ(Sn_SSR(s));     // first read

   while(1)
   {
      ssr1 = 0x00FF & w5300_IINCHIP_READ(Sn_SSR(s)); // second read
      if(ssr == ssr1) break;                 // if first == sencond, Sn_SSR value is valid.
      ssr = ssr1;                            // if first <> second, save second value into first.
   }
   return ssr;
}

void     w5300_getSn_DHAR(SOCKET s, uint8* addr)
{
   addr[0] = 0x00FF & (w5300_IINCHIP_READ(Sn_DHAR(s))>>8);
   addr[1] = 0x00FF & w5300_IINCHIP_READ(Sn_DHAR(s));
   addr[2] = 0x00FF & (w5300_IINCHIP_READ(Sn_DHAR2(s))>>8);
   addr[3] = 0x00FF & w5300_IINCHIP_READ(Sn_DHAR2(s));
   addr[4] = 0x00FF & (w5300_IINCHIP_READ(Sn_DHAR4(s))>>8);
   addr[5] = 0x00FF & w5300_IINCHIP_READ(Sn_DHAR4(s));
}

void     w5300_setSn_DHAR(SOCKET s, uint8* addr)
{
	w5300_IINCHIP_WRITE(Sn_DHAR(s),  ((uint16)(addr[0]<<8)) + addr[1]);
	w5300_IINCHIP_WRITE(Sn_DHAR2(s), ((uint16)(addr[2]<<8)) + addr[3]);
	w5300_IINCHIP_WRITE(Sn_DHAR4(s), ((uint16)(addr[4]<<8)) + addr[5]);
}

// M_15052008 : Delete this function
//uint16   getSn_DPORTR(SOCKET s)
//{
//   return IINCHIP_READ(Sn_DPORTR(s));
//}


void     w5300_setSn_DPORTR(SOCKET s, uint16 port)
{
	w5300_IINCHIP_WRITE(Sn_DPORTR(s),port);
}

void     w5300_getSn_DIPR(SOCKET s, uint8* addr)
{
   addr[0] = 0x00FF & (w5300_IINCHIP_READ(Sn_DIPR(s))>>8);
   addr[1] = 0x00FF & w5300_IINCHIP_READ(Sn_DIPR(s));
   addr[2] = 0x00FF & (w5300_IINCHIP_READ(Sn_DIPR2(s))>>8);
// M_15052008 : Replace Sn_DHAR2 with Sn_DIPR.   
// addr[3] = 0x00FF & IINCHIP_READ(Sn_DHAR2(s));
   addr[3] = 0x00FF & w5300_IINCHIP_READ(Sn_DIPR2(s));
}
void     w5300_setSn_DIPR(SOCKET s, uint8* addr)
{
	w5300_IINCHIP_WRITE(Sn_DIPR(s),  ((uint16)(addr[0]<<8)) + addr[1]);
	w5300_IINCHIP_WRITE(Sn_DIPR2(s), ((uint16)(addr[2]<<8)) + addr[3]);
}

uint16   w5300_getSn_MSSR(SOCKET s)
{
   return w5300_IINCHIP_READ(Sn_MSSR(s));
}

void     w5300_setSn_MSSR(SOCKET s, uint16 mss)
{
	w5300_IINCHIP_WRITE(Sn_MSSR(s),mss);
}


/* SOCKET communication */

uint8    w5300_getSn_KPALVTR(SOCKET s)
{
   return 0x00FF & (w5300_IINCHIP_READ(Sn_KPALVTR(s)) >> 8);
}

void     w5300_setSn_KPALVTR(SOCKET s, uint8 time)
{
   uint16 keepalive=0;
   keepalive = (w5300_IINCHIP_READ(Sn_KPALVTR(s)) & 0x00FF) + ((uint16)time<<8);
   w5300_IINCHIP_WRITE(Sn_KPALVTR(s),keepalive);
}

uint32   w5300_getSn_TX_WRSR(SOCKET s)
{
   uint32 tx_write_size=0;
   tx_write_size = w5300_IINCHIP_READ(Sn_TX_WRSR(s));
   tx_write_size = (tx_write_size << 16) + w5300_IINCHIP_READ(Sn_TX_WRSR2(s));
   return tx_write_size;
}
void     w5300_setSn_TX_WRSR(SOCKET s, uint32 size)
{
	w5300_IINCHIP_WRITE(Sn_TX_WRSR(s), (uint16)(size >> 16));
	w5300_IINCHIP_WRITE(Sn_TX_WRSR2(s), (uint16)size);
}

uint32   w5300_getSn_TX_FSR(SOCKET s)
{
   uint32 free_tx_size=0;
   uint32 free_tx_size1=0;
   while(1)
   {
      free_tx_size = w5300_IINCHIP_READ(Sn_TX_FSR(s));                           // read
      free_tx_size = (free_tx_size << 16) + w5300_IINCHIP_READ(Sn_TX_FSR2(s));
      if(free_tx_size == free_tx_size1) break;                             // if first == sencond, Sn_TX_FSR value is valid.                                                          
      free_tx_size1 = free_tx_size;                                        // save second value into firs                                                    
   }                                                                       
   return free_tx_size;                                                    
}                                                                          

uint32   w5300_getSn_RX_RSR(SOCKET s)
{
   uint32 received_rx_size=0;
   uint32 received_rx_size1=1;
   while(1)
   {
      received_rx_size = w5300_IINCHIP_READ(Sn_RX_RSR(s));
      received_rx_size = (received_rx_size << 16) + w5300_IINCHIP_READ(Sn_RX_RSR2(s)); // read
      if(received_rx_size == received_rx_size1) break;                                                                         
      received_rx_size1 = received_rx_size;                                      // if first == sencond, Sn_RX_RSR value is valid.
   }                                                                             // save second value into firs                
   return received_rx_size;   
}


void     w5300_setSn_TX_FIFOR(SOCKET s, uint16 data)
{
	w5300_IINCHIP_WRITE(Sn_TX_FIFOR(s),data);
}

uint16   w5300_getSn_RX_FIFOR(SOCKET s)
{
   return w5300_IINCHIP_READ(Sn_RX_FIFOR(s));
}


/* IP header field */

uint8    w5300_getSn_PROTOR(SOCKET s)
{
   return 0x00FF & w5300_IINCHIP_READ(Sn_PROTOR(s));
}
void     w5300_setSn_PROTOR(SOCKET s, uint8 pronum)
{
   uint16 protocolnum;
   protocolnum = w5300_IINCHIP_READ(Sn_PROTOR(s)) & 0xFF00 + pronum;
   w5300_IINCHIP_WRITE(Sn_PROTOR(s),protocolnum);
}

uint8    w5300_getSn_TOSR(SOCKET s)
{
   return 0x00FF & w5300_IINCHIP_READ(Sn_TOSR(s));
}
void     w5300_setSn_TOSR(SOCKET s, uint8 tos)
{
	w5300_IINCHIP_WRITE(Sn_TOSR(s),tos);
}

uint8    w5300_getSn_TTLR(SOCKET s)
{
   return 0x00FF & w5300_IINCHIP_READ(Sn_TTLR(s));
}
void     w5300_setSn_TTLR(SOCKET s, uint8 ttl)
{
	w5300_IINCHIP_WRITE(Sn_TTLR(s),ttl);
}

uint8    w5300_getSn_FRAGR(SOCKET s)
{
   return 0x00FF & w5300_IINCHIP_READ(Sn_FRAGR(s));
}

void     w5300_setSn_FRAGR(SOCKET s, uint8 frag)
{
	w5300_IINCHIP_WRITE(Sn_FRAGR(s),frag);
}


/*******
 * ETC *
 *******/

/* Initialization & Interrupt request routine */

void     w5300_iinchip_init(void)
{
	*((uint16*)MR) = MR_RST;
	w5300_wait_10ms(20);				// wait PLL lock
#if (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_INDIRECT_MODE__)
	   *((vint16*)MR) |= MR_IND;
    #ifdef __DEF_IINCHIP_DBG__	
	      printf("MR value is %04x\r\n",*((vuint16*)MR));
    #endif	
#endif
}


#ifdef __DEF_IINCHIP_INT__ 
/**
 * \todo You should do implement your interrupt request routine instead of this function.
 *       If you want to use ISR, this function should be mapped in your ISR handler.
 */
void     iinchip_irq(void)
{
   uint16 int_val;
   uint16 idx;
   IINCHIP_CRITICAL_SECTION_ENTER();
   //M_01052008 : replaced '==' with '='.
   //while(int_val == IINCHIP_READ(IR))  // process all interrupt 
   while((int_val = IINCHIP_READ(IR)))  
   {          
      for(idx = 0 ; idx < MAX_SOCK_NUM ; idx++)
      {
         if (int_val & IR_SnINT(idx))  // check the SOCKETn interrupt
         {
            SOCK_INT[idx] |= 0x00FF & IINCHIP_READ(Sn_IR(idx)); // Save the interrupt stauts to SOCK_INT[idx]
            IINCHIP_WRITE(Sn_IR(idx),(uint16)SOCK_INT[idx]);  // Clear the interrupt status bit of SOCKETn
         }
      }
      
      if (int_val & (IR_IPCF << 8))    // check the IP conflict interrupt
      {
         printf("IP conflict : %04x\r\n", int_val);
      }
      if (int_val & (IR_DPUR << 8))    // check the unreachable destination interrupt
      {
         printf("INT Port Unreachable : %04x\r\n", int_val);
         printf("UIPR : %d.%d.%d.%d\r\n", 0x00FF & (IINCHIP_READ(UIPR)>>8),
                                          0x00FF & IINCHIP_READ(UIPR),
                                          0x00FF & (IINCHIP_READ(UIPR2)>>8),
                                          0x00FF & IINCHIP_READ(UIPR2));
         printf("UPORTR : %04x\r\n", IINCHIP_READ(UPORTR));
      }
      IINCHIP_WRITE(IR, int_val & 0xFF00);
   }
   IINCHIP_CRITICAL_SECTION_EXIT();
}
#endif 

/* Internal memory operation */
 
uint8    w5300_sysinit(uint8* tx_size, uint8* rx_size)
{
   uint16 i;
   uint16 ssum=0,rsum=0;
   uint mem_cfg = 0;
   
   for(i=0; i < MAX_SOCK_NUM; i++)
   {
      if(tx_size[i] > 64)
      {
      #ifdef __DEF_IINCHIP_DBG__
         printf("Illegal Channel(%d) TX Memory Size.\r\n",i);
      #endif
         return 0;
      }
      if(rx_size[i] > 64)
      {
      #ifdef __DEF_IINCHIP_DBG__         
         printf("Illegal Channel(%d) RX Memory Size.\r\n",i);
      #endif
         return 0;
      }
      ssum += (uint16)tx_size[i];
      rsum += (uint16)rx_size[i];
      TXMEM_SIZE[i] = ((uint32)tx_size[i]) << 10;
      RXMEM_SIZE[i] = ((uint32)rx_size[i]) << 10;
   }
   if( (ssum % 8) || ((ssum + rsum) != 128) )
   {
   #ifdef __DEF_IINCHIP_DBG__
      printf("Illegal Memory Allocation\r\n");
   #endif
      return 0;
   }
   
   w5300_IINCHIP_WRITE(TMS01R,((uint16)tx_size[0] << 8) + (uint16)tx_size[1]);
   w5300_IINCHIP_WRITE(TMS23R,((uint16)tx_size[2] << 8) + (uint16)tx_size[3]);
   w5300_IINCHIP_WRITE(TMS45R,((uint16)tx_size[4] << 8) + (uint16)tx_size[5]);
   w5300_IINCHIP_WRITE(TMS67R,((uint16)tx_size[6] << 8) + (uint16)tx_size[7]);
   
   w5300_IINCHIP_WRITE(RMS01R,((uint16)rx_size[0] << 8) + (uint16)rx_size[1]);
   w5300_IINCHIP_WRITE(RMS23R,((uint16)rx_size[2] << 8) + (uint16)rx_size[3]);
   w5300_IINCHIP_WRITE(RMS45R,((uint16)rx_size[4] << 8) + (uint16)rx_size[5]);
   w5300_IINCHIP_WRITE(RMS67R,((uint16)rx_size[6] << 8) + (uint16)rx_size[7]);
   
   for(i=0; i <ssum/8 ; i++)
   {
      mem_cfg <<= 1;
      mem_cfg |= 1;
   }
   
   w5300_IINCHIP_WRITE(MTYPER,mem_cfg);
   
   #ifdef __DEF_IINCHIP_DBG__
      printf("Total TX Memory Size = %dKB\r\n",ssum);
      printf("Total RX Memory Size = %dKB\r\n",rsum);
      printf("Ch : TX SIZE : RECV SIZE\r\n");
      printf("%02d : %07dKB : %07dKB \r\n", 0, 0x00FF & (IINCHIP_READ(TMS01R)>>8),0x00FF & (IINCHIP_READ(RMS01R)>>8));
      printf("%02d : %07dKB : %07dKB \r\n", 1, 0x00FF & IINCHIP_READ(TMS01R),0x00FF & IINCHIP_READ(RMS01R));
      printf("%02d : %07dKB : %07dKB \r\n", 2, 0x00FF & (IINCHIP_READ(TMS23R)>>8),0x00FF & (IINCHIP_READ(RMS23R)>>8));
      printf("%02d : %07dKB : %07dKB \r\n", 3, 0x00FF & IINCHIP_READ(TMS23R),0x00FF & IINCHIP_READ(RMS23R));
      printf("%02d : %07dKB : %07dKB \r\n", 4, 0x00FF & (IINCHIP_READ(TMS45R)>>8),0x00FF & (IINCHIP_READ(RMS45R)>>8));
      printf("%02d : %07dKB : %07dKB \r\n", 5, 0x00FF & IINCHIP_READ(TMS45R),0x00FF & IINCHIP_READ(RMS45R));
      printf("%02d : %07dKB : %07dKB \r\n", 6, 0x00FF & (IINCHIP_READ(TMS67R)>>8),0x00FF & (IINCHIP_READ(RMS67R)>>8));
      printf("%02d : %07dKB : %07dKB \r\n", 7, 0x00FF & IINCHIP_READ(TMS67R),0x00FF & IINCHIP_READ(RMS67R));
      printf("\r\nMTYPER=%04x\r\n",IINCHIP_READ(MTYPER));
   #endif
   
   return 1;
}

uint32   w5300_wiz_write_buf(SOCKET s,uint16* buf,uint32 len)
{
#if (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_DIRECT_MODE__)
   #if (__DEF_IINCHIP_BUF_OP__ == __DEF_C__)
      uint32 idx=0;
      uint16 Temp=0;

      // M_08082008
      IINCHIP_CRITICAL_SECTION_ENTER();

      for (idx = 0; idx < (len / 2); idx++)
    	  w5300_IINCHIP_WRITE(Sn_TX_FIFOR(s),*((uint16*)(buf+idx)));

      if(len % 2)
      {
    	  Temp = *((uint16*)(buf+idx));
    	  Temp &= 0xFF00;
    	  w5300_IINCHIP_WRITE(Sn_TX_FIFOR(s),(uint16)(Temp));
      }

      // M_08082008
      IINCHIP_CRITICAL_SECTION_EXIT();
   #elif (__DEF_IINCHIP_BUF_OP__ == __DEF_MCU_DEP_INLINE_ASM__)
      register uint16 data;
      register uint16 loop;
      loop = len;
      // M_08082008
      IINCHIP_CRITICAL_SECTION_ENTER();
      __asm
      {
      fifo_write_loop:
        // ARM version
        LDRH    data, [buf], #2
        STRH    data, [Sn_TX_FIFOR(s)]
        SUBS    loop,loop, #2
        BGT     fifo_write_loop
      } 
      // M_08082008
      IINCHIP_CRITICAL_SECTION_EXIT();
   #elif (__DEF_IINCHIP_BUF_OP__ == __DEF_MCU_DEP_DMA__)
      #error "You should do implement this block."
   #endif
#elif (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_INDIRECT_MODE__)
   #if (__DEF_IINCHIP_BUF_OP__ == __DEF_C__)
      uint32 idx=0;
      IINCHIP_CRITICAL_SECTION_ENTER();
      *((vuint16*)IDM_AR) = Sn_TX_FIFOR(s);
      for (idx = 0; idx < len; idx+=2)
          *((vuint16*)IDM_DR) = *((uint16*)(buf+idx));
      IINCHIP_CRITICAL_SECTION_EXIT();

  #elif (__DEF_IINCHIP_BUF_OP__ == __DEF_MCU_DEP_INLINE_ASM__)
      register uint16 data=0;
      register uint16 loop;
      loop = len;
      IINCHIP_CRITICAL_SECTION_ENTER();    
      *((vuint16*)IDM_AR) = Sn_TX_FIFOR(s);
      __asm
      {
      fifo_write_loop:
        // ARM version
        LDRH    data, [buf], #2
        STRH    data, [IDM_DR]
        SUBS    loop,loop, #2
        BGT     fifo_write_loop      
      }
      IINCHIP_CRITICAL_SECTION_EXIT();
   #elif (__DEF_IINCHIP_BUF_OP__ == __DEF_MCU_DEP_DMA__)
      #error "You should do implement this block."
   #else
      #error "Undefined __DEF_IINCHIP_BUF_OP__"
   #endif 
#else
   #error "Undefined __DEF_IINCHIP_ADDRESS_MODE__"   
#endif
    return len;   
}

uint32   w5300_wiz_read_buf(SOCKET s, uint16* buf,uint32 len)
{

#if (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_DIRECT_MODE__)
   #if (__DEF_IINCHIP_BUF_OP__ == __DEF_C__)
      uint32 idx;
      uint16 Temp;
      // M_08082008
      IINCHIP_CRITICAL_SECTION_ENTER();


      for (idx = 0; idx < (len / 2); idx++)
      *((uint16*)(buf+idx)) = w5300_IINCHIP_READ(Sn_RX_FIFOR(s));

      if(len % 2)
      {
    	  Temp = w5300_IINCHIP_READ(Sn_RX_FIFOR(s));
    	  *((uint16*)(buf+idx)) &= 0x00FF;
    	  *((uint16*)(buf+idx)) |= (Temp & 0xFF00) ;
      }



//      if(len > 2)
//      for (idx = 0, add = 0; idx < len; idx+=2, add++)
//      {
//    	  //Temp = 0;
//    	  Swap = w5300_IINCHIP_READ(Sn_RX_FIFOR(s));
////    	  Temp =  0xFF00 & (Swap << 8);
////		  Temp += 0x00FF & (Swap >> 8);
//
////		  Temp = Swap;
//    	  //bgkim add
////    	  *((uint16*)(buf+idx))   = 0x00FF & (Temp);
////    	  *((uint16*)(buf+idx+1)) = 0x00FF & (Temp >> 8);
//    	  *((uint16*)(buf+idx))   = Swap;
//      }
//      else
//      {
//          for (idx = 0, add = 0; idx < len; idx+=2, add++)
//          {
//        	  *((uint16*)(buf+add)) = w5300_IINCHIP_READ(Sn_RX_FIFOR(s));
//          	  *((uint16*)(buf+add)) |= 0xFF00;
//          }

//      }




      // M_08082008
      IINCHIP_CRITICAL_SECTION_EXIT();
   #elif (__DEF_IINCHIP_BUF_OP__ == __DEF_MCU_DEP_INLINE_ASM__)
      register uint16 data=0;
      register int16 loop;
      loop = (int16) len;
      // M_08082008
      IINCHIP_CRITICAL_SECTION_ENTER();
      __asm
      {
      fifo_read_loop:
        // ARM version
        LDRH    data, [Sn_RX_FIFOR(s)]
        STRH    data, [buf], #2
        SUBS    loop,loop, #2
        BGT     fifo_read_loop
      }
      // M_08082008
      IINCHIP_CRITICAL_SECTION_EXIT();
   #elif (__DEF_IINCHIP_BUF_OP__ == __DEF_MCU_DEP_DMA__)
      #error "You should do implement this block."
   #else
      #error "Undefined __DEF_IINCHIP_BUF_OP__"
   #endif 
#elif (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_INDIRECT_MODE__)
   #if (__DEF_IINCHIP_BUF_OP__ == __DEF_C__)
      uint32 idx=0;
      IINCHIP_CRITICAL_SECTION_ENTER();
      *((vuint16*)IDM_AR) = Sn_RX_FIFOR(s);
      for (idx = 0; idx < len; idx+=2)
         *((uint16*)(buf+idx)) = *((vuint16*)IDM_DR);
      IINCHIP_CRITICAL_SECTION_EXIT();

   #elif (__DEF_IINCHIP_BUF_OP__ == __DEF_MCU_DEP_INLINE_ASM__)
      register uint16 data=0;
      register int16 loop;
      loop = (int16)len;
      IINCHIP_CRITICAL_SECTION_ENTER();      
      *((vuint16*)IDM_AR) = Sn_RX_FIFOR(s);
      __asm
      {
      fifo_read_loop:
        // ARM version
        LDRH    data, [IDM_DR]
        STRH    data, [buf], #2
        SUBS    loop,loop, #2
        BGT     fifo_read_loop      
      }
      IINCHIP_CRITICAL_SECTION_EXIT();
   #elif (__DEF_IINCHIP_BUF_OP__ == __DEF_MCU_DEP_DMA__)
      #error "You should do implement this block."
   #else
      #error "Undefined __DEF_IINCHIP_BUF_OP__"
   #endif 
#else
   #error "Undefined __DEF_IINCHIP_ADDRESS_MODE__"   
#endif
    return len;
}


uint32   w5300_getIINCHIP_TxMAX(SOCKET s)
{
   return TXMEM_SIZE[s];
}

uint32   w5300_getIINCHIP_RxMAX(SOCKET s)
{
   return RXMEM_SIZE[s];
}


#ifdef __DEF_IINCHIP_PPP__

/**
 * Max lenght of PPPoE message 
 */
#define PPP_OPTION_BUF_LEN 64 

   /**
    * \fn uint8    pppinit_in(uint8 * id, uint8 idlen, uint8 * passwd, uint8 passwdlen)
    *  Internal function.
    * \details It is called in  pppinit().\n
    *          It processes 4 phases as following. 
    *          - PPPoE Discovery
    *          - LCP process
    *          - PPPoE authentication
    *          - IPCP process
    *
    * \param id User ID for connecting to a PPPoE server
    * \param idlen Length of \em id
    * \param passwd User password for connecting to a PPPoE server
    * \param passwdlen Length of \em passwd
    * \return 1 - success \n
    *         2 - ID or password error \n
    *         3 - Timeout occurs while processing PPPoE \n
    *         4 - Not support authentication type.
    */
uint8    pppinit_in(uint8 * id, uint8 idlen, uint8 * passwd, uint8 passwdlen)
{
   uint8 loop_idx = 0;
   uint8 isr = 0;
   uint8 buf[PPP_OPTION_BUF_LEN];
   uint32 len;
   uint8 str[PPP_OPTION_BUF_LEN];
   uint8 str_idx,dst_idx;
   
   /////////////////////////////
   // PHASE1. PPPoE Discovery //
   /////////////////////////////
   // start to connect pppoe connection
   printf("-- PHASE 1. PPPoE Discovery process --");
   printf(" ok\r\n");
   printf("\r\n");
   setSn_CR(0,Sn_CR_PCON);
   wait_10ms(100);
   
   // check if PPPoE discovery phase is success or not
   loop_idx = 0;
   while (!(getSn_IR(0) & Sn_IR_PNEXT)) 
   {
      printf(".");
      if (loop_idx++ == 10) // timeout
      {
         printf("timeout before LCP\r\n"); 
         return 3;
      }
      wait_10ms(100);
   }
   setSn_IR(0,Sn_IR_PNEXT);
   
   /////////////////////////
   // PHASE2. LCP process //
   /////////////////////////
   printf("-- PHASE 2. LCP process --");
   {
      // Magic number option
      // option format (type value + length value + data)
      // write magic number value
      buf[0] = 0x05; // type value
      buf[1] = 0x06; // length value
      buf[2] = 0x01; buf[3] = 0x01; buf[4] = 0x01; buf[5]= 0x01; // data
      // for MRU option, 1492 0x05d4  
      // buf[6] = 0x01; buf[7] = 0x04; buf[8] = 0x05; buf[9] = 0xD4;
   }
   wiz_write_buf(0,buf, 0x06);
   setSn_TX_WRSR(0,0x06);
   setSn_CR(0,Sn_CR_PCR); // send LCP request to PPPoE server
   wait_10ms(100);
   
   while (!((isr = getSn_IR(0)) & Sn_IR_PNEXT))
   {
      if (isr & Sn_IR_PRECV)     // Not support option
      {
         len = getSn_RX_RSR(0);
         if ( len > 2 )
         {
            wiz_read_buf(0,str, 2);
            len = ((uint16)str[0] << 8) + str[1];
            wiz_read_buf(0,(str+2), len);
            setSn_CR(0,Sn_CR_RECV);
            // get option length
            len = (uint32)str[4]; len = ((len & 0xff) << 8) + (uint32)str[5];
            len += 2;
            str_idx = 6; dst_idx = 0; // PPP header is 6 byte, so starts at 6.
            do 
            {
               if ((str[str_idx] == 0x01) || (str[str_idx] == 0x02) || (str[str_idx] == 0x03) || (str[str_idx] == 0x05))
               {
                  // skip as length of support option. str_idx+1 is option's length.
                  str_idx += str[str_idx+1];
               }
               else
               {
                  // not support option , REJECT
                  memcpy((uint8 *)(buf+dst_idx), (uint8 *)(str+str_idx), str[str_idx+1]);
                  dst_idx += str[str_idx+1]; str_idx += str[str_idx+1];
               }
            } while (str_idx != len);
         
            // send LCP REJECT packet
            wiz_write_buf(0,buf, dst_idx);
            setSn_TX_WRSR(0,dst_idx);
            setSn_CR(0,Sn_CR_PCJ);
            setSn_IR(0,Sn_IR_PRECV);
         }
      }
      printf(".");
      if (loop_idx++ == 10) // timeout
      {
         printf("timeout after LCP\r\n");
         return 3;
      }
      wait_10ms(100);
   }
   setSn_IR(0,Sn_IR_PNEXT);
   printf(" ok\r\n");
   printf("\r\n");

   ///////////////////////////////////
   // PHASE 3. PPPoE Authentication //
   ///////////////////////////////////
   printf("-- PHASE 3. PPPoE Authentication mode --\r\n");
   printf("Authentication protocol : %04x, ", getPATR());
   loop_idx = 0;
   if (getPATR() == 0xC023)         // PAP type
   {
      printf("PAP\r\n"); // in case of adsl normally supports PAP.
      // send authentication data
      // copy (idlen + id + passwdlen + passwd)
      buf[loop_idx] = idlen; loop_idx++;
      memcpy((uint8 *)(buf+loop_idx), (uint8 *)(id), idlen); loop_idx += idlen;
      buf[loop_idx] = passwdlen; loop_idx++;
      memcpy((uint8 *)(buf+loop_idx), (uint8 *)(passwd), passwdlen); loop_idx += passwdlen;
      wiz_write_buf(0,buf, loop_idx);
      setSn_TX_WRSR(0,loop_idx);
      setSn_CR(0,Sn_CR_PCR);
      wait_10ms(100);
   }
   else if (getPATR() == 0xC223)    // CHAP type
   {
      uint8 chal_len;
      md5_ctx context;
      uint8  digest[16];
      
      len = getSn_RX_RSR(0);
      if ( len > 2 )
      {
         wiz_read_buf(0,str,2);
         len = ((uint32)str[0] << 8) + (uint32)str[1];
         wiz_read_buf(0, str, len);
         setSn_CR(0,Sn_CR_RECV);
         #ifdef __DEF_IINCHIP_DBG__
         {
            int16 i;
            printf("recv CHAP\r\n");
            for (i = 0; i < 32; i++) printf ("%02X ", str[i]);
            printf("\r\n");
         }
         #endif
         // str is C2 23 xx CHAL_ID xx xx CHAP_LEN CHAP_DATA
         // index  0  1  2  3       4  5  6        7 ...
         
         memset(buf,0x00,64);
         buf[loop_idx] = str[3]; loop_idx++; // chal_id
         memcpy((uint8 *)(buf+loop_idx), (uint8 *)(passwd), passwdlen); loop_idx += passwdlen; //passwd
         chal_len = str[6]; // chal_id
         memcpy((uint8 *)(buf+loop_idx), (uint8 *)(str+7), chal_len); loop_idx += chal_len; //challenge
         buf[loop_idx] = 0x80;
         #ifdef __DEF_IINCHIP_DBG__
         {
            int16 i;
            printf("CHAP proc d1\r\n");
            
            for (i = 0; i < 64; i++) printf ("%02X ", buf[i]);
            printf("\r\n");
         }
         #endif
         
         md5_init(&context);
         md5_update(&context, buf, loop_idx);
         md5_final(digest, &context);
         
         #ifdef __DEF_IINCHIP_DBG__
         {
            uint i;
            printf("CHAP proc d1\r\n");
            for (i = 0; i < 16; i++) printf ("%02X", digest[i]);
            printf("\r\n");
         }
         #endif
         loop_idx = 0;
         buf[loop_idx] = 16; loop_idx++; // hash_len
         memcpy((uint8 *)(buf+loop_idx), (uint8 *)(digest), 16); loop_idx += 16; // hashed value
         memcpy((uint8 *)(buf+loop_idx), (uint8 *)(id), idlen); loop_idx += idlen; // id
         wiz_write_buf(0,buf, loop_idx);
         setSn_TX_WRSR(0,loop_idx);
         setSn_CR(0,Sn_CR_PCR);
         wait_10ms(100);
      }
   }
   else
   {
      printf("Not support\r\n");
      #ifdef __DEF_IINCHIP_DBG__
         printf("Not support PPP Auth type: %.4x\r\n",getPATR());
      #endif
      return 4;
   }
   printf("\r\n");
   
   printf("-- Waiting for PPPoE server's admission --");
   loop_idx = 0;
   while (!((isr = getSn_IR(0)) & Sn_IR_PNEXT))
   {
      if (isr & Sn_IR_PFAIL)
      {
         printf("failed\r\nReinput id, password..\r\n");
         return 2;
      }
      printf(".");
      if (loop_idx++ == 10) // timeout
      {
         printf("timeout after PAP\r\n");
         return 3;
      }
      wait_10ms(100);
   }
   setSn_IR(0,Sn_IR_PNEXT);
   printf("ok\r\n");
   printf("\r\n");

   ///////////////////////////
   // PHASE 4. IPCP process //
   ///////////////////////////
   printf("-- PHASE 4. IPCP process --");
   // IP Address
   buf[0] = 0x03; buf[1] = 0x06; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x00; buf[5] = 0x00;
   wiz_write_buf(0,buf, 6);
   setSn_TX_WRSR(0,6);
   setSn_CR(0,Sn_CR_PCR);
   wait_10ms(100);
   
   loop_idx = 0;
   while (1)
   {
      if (getSn_IR(0) & Sn_IR_PRECV)
      {
         len = getSn_RX_RSR(0);
         if ( len > 2 )
         {
            wiz_read_buf(0,str,2);
            len = ((uint32)str[0] << 8) + (uint32)str[1];
            wiz_read_buf(0, str, len);
            setSn_CR(0,Sn_CR_RECV);
            str_idx = 6; dst_idx = 0;
            if (str[2] == 0x03) // in case of NAK
            {
               do 
               {
                  if (str[str_idx] == 0x03) // request only ip information
                  {
                     memcpy((uint8 *)(buf+dst_idx), (uint8 *)(str+str_idx), str[str_idx+1]);
                     dst_idx += str[str_idx+1]; str_idx += str[str_idx+1];
                  }
                  else
                  {
                     // skip byte
                     str_idx += str[str_idx+1];
                  }
                  // for debug
                  //printf("s: %d, d: %d, l: %d", str_idx, dst_idx, len);
               } while (str_idx != len);
               wiz_write_buf(0,buf, dst_idx);
               setSn_TX_WRSR(0,dst_idx);
               setSn_CR(0,Sn_CR_PCR); // send ipcp request
               wait_10ms(100);
               break;
            }
         }
         setSn_IR(0,Sn_IR_PRECV);
      }
      
      printf(".");
      if (loop_idx++ == 10) // timeout
      {
         printf("timeout after IPCP\r\n");
         return 3;
      }
      wait_10ms(100);
      wiz_write_buf(0, buf, 6);
      setSn_TX_WRSR(0,6);
      setSn_CR(0,Sn_CR_PCR); //ipcp re-request
   }
   
   loop_idx = 0;
   while (!(getSn_IR(0) & Sn_IR_PNEXT))
   {
      printf(".");
      if (loop_idx++ == 10) // timeout
      {
         printf("timeout after IPCP NAK\r\n");
         return 3;
      }
      wait_10ms(100);
      setSn_CR(0,Sn_CR_PCR); // send ipcp request
   }
   setSn_IR(0,Sn_IR_PNEXT);
   printf("ok\r\n");
   printf("\r\n");
   return 1;
   // after this function, the pppoe server's mac address and pppoe session id is saved in PHAR and PSIDR repectly.
}

uint8    pppinit(uint8 *id, uint8 idlen, uint8 *passwd, uint8 passwdlen)
{
	uint8 ret;
	uint8 isr;
	
	// PHASE0. PPPoE setup

	printf("-- PHASE 0. PPPoE setup process --\r\n");
	printf("\r\n");
	setMR(getMR()|MR_PPPoE); 		             // set PPPoE mode and FIFO swap
	setMR(getMR()|MR_FS);						 // If little-endian, set MR_FS. Otherwise, comment.
	isr =  getSn_IR(0);
	setSn_IR(0,(uint16)isr);                     // clear the previous value of Sn_IR(0)   
	
	setPTIMER(200);                              // set LPC request time to 5 seconds
	setPMAGICR(0x01);                            // set the magic number
	setSn_MR(0, Sn_MR_PPPoE);                    // set Sn_MR(0) to PPPoE mode
	setSn_CR(0,Sn_CR_OPEN);                      //open SOCKET0 with PPPoE mode                      
	
	ret = pppinit_in(id, idlen, passwd, passwdlen); // process the PPPoE message

	setSn_CR(0, Sn_CR_CLOSE);                       // close PPPoE SOCKET0

	return ret;   
}
   
void    pppterm(uint8 *mac, uint16 sessionid)
{
   uint8 isr;
   #ifdef __DEF_IINCHIP_DBG__
      printf("pppterm()\r\n");
   #endif
   
   // set PPPoE mode
   setMR(getMR() | MR_PPPoE);                
   
   // set pppoe server's mac address and session id 
   // must be setted these value.
   setSn_DHAR(0, mac);
   setSn_DPORTR(0,sessionid);

   // clear the previous value of Sn_IR(0) 
	isr =  getSn_IR(0);
	setSn_IR(0,(uint16)isr);                  
   
   //open SOCKET0 with PPPoE mode
	setSn_MR(0, Sn_MR_PPPoE);                    
	setSn_CR(0,Sn_CR_OPEN);                      
   while(getSn_SSR(0) != SOCK_PPPoE)            
   
   // close PPPoE connection
   setSn_CR(0,Sn_CR_PDISCON);
   wait_10ms(100);
   // close socket
   setSn_CR(0,Sn_CR_CLOSE);
   
   #ifdef __DEF_IINCHIP_DBG__
      printf("pppterm() end ..\r\n");
   #endif
}

#endif


void  w5300_wait_1us(uint32 us)
{
   uint32 i;
   for(i = 0; i < us ; i++)
   {
//      for(j = 0; j < 100; j++);
	   DELAY_US(1); //1us //bgkim add 20180223
   }
}

void  w5300_wait_1ms(uint32 ms)
{
   uint32 i;
   for(i = 0; i < ms ; i++)
   {
	   w5300_wait_1us(1000);
   }
   
}

void  w5300_wait_10ms(uint32 ms)
{
   uint32 i;
   for(i = 0; i < ms ; i++)
   {
	   w5300_wait_1ms(10);
   }
}
