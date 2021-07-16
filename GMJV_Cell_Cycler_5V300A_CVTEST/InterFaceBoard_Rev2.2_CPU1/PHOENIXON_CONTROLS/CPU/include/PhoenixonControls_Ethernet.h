/*
 * New.h
 *
 *  Created on: 2018. 5. 28.
 *      Author: BGKim
 */

#ifndef PHOENIXON_CONTROLS_CPU_INCLUDE_NEW_H_
#define PHOENIXON_CONTROLS_CPU_INCLUDE_NEW_H_

#define ETHERNET_RX_BUFFER_ADDRESS 0x81000000
#define ETHERNET_RX_BUFFER_LENGTH

#define ETHERNET_TX_BUFFER_ADDRESS 0x82000000
#define ETHERNET_TX_BUFFER_LENGTH


typedef enum
{
	ETHERNET_NONE,
}E_ETHERNET;

typedef struct
{
	Uint16 ui_New;
	Uint16 b_New : 1;
}S_ETHERNET_FILE;

typedef union
{
	Uint32 ul_New;	  //가장 큰 변수에 앞 주소부터 채워져 간다.
	Uint16 ui_New[2]; //Uint8=0x12, Uint16 = 0x1234 Uint32 = 0x12345678
					  //0x12 34 56 78
					  //0x78 56 34 12 //Little Endian
					  //0x12 34 56 78 //Big Endian
}U_ETHERNET_FILE;

void ETHERNET_Init(void);

Uint16 ETHERNET_TCPs(SOCKET s, Uint16 port, Uint16 mode, Uint16* rxbuf, Uint16* txbuf,
		volatile S_COMMUNICATION_DRAM **sp_IF, volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1,
		  volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2);

void W5300_LoopBackTCPs(SOCKET s, uint16 port, uint16* buf, uint16 mode);
void W5300_LoopBackTCPc(SOCKET s, uint16* addr, uint16 port, uint16* buf, uint16 mode);
void W5300_LoopBackUDP(SOCKET s, uint16 port, uint16* buf, uint16 mode);


#endif /* PHOENIXON_CONTROLS_CPU_INCLUDE_NEW_H_ */
