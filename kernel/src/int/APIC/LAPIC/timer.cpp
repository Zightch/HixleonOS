#include "int/APIC/LAPIC/timer.h"
#include "int/APIC/LAPIC/LAPIC.h"
#include "int/fixedIntNum.h"

namespace LAPIC {
    namespace Timer {
        void init() {
            reg(LAPIC_LVT_TIMER) = LAPIC_INTNUM_TIMER;
        }
    }
}
