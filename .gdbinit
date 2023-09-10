set history filename ./.gdb_history
set history save on
set logging enable

target remote 192.168.117.178:1234
set disassemble-next-line on
set disassembly-flavor intel

