
// We are overriding this header since imlib2 reqiures this, and it doesn't exist on all systems

#include <stdint.h>

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);

