cmake_minimum_required(VERSION 3.30)
project(HixleonOS)

set(CMAKE_CXX_STANDARD 14)

include_directories(boot/grub)
include_directories(kernel/class)
include_directories(kernel/include)

add_executable(
        HixleonOS
        kernel/class/ByteArray/ByteArray.cpp
        kernel/class/SharedPtr/SharedPtr.cpp
        kernel/class/Map.hpp
        kernel/src/int/APIC/LAPIC/LAPIC.cpp
        kernel/src/int/APIC/LAPIC/timer.cpp
        kernel/src/int/APIC/APIC.cpp
        kernel/src/int/APIC/IOAPIC.cpp
        kernel/src/int/asmIsr.S
        kernel/src/int/intTable.cpp
        kernel/src/memManager/kernelMem.cpp
        kernel/src/memManager/memManager.cpp
        kernel/src/memManager/physMem.cpp
        kernel/src/memManager/virtMem.cpp
        kernel/src/tools/numTools.cpp
        kernel/src/ACPI.cpp
        kernel/src/asmKernel.S
        kernel/src/boot.S
        kernel/src/cpu.cpp
        kernel/src/crash.cpp
        kernel/src/GDT.cpp
        kernel/src/hhkInit.cpp
        kernel/src/IDT.cpp
        kernel/src/kernel.cpp
        kernel/src/tty.cpp
)
