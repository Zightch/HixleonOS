#include "ACPI.h"
#include "tty.h"
#include "kernel.h"
#include "cpu.h"

namespace ACPI{
    #define INVALID_REDP (RSDP*)0xFFFFFFFF
    RSDP *rsdp = INVALID_REDP;
    void findRSDP() {
        for (unsigned short i = 0; i < memMapSize; i++) {
            if (memMap[i].type != 2) continue;
            if (memMap[i].base >= 0x1000000) break;
            unsigned int base = memMap[i].base;
            unsigned int size = memMap[i].size;
            for (unsigned int j = base; j < base + size; j += 16) {
                RSDP *tmp = (RSDP*)j;
                if (tmp->Signature == ACPI_RSDP_SIG)
                    rsdp = tmp;
            }
        }
    }
    RSDP getRSDP() {
        if (rsdp == INVALID_REDP)
            return {};
        return *rsdp;
    }
    void init() {
        findRSDP();
        if (rsdp == INVALID_REDP) {
            ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "Error, ACPI_RSDP not fount\n");
            hlt();
        }
        
    }

}
