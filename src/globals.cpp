#include <mutex>

namespace itc {
    namespace global {
        static std::mutex gGAINTLock;
    }
}

void accuireGAINTLock() {
    itc::global::gGAINTLock.lock();
}

void releaseGAINTLock() {
    itc::global::gGAINTLock.unlock();
}
