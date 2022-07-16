#include "ethernet.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <asm-generic/socket.h>
#include <linux/filter.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct ethernet_s
{
    int rawSocket;
    bool isBind;
    struct sockaddr_ll socketAddress;
};

static int get_interface_index(int socketfd, char* devicename)
{
    struct ifreq ifr;

    strncpy(ifr.ifr_name, devicename, IFNAMSIZ - 1);

    if (ioctl(socketfd, SIOCGIFINDEX, &ifr) == -1)
    {
        return -1;
    }

    return ifr.ifr_ifindex;
}

void ethernet_get_MACaddress(char* interfaceId, uint8_t* addr)
{
    struct ifreq buffer;

    int fd = socket(PF_INET, SOCK_DGRAM, 0);

    memset(&buffer, 0, sizeof(buffer));

    strncpy(buffer.ifr_name, interfaceId, IFNAMSIZ - 1);

    ioctl(fd, SIOCGIFHWADDR, &buffer);

    close(fd);

    for (int i = 0; i < 6; i++)
    {
        addr[i] = (uint8_t)buffer.ifr_hwaddr.sa_data[i];
    }
}

ethernet_t ethernet_create(char* interfacId, uint8_t* destAddress)
{
    ethernet_t ethernetSocket = malloc(sizeof(struct ethernet_s));

    if (!ethernetSocket)
    {
        return NULL;
    }

    ethernetSocket->rawSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (ethernetSocket->rawSocket == -1)
    {
        printf("Error creating raw socket!\r\n");
        free(ethernetSocket);
        return NULL;
    }

    ethernetSocket->socketAddress.sll_family = PF_PACKET;
    ethernetSocket->socketAddress.sll_protocol = htons(ETH_P_IP);

    int ifcIdx = getInterfaceIndex(ethernetSocket->rawSocket, interfacId);

    if (ifcIdx == -1)
    {
        close(ethernetSocket->rawSocket);
        free(ethernetSocket);
        return NULL;
    }

    ethernetSocket->socketAddress.sll_ifindex = ifcIdx;
    ethernetSocket->socketAddress.sll_hatype = ARPHRD_ETHER;
    ethernetSocket->socketAddress.sll_pkttype = PACKET_HOST | PACKET_MULTICAST;
    ethernetSocket->socketAddress.sll_halen = ETH_ALEN;

    memset(ethernetSocket->socketAddress.sll_addr, 0, 8);

    if (destAddress != NULL)
    {
        memcpy(ethernetSocket->socketAddress.sll_addr, destAddress, 6);
    }

    ethernetSocket->isBind = false;

    return ethernetSocket;
}

void ethernet_set_mode(ethernet_t ethSocket, ethernet_mode_e mode)
{
    if (mode == ETHERNET_MODE_PROMISC)
    {
        struct ifreq ifr;

        if (ioctl(ethSocket->rawSocket, SIOCGIFFLAGS, &ifr) < 0)
        {
            return;
        }

        ifr.ifr_flags |= IFF_PROMISC;
        if (ioctl(ethSocket->rawSocket, SIOCSIFFLAGS, &ifr) < 0)
        {
            return;
        }
    }
    else if (mode == ETHERNET_MODE_ALL_MULTICAST)
    {
        struct ifreq ifr;

        if (ioctl(ethSocket->rawSocket, SIOCGIFFLAGS, &ifr) < 0)
        {
            return;
        }

        ifr.ifr_flags |= IFF_ALLMULTI;
        if (ioctl(ethSocket->rawSocket, SIOCSIFFLAGS, &ifr) < 0)
        {
            return;
        }
    }
    else if (mode == ETHERNET_MODE_HOST_ONLY)
    {
        ethSocket->socketAddress.sll_pkttype = PACKET_HOST;
    }
    else if (mode == ETHERNET_MODE_MULTICAST)
    {
        ethSocket->socketAddress.sll_pkttype = PACKET_HOST | PACKET_MULTICAST;
    }
}

int ethernet_set_multicast_address(ethernet_t ethSocket, uint8_t* multicastAddress)
{
    struct packet_mreq mreq;
    mreq.mr_ifindex = ethSocket->socketAddress.sll_ifindex;
    mreq.mr_alen = ETH_ALEN;
    mreq.mr_type = PACKET_MR_MULTICAST;
    mreq.mr_address[0] = multicastAddress[0];
    mreq.mr_address[1] = multicastAddress[1];
    mreq.mr_address[2] = multicastAddress[2];
    mreq.mr_address[3] = multicastAddress[3];
    mreq.mr_address[4] = multicastAddress[4];
    mreq.mr_address[5] = multicastAddress[5];

    return setsockopt(ethSocket->rawSocket, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
}

int ethernet_set_protocolfilter(ethernet_t ethSocket, uint16_t etherType)
{
#if 0
    if (etherType == 0x88b8)
    {
        struct sock_fprog fprog;
        struct sock_filter fileter[] = {
            {0x28, 0, 0, 0x0000000c},
            {0x15, 0, 1, 0x000088b8},
            {0x6, 0, 0, 0x00040000},
            {0x6, 0, 0, 0x00000000},
        };

        fprog.len = sizeof(fileter) / sizeof(fileter[0]);
        fprog.filter = fileter;

        return setsockopt(ethSocket->rawSocket, SOL_SOCKET, SO_ATTACH_FILTER, &fprog, sizeof(fprog));
    }
    else
    {
        ethSocket->socketAddress.sll_protocol = htons(etherType);
    }
#endif

    ethSocket->socketAddress.sll_protocol = htons(etherType);
    return 0;
}

void ethernet_destroy(ethernet_t ethSocket)
{
    close(ethSocket->rawSocket);
    free(ethSocket);
}

void ethernet_sendpacket(ethernet_t ethSocket, uint8_t* buffer, int packetsize)
{
    sendto(ethSocket->rawSocket, buffer, packetsize, 0,
        (struct sockaddr*)&(ethSocket->socketAddress), sizeof(ethSocket->socketAddress));
}

void ethernet_receivepacket(ethernet_t self, uint8_t* buffer, int buffersize)
{
    if (self->isBind == false)
    {
        bind(self->rawSocket, &self->socketAddress, sizeof(self->socketAddress));
        self->isBind = true;
    }

    return recvfrom(self->rawSocket, buffer, buffersize, MSG_DONTWAIT, 0, 0);
}
