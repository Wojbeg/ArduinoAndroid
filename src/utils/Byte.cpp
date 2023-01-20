#include "Byte.h"

Byte charToByte(unsigned char c)
{
    return c;
}

Byte charToByte(signed char c)
{
    return c < 0 ? c + 256 : c;
}
