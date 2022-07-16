#ifndef ETHERNET_H
#define ETHERNET_H
#include <stdint.h>

typedef enum
{
    ETHERNET_MODE_PROMISC,   /*receive all ethernet msg*/
    ETHERNET_MODE_ALL_MULTICAST, /*receive all multicast msg*/
    ETHERNET_MODE_MULTICAST, /*receive only specific multicast msg*/
    ETHERNET_MODE_HOST_ONLY, /*receive only msg for the host*/
}ethernet_mode_e;

typedef struct ethernet_s* ethernet_t;

void ethernet_get_MACaddress(char* interfaceId, uint8_t* addr);

ethernet_t ethernet_create(char* interfacId, uint8_t* destAddress);

void ethernet_set_mode(ethernet_t ethSocket, ethernet_mode_e mode);

int ethernet_set_multicast_address(ethernet_t ethSocket, uint8_t* multicastAddress);

int ethernet_set_protocolfilter(ethernet_t ethSocket, uint16_t etherType);

void ethernet_destroy(ethernet_t ethSocket);

void ethernet_sendpacket(ethernet_t ethSocket, uint8_t* buffer, int packetsize);

void ethernet_receivepacket(ethernet_t self, uint8_t* buffer, int buffersize);
#endif