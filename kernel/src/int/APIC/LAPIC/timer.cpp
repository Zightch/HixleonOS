#include "int/APIC/LAPIC/timer.h"
#include "int/APIC/LAPIC/LAPIC.h"
#include "int/fixedIntNum.h"
#include "int/intTable.h"

namespace LAPIC {
    namespace Timer {
        namespace TmpFn {
            bool ltDone = false;
            void LT(IsrParam *) {

            }
            void RTC(IsrParam *) {

            }
        }
        void init() {
            reg(LAPIC_LVT_TIMER) = LAPIC_INTNUM_TIMER | LAPIC_TM_ONESHOT | LAPIC_MASK(0);
            reg(LAPIC_TIMER_DCR) = LAPIC_TIMER_DIV64;
            setIntFn(LAPIC_INTNUM_TIMER, TmpFn::LT);

        }
    }
}
