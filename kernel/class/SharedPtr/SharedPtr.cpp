#include "SharedPtr.h"

RefCount *SharedRefCount = nullptr;

void initRefCount() {
    if (SharedRefCount == nullptr)
        SharedRefCount = new RefCount;
}
void destroyRefCount() {
    delete SharedRefCount;
    SharedRefCount = nullptr;
}
