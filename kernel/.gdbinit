set mi-async on
set history filename ./.gdbhistory
set history save on
set logging enable

set disassemble-next-line on
set disassembly-flavor intel
target remote 172.24.0.1:1234
