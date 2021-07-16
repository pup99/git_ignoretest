#ifndef	PHOENIXON_W5300_SOCKET_H
#define	PHOENIXON_W5300_SOCKET_H

/**
 * \file    socket.h
 * WIZnet SOCKET API function definition
 *
 * For user application, WIZnet provides SOCKET API functions 
 * which are similar to Berkeley SOCKET API.\n
 *
 * \author MidnightCow
 * \date 04/07/2008
 * \version 1.1.1
 *
 * Modify the warning-block code in recv(). Refer to M_15052008.
 * ----------  -------  -----------  ----------------------------
 * Date        Version  Author       Description
 * ----------  -------  -----------  ----------------------------
 * 24/03/2008  1.0.0    MidnightCow  Release with W5300 launching
 * ----------  -------  -----------  ----------------------------
 * 15/05/2008  1.1.0    MidnightCow  Refer to M_15052008.
 *                                   Modify the warning code block in recv(). 
 * ----------  -------  -----------  ----------------------------  
 * 04/07/2008  1.1.1    MidnightCow  Refer to M_04072008.
 *                                   Modify the warning code block in recv(). 
 * ----------  -------  -----------  ----------------------------  
 * 08/08/2008  1.2.0    MidnightCow  Refer to M_08082008.
 *                                   Modify close(). 
 * ----------  -------  -----------  ----------------------------  
 */
#include "PhoenixonControls_W5300.h"
#include "PhoenixonControls_W5300_types.h"

/**********************************
 * define function of SOCKET APIs * 
 **********************************/

/**
 * Open a SOCKET.
 */ 
uint8 w5300_socket(SOCKET s, uint8 protocol, uint16 port, uint16 flag);

/**
 * Close a SOCKET.
 */ 
void w5300_close(SOCKET s);                                                           // Close socket

/**
 * It tries to connect a client.
 */
uint8 w5300_connect(SOCKET s, uint16 * addr, uint16 port);

/**
 * It tries to disconnect a connection SOCKET with a peer.
 */
void w5300_disconnect(SOCKET s);

/**
 * It is listening to a connect-request from a client.
 */
uint8 w5300_listen(SOCKET s);

/**
 * It sends TCP data on a connection SOCKET
 */
uint32 w5300_send(SOCKET s, uint16 * buf, uint32 len);


/**
 * It receives TCP data on a connection SOCKET
 */
uint32 w5300_recv(SOCKET s, uint16 * buf, uint32 len);

/**
 * It sends UDP, IPRAW, or MACRAW data 
 */
uint32 w5300_sendto(SOCKET s, uint8 * buf, uint32 len, uint8 * addr, uint16 port);

/**
 * It receives UDP, IPRAW, or MACRAW data
 */
uint32 w5300_recvfrom(SOCKET s, uint8 * buf, uint32 len, uint8 * addr, uint16  *port);

#endif
/* _SOCKET_H_ */


