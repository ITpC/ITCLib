#ifndef GAINTLOCK_H_
#define GAINTLOCK_H_

#include <globals.h>

class GAINTLock
{
public:
    explicit GAINTLock()
    {
        accuireGAINTLock();
    }
    ~GAINTLock()
    {
        releaseGAINTLock();
    }
};

#endif /*GAINTLOCK_H_*/
