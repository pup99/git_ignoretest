/*
 * NewFile.c
 *
 *  Created on: 2018. 5. 28.
 *      Author: BGKim
 */
#include "../../../common/include/F28x_Project.h"
#include "../../../common/include/driverlib.h"
#include "../../../common/include/device.h"
#include "../include/PhoenixonControls_FPU.h"

#pragma CODE_SECTION(W5300_LoopBackTCPs, ".TI.ramfunc");
#pragma CODE_SECTION(W5300_LoopBackTCPc, ".TI.ramfunc");
#pragma CODE_SECTION(W5300_LoopBackUDP, ".TI.ramfunc");
#pragma CODE_SECTION(ETHERNET_TCPs, ".TI.ramfunc");

extern volatile S_COMMUNICATION_GSRAM_CPU1 s_CPU_1_GSRAM[];
extern volatile S_COMMUNICATION_GSRAM_CPU2 s_CPU_2_GSRAM[];
extern volatile S_COMMUNICATION_DRAM *s_SDRAMp[];
extern volatile Uint16 ui_Updateflag;

#if ETX_VER
Uint16 g_debug_ETX_LENGTH = 2;
Uint16 g_debug_ETX[1] = {0x4021};
Uint16 g_etx_buf[1024];
#endif


uint8 tx_mem_conf[8] = {16,4,4,8,8,8,8,8};          // for setting TMSR regsiter
uint8 rx_mem_conf[8] = {16,4,4,8,8,8,8,8};          // for setting RMSR regsiter

Uint16 mem_pause[20] = { 16464, 10, 1, 1, 2,
                            16464, 10, 2, 1, 2,
                            16464, 10, 4, 1, 2,
                            16464, 10, 8, 1, 2 };

Uint16 mem_restart1[20] = { 16464, 10, 1, 1, 3, 16464, 10, 0, 1, 3, 16464, 10, 0, 1, 3, 16464, 10, 0, 1, 3 };
Uint16 mem_restart2[20] = { 16464, 10, 0, 1, 3, 16464, 10, 2, 1, 3, 16464, 10, 0, 1, 3, 16464, 10, 0, 1, 3 };
Uint16 mem_restart3[20] = { 16464, 10, 0, 1, 3, 16464, 10, 0, 1, 3, 16464, 10, 4, 1, 3, 16464, 10, 0, 1, 3 };
Uint16 mem_restart4[20] = { 16464, 10, 0, 1, 3, 16464, 10, 0, 1, 3, 16464, 10, 0, 1, 3, 16464, 10, 8, 1, 3 };

Uint16 mem_restart12[20] = { 16464, 10, 1, 1, 3, 16464, 10, 2, 1, 3, 16464, 10, 0, 1, 3, 16464, 10, 0, 1, 3 };
Uint16 mem_restart34[20] = { 16464, 10, 0, 1, 3, 16464, 10, 0, 1, 3, 16464, 10, 4, 1, 3, 16464, 10, 8, 1, 3 };

uint16 *ETHERNET_RxBuffer = (uint16 *)ETHERNET_RX_BUFFER_ADDRESS;         // buffer for EthernetRx data
uint16 *ETHERNET_TxBuffer = (uint16 *)ETHERNET_TX_BUFFER_ADDRESS;         // buffer for EthernetRx data
uint8 ip[4] = {192,168,0,COMMUNICATION_ETHERNET_IP};                   // for setting SIP register
uint8 gw[4] = {192,168,0,1};                     // for setting GAR register
uint8 sn[4] = {255,255,255,0};                     // for setting SUBR register
uint8 mac[6] = {0x00,0x08,0xDC,0x00,111,COMMUNICATION_ETHERNET_MAC};      // for setting SHAR register

uint8 serverip[4] = {192,168,1,6};              // "TCP SERVER" IP address for loopback_tcpc()

volatile S_COMMUNICATION_RX_TEMP chamber_temp[4];
volatile S_COMMUNICATION_RX_TEMP_CONV read_chamber_temp_conv[4];

volatile S_COMMUNICATION_RX_TEMP_CONV setted_channel_temp_conv[4];

volatile Uint32 count_channel_time[4] = { 0, };

volatile Uint16 now_channel_status[4] = { 0, };
volatile Uint16 now_channel_index[4] = { 0, };
volatile Uint32 setted_channel_time[4] = { 0, };

extern volatile Uint16 gui_Channel;
extern volatile Uint16 gui_StartFlag;
extern volatile Uint32 gul_DataCount;
extern volatile Uint32 gul_SumCount;
extern volatile Uint32 gPointerCount[COMMUNICATION_CHANNEL];

void ETHERNET_Init(void)
{
    //
    // W5300 Init
    //
    /* initiate W5300 */
    w5300_iinchip_init();

//    data_buf = (unsigned char *)0x80000000;


    /* allocate internal TX/RX Memory of W5300 */
    if(!w5300_sysinit(tx_mem_conf,rx_mem_conf))
    {
//       printf("MEMORY CONFIG ERR.\r\n");
       while(1);
    }
//    w5300_setMR(w5300_getMR() | MR_FS);                        // If Little-endian, set MR_FS.

    w5300_setSHAR(mac);                                      // set source hardware address

    #ifdef __DEF_IINCHIP_PPP__
       if(w5300_pppinit((uint8*)"test01", 6, (uint8*)"pppoe1000", 9)!=1)
       {
          printf("PPPoE fail.\r\n");
          while(1);
       }
       w5300_close(0);
    #else
       /* configure network information */
       w5300_setGAR(gw);                                     // set gateway IP address
       w5300_setSUBR(sn);                                    // set subnet mask address
       w5300_setSIPR(ip);                                    // set source IP address
    #endif

    /* verify network information */
       w5300_getSHAR(mac);                                      // get source hardware address
       w5300_getGAR(gw);                                        // get gateway IP address
       w5300_getSUBR(sn);                                       // get subnet mask address
       w5300_getSIPR(ip);                                       // get source IP address

    //printf("SHAR : %02x:%02x:%02x:%02x:%02x:%02x\r\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    //printf("GWR  : %d.%d.%d.%d\r\n",gw[0],gw[1],gw[2],gw[3]);
   // printf("SUBR : %d.%d.%d.%d\r\n",sn[0],sn[1],sn[2],sn[3]);
   // printf("SIPR : %d.%d.%d.%d\r\n",ip[0],ip[1],ip[2],ip[3]);
}

/**
 * "TCP SERVER" loopback program.
 */
#if ETX_VER
void UserMemcopy(Uint16 *dst_addr, Uint16 dst_addr_offset, Uint16 *src_addr, Uint16 src_addr_offset, Uint16 size)
{
    Uint16 i;

    for (i = 0; i < size; i++)
    {
        dst_addr[dst_addr_offset + i] = src_addr[src_addr_offset + i];
    }
}
#endif

Uint16 ETHERNET_TCPs(SOCKET s, Uint16 port, Uint16 mode, Uint16* rxbuf, Uint16* txbuf, volatile S_COMMUNICATION_DRAM **sp_IF, volatile S_COMMUNICATION_GSRAM_CPU1 *sp_GSRAM_CPU1, volatile S_COMMUNICATION_GSRAM_CPU2 *sp_GSRAM_CPU2)
{
    volatile static Uint16 HeaderFlag = 0;
//   volatile static Uint16 *p_SDRAM;
    volatile static Uint16 DATACount = 0;
    volatile static Uint16 RXCount = 0;
    volatile static Uint16 CHANNEL = 0;
    volatile static Uint16 HEADER = 0;

    volatile static Uint16 ui_Pauseflag = CLEAR;

    Uint32 len;

    Uint16 i, seper, ptr;

    switch (w5300_getSn_SSR(s))
    // check SOCKET status
    {// ------------
        case SOCK_ESTABLISHED:           // ESTABLISHED?

            ui_Pauseflag = CLEAR;

            if (w5300_getSn_IR(s) & Sn_IR_CON)   // check Sn_IR_CON bit
            {
                //printf("%d : Connect OK\r\n",s);
                gui_Channel = 0;
                gui_StartFlag = 0;
                gul_DataCount = 0;
                gul_SumCount  = 0;
                HEADER = CLEAR;
                w5300_setSn_IR(s, Sn_IR_CON);     // clear Sn_IR_CON
            }

            if ((len = w5300_getSn_RX_RSR(s)) > 0)  // check the size of received data
            {

                //데이타 수신 부분임
                len = w5300_recv(s, rxbuf, len);     // recv

                if ((rxbuf[0] == HEADER_DATA || rxbuf[0] == HEADER_JUMP || rxbuf[0] == HEADER_COMMAND || rxbuf[0] == HEADER_START || rxbuf[0] == HEADER_END || rxbuf[0] == HEADER_PAUSE || rxbuf[0] == HEADER_ALARM || rxbuf[0] == HEADER_TEMP) && HEADER == CLEAR) HEADER = rxbuf[0];

                switch (HEADER)
                {
                    case HEADER_DATA:
                        HEADER = COMMUNICATION_Ethernet_STEPRead(sp_IF, rxbuf, len);
                        break;
                    case HEADER_JUMP:
                        HEADER = COMMUNICATION_Ethernet_JUMPRead(sp_GSRAM_CPU1, sp_GSRAM_CPU2, rxbuf, len);
#if(ETX_VER == 0)
                        w5300_send(s, rxbuf, len);     // send
#else
                        ptr = 0;
                        seper = (Uint16) ((Uint32) (len) / (Uint32) (sizeof(S_COMMUNICATION_RX_JUMP) * 2));
                        for(i=0; i < seper; i++)
                        {
                            UserMemcopy(g_etx_buf, ptr, rxbuf, i*sizeof(S_COMMUNICATION_RX_JUMP), sizeof(S_COMMUNICATION_RX_JUMP));
                            ptr += sizeof(S_COMMUNICATION_RX_JUMP);
                            UserMemcopy(g_etx_buf, ptr, g_debug_ETX, 0, 1);
                            ptr++;
                        }
                        w5300_send(s, g_etx_buf, len + (i*2));  // send
#endif
                        break;
                    case HEADER_COMMAND:
                        HEADER = COMMUNICATION_Ethernet_COMMANDRead(sp_GSRAM_CPU1, sp_GSRAM_CPU2, rxbuf, len);
#if(ETX_VER == 0)
                        w5300_send(s, rxbuf, len);     // send
#else
                        ptr = 0;
                        seper = (Uint16) ((Uint32) (len) / (Uint32) (sizeof(S_COMMUNICATION_RX_COMMAD) * 2));
                        for(i=0; i < seper; i++)
                        {
                            UserMemcopy(g_etx_buf, ptr, rxbuf, i*sizeof(S_COMMUNICATION_RX_COMMAD), sizeof(S_COMMUNICATION_RX_COMMAD));
                            ptr += sizeof(S_COMMUNICATION_RX_COMMAD);
                            UserMemcopy(g_etx_buf, ptr, g_debug_ETX, 0, 1);
                            ptr++;
                        }
                        w5300_send(s, g_etx_buf, len + (i*2));  // send
#endif
                        break;
                    case HEADER_START:
                        HEADER = COMMUNICATION_Ethernet_STARTRead(sp_GSRAM_CPU1, sp_GSRAM_CPU2, rxbuf, len);
#if(ETX_VER == 0)
                        w5300_send(s, rxbuf, len);     // send
#else
                        ptr = 0;
                        seper = (Uint16) ((Uint32) (len) / (Uint32) (sizeof(S_COMMUNICATION_RX_START) * 2));
                        for(i=0; i < seper; i++)
                        {
                            UserMemcopy(g_etx_buf, ptr, rxbuf, i*sizeof(S_COMMUNICATION_RX_START), sizeof(S_COMMUNICATION_RX_START));
                            ptr += sizeof(S_COMMUNICATION_RX_START);
                            UserMemcopy(g_etx_buf, ptr, g_debug_ETX, 0, 1);
                            ptr++;
                        }
                        w5300_send(s, g_etx_buf, len + (i*2));  // send
#endif
                        break;
                    case HEADER_PAUSE:
                        HEADER = COMMUNICATION_Ethernet_PAUSERead(sp_GSRAM_CPU1, sp_GSRAM_CPU2, rxbuf, len);
#if(ETX_VER == 0)
                        w5300_send(s, rxbuf, len);     // send
#else
                        ptr = 0;
                        seper = (Uint16) ((Uint32) (len) / (Uint32) (sizeof(S_COMMUNICATION_RX_PAUSE) * 2));
                        for(i=0; i < seper; i++)
                        {
                            UserMemcopy(g_etx_buf, ptr, rxbuf, i*sizeof(S_COMMUNICATION_RX_PAUSE), sizeof(S_COMMUNICATION_RX_PAUSE));
                            ptr += sizeof(S_COMMUNICATION_RX_PAUSE);
                            UserMemcopy(g_etx_buf, ptr, g_debug_ETX, 0, 1);
                            ptr++;
                        }
                        w5300_send(s, g_etx_buf, len + (i*2));  // send
#endif
                        break;
                    case HEADER_TEMP:
                        HEADER = COMMUNICATION_Ethernet_TempRead(chamber_temp, rxbuf, len);
//                        w5300_send(s, rxbuf, len);     // send
                        break;
                }
                len = 0;
            }
            //데이타 송신 부분임
            //수신이 완료 되고 시간이 500ms이상이면 송신함.
            if (HEADER == CLEAR && ui_Updateflag == SET)         // && ui_TimerFlag500ms)
            {
                len = COMMUNICATION_Ethernet_DATAWrite(sp_IF, txbuf);
               //Data send
                if (len){
#if(ETX_VER == 0)
                        w5300_send(s, txbuf, len);     // send
#else
                        w5300_send(s, txbuf, len);     // send

#endif
                }

                len = COMMUNICATION_Ethernet_ENDWrite(sp_GSRAM_CPU1, sp_GSRAM_CPU2, txbuf);
                //End send
                if (len){
#if(ETX_VER == 0)
                        w5300_send(s, txbuf, len);     // send
#else
                        txbuf[len/2] = g_debug_ETX[0];
                        w5300_send(s, txbuf, len + g_debug_ETX_LENGTH);     // send
#endif
                }
                len = COMMUNICATION_Ethernet_ALARMWrite(sp_GSRAM_CPU1, sp_GSRAM_CPU2, txbuf);
                // ALARM send
                if (len){
#if(ETX_VER == 0)
                        w5300_send(s, txbuf, len);     // send
#else
                        txbuf[len/2] = g_debug_ETX[0];
                        w5300_send(s, txbuf, len + g_debug_ETX_LENGTH);     // send
#endif
                }

                ui_Updateflag = CLEAR;
            }

            break;
            // ---------------
        case SOCK_CLOSE_WAIT:               // PASSIVE CLOSED
            w5300_disconnect(s);                // disconnect
            break;
            // --------------
        case SOCK_CLOSED:                   // CLOSED
            w5300_close(s);                        // close the SOCKET
            w5300_socket(s, Sn_MR_TCP, port, mode);   // open the SOCKET
            break;
            // ------------------------------
        case SOCK_INIT:                     // The SOCKET opened with TCP mode
            w5300_listen(s);                       // listen to any connection request from "TCP CLIENT"
            break;

        case SOCK_LISTEN:
            if (ui_Pauseflag == CLEAR) {
                COMMUNICATION_Ethernet_PAUSERead(sp_GSRAM_CPU1, sp_GSRAM_CPU2, mem_pause, 40);
                ui_Pauseflag = SET;
            }
            break;
        default:
            break;
    }
    return (HEADER);
}

/**
 * "TCP SERVER" loopback program.
 */
void W5300_LoopBackTCPs(SOCKET s, uint16 port, uint16* buf, uint16 mode)
{
   uint32 len;
//   uint32 Test = 0xFFFFFFFF;

   switch(w5300_getSn_SSR(s))                // check SOCKET status
   {                                   // ------------
      case SOCK_ESTABLISHED:           // ESTABLISHED?
         if(w5300_getSn_IR(s) & Sn_IR_CON)   // check Sn_IR_CON bit
         {
            //printf("%d : Connect OK\r\n",s);
            w5300_setSn_IR(s,Sn_IR_CON);     // clear Sn_IR_CON
         }

         if((len=w5300_getSn_RX_RSR(s)) > 0) // check the size of received data
         {
            len = w5300_recv(s,buf,len);     // recv
            w5300_send(s,buf,1000);
            if(len !=w5300_send(s,buf,len))  // send
            {
               //printf("%d : Send Fail.len=%ld\r\n",s,len);
            }
         }
         break;
                                       // ---------------
   case SOCK_CLOSE_WAIT:               // PASSIVE CLOSED
       w5300_disconnect(s);                // disconnect
         break;
                                       // --------------
   case SOCK_CLOSED:                   // CLOSED
       w5300_close(s);                        // close the SOCKET
       w5300_socket(s,Sn_MR_TCP,port,mode);   // open the SOCKET
      break;
                                       // ------------------------------
   case SOCK_INIT:                     // The SOCKET opened with TCP mode
       w5300_listen(s);                       // listen to any connection request from "TCP CLIENT"
      break;
   default:
      break;
   }
}

/**
 * "TCP CLIENT" loopback program.
 */
void W5300_LoopBackTCPc(SOCKET s, uint16* addr, uint16 port, uint16* buf, uint16 mode)
{
   uint32 len;
   static uint16 any_port = 1000;

   switch(w5300_getSn_SSR(s))                   // check SOCKET status
   {                                      // ------------
      case SOCK_ESTABLISHED:              // ESTABLISHED?
         if(w5300_getSn_IR(s) & Sn_IR_CON)      // check Sn_IR_CON bit
         {
          //  printf("%d : Connect OK\r\n",s);
            w5300_setSn_IR(s,Sn_IR_CON);        // clear Sn_IR_CON
         }
         if((len=w5300_getSn_RX_RSR(s)) > 0)    // check the size of received data
         {
            len = w5300_recv(s,buf,len);        // recv
            if(len !=w5300_send(s,buf,len))     // send
            {
         //      printf("%d : Send Fail.len=%ld\r\n",s,len);
            }
         }
         break;
                                          // ---------------
   case SOCK_CLOSE_WAIT:                  // PASSIVE CLOSED
       w5300_disconnect(s);                   // disconnect
         break;
                                          // --------------
   case SOCK_CLOSED:                      // CLOSED
       w5300_close(s);                           // close the SOCKET
       w5300_socket(s,Sn_MR_TCP,any_port++,mode);// open the SOCKET with TCP mode and any source port number
      break;
                                          // ------------------------------
   case SOCK_INIT:                        // The SOCKET opened with TCP mode
       w5300_connect(s, addr, port);             // Try to connect to "TCP SERVER"
    //  printf("%d : LOOPBACK_TCPC(%d.%d.%d.%d:%d) Started.\r\n",s,addr[0],addr[1],addr[2],addr[3],port);
      break;
   default:
      break;
   }
}

//
// End of file
//
