#define MB_MAGIC 0x1BADB002
#define MB_FLAGS 0x03
#define CHECKSUM(flags) ( - (MB_MAGIC + flags))