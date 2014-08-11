#include <sys/Mutex.h>

namespace itc {
    namespace global {
        itc::sys::Mutex gGAINTLock;
    }
}

void accuireGAINTLock() {
    itc::global::gGAINTLock.Lock();
}

void releaseGAINTLock() {
    itc::global::gGAINTLock.unLock();
}
