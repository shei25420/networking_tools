//
// Created by shei on 9/9/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include "IPLookup.h"

void lookup_ip (struct ip_lookup* ip_lookup);

IPLookup * ip_lookup_constructor (char* hostname) {
    IPLookup * ip_lookup = (IPLookup*)calloc(1, sizeof(IPLookup));
    if (!ip_lookup) {
        perror("[-] error allocating memory for ip lookup struct\n");
        return NULL;
    }

    ip_lookup->hostname = (char *)calloc(1, strlen(hostname) + 1);
    if (!ip_lookup->hostname) {
        perror("[-] error allocating memory for hostname");
        return NULL;
    }

    strncpy(ip_lookup->hostname, hostname, strlen(hostname));

    ip_lookup->lookup = lookup_ip;
    return ip_lookup;
}

void ip_lookup_deconstructor(IPLookup* ip_lookup) {
    free(ip_lookup->hostname);
    free(ip_lookup);
}

void lookup_ip (struct ip_lookup* ip_lookup) {
    printf("Resolving hostname: %s\n", ip_lookup->hostname);
    struct addrinfo hints = { 0 };
    hints.ai_flags = AI_ALL;

    struct addrinfo *peer_address = 0;
    int addrRet = 0;
    if ((addrRet = getaddrinfo(ip_lookup->hostname, 0, &hints, &peer_address))) {
        printf("[-] something went wrong fetching address: %s\n", gai_strerror(addrRet));
        return;
    }

    struct addrinfo *address = peer_address;
    do {
        char address_buf[100];
        getnameinfo(address->ai_addr, address->ai_addrlen, address_buf, sizeof(address_buf), 0, 0, NI_NUMERICHOST);
        printf("[resolved] %s\n", address_buf);
    } while ((address = address->ai_next));

    freeaddrinfo(peer_address);
}