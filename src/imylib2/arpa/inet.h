

// I have no idea if this will conflict with some non-unix system.
// But this works for cross compilation to Windows at least.
#ifndef __unix__

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);

#endif
