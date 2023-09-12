//
// Created by shei on 9/9/23.
//

#include <stdlib.h>
#include <stdio.h>
#include "DNSQuery.h"

unsigned char* print_name(unsigned char* msg, unsigned char* p, unsigned char* end);
void print_full_dns_message(char* message, int messageLen);

DNSQuery * dns_query_constructor() {
    DNSQuery * dnsQuery = (DNSQuery*)calloc(1, sizeof(DNSQuery));
    if (!dnsQuery) {
        perror("[-] error allocating memory for dns query struct");
        return NULL;
    }

    dnsQuery->print_name = print_name;
    dnsQuery->print_full_dns_message = print_full_dns_message;
    return dnsQuery;
}

void dns_query_deconstructor(DNSQuery* dnsQuery) {
    free(dnsQuery);
}

unsigned char* print_name(unsigned char* msg, unsigned char* p, unsigned char* end) {
    if (((p + 2) > end)) {
        fprintf(stderr, "[-] End of message\n");
        return NULL;
    }

    if ((*p & 0xc0) == 0xc0) {
        const int k = ((*p & 0xcf) << 8) + p[1];
        p += 2;
        printf("[debug] (pointer %d)\n", k);
        print_name(msg, msg + k, end);
        return p;
    } else {
        const int len = *p++;
        if ((p + len + 1) > end) {
            fprintf(stderr, "[-] End of message\n");
            return NULL;
        }

        printf("%.*s", len, p);
        p += len;

        if (*p) {
            printf(".");
            return print_name(msg, p, end);
        } else {
            return p + 1;
        }
    }
}

void print_full_dns_message(char* message, int messageLen) {
    if (messageLen < 12 ) {
        fprintf(stderr, "[-] invalid message. too short\n");
        return;
    }

    const unsigned char* msg = (unsigned char*)message;
    for (int i = 0; i < messageLen; ++i) {
        unsigned char r = msg[i];
        printf("%02d:   %02X %03d '%c'\n", i, r, r, r);
    }
    printf("\n");

    printf("ID = %0X %0X\n", msg[0], msg[1]);
    const int qr = (msg[2] & 0x80) >> 7;
    printf("QR = %d %s\n",  qr, qr ? "response" : "query");

    const int opCode = (msg[2] & 0x78) >> 3;
    printf("OPCode = %d ", opCode);

    switch (opCode) {
        case 0:
            printf("standard\n");
            break;
        case 1:
            printf("reverse\n");
            break;
        case 2:
            printf("status\n");
            break;
        default:
            printf("???\r");
            break;
    }

    const int aa = (msg[2] & 0x04) >> 2;
    printf("AA = %d %s\n", aa, aa ? "Authoritative" : "");

    const int tc = (msg[2] & 0x02) >> 1;
    printf("TC = %d %s\n", tc, tc ? "Truncated" : "");

    const int rd = (msg[2] & 0x01);
    printf("RD = %d %s\n", rd, rd ? "Recursion Desired" : "");
}