oArgs+=-g -O0 -I ./src/includes -nostdlib
eArgs+=-g -nostdlib -O0 -T linker.ld
oDir:=o
cmd:=i686-elf-g++

src := $(shell find ./src/ -maxdepth 7 -type d)

allCpp += $(foreach dir,${src}, $(wildcard ${dir}/*.cpp))
allS += $(foreach dir,${src}, $(wildcard ${dir}/*.S))

oCpp += $(patsubst %.cpp, %.o, ${allCpp})
oS += $(patsubst %.S, %.o, ${allS})

OBJS += $(addprefix ${oDir}/, ${oCpp})
OBJS += $(addprefix ${oDir}/, ${oS})

Kernel: mkdir ${OBJS}
	${cmd} ${OBJS} -o KERNEL.BIN ${eArgs}

${oDir}/%.o: %.cpp
	${cmd} -c $< -o $@ ${oArgs}

${oDir}/%.o: %.S
	${cmd} -c $< -o $@ ${oArgs}

mkdir: ${oDir}
	mkdir $(addprefix ${oDir}/, ${src})

clear:
	rm -rf ${oDir}/*
	rm -rf KERNEL.BIN
