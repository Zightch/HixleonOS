commonArgs:=-nostdlib -nostdinc -nodefaultlibs -nostartfiles -O0 -static -fpermissive -g
oArgs:=-I ./kernel/include -I ./kernel/class -I ./boot ${commonArgs}
eArgs:=-T ./kernel/linker.ld ${commonArgs}
oDir:=./kernel/obj
cmd:=i686-elf-g++

src := $(shell find ./kernel/src/ -maxdepth 15 -type d)
src += $(shell find ./kernel/class/ -maxdepth 15 -type d)

allCpp += $(foreach dir,${src}, $(wildcard ${dir}/*.cpp))
allS += $(foreach dir,${src}, $(wildcard ${dir}/*.S))

oCpp += $(patsubst %.cpp, %.o, ${allCpp})
oS += $(patsubst %.S, %.o, ${allS})

OBJS += $(addprefix ${oDir}/, ${oCpp})
OBJS += $(addprefix ${oDir}/, ${oS})

Kernel: clean mkdir ${OBJS}
	${cmd} ${OBJS} -o ./kernel/Kernel.bin ${eArgs}
	objcopy --strip-debug ./kernel/Kernel.bin ./kernel/KERNEL
	objcopy --only-keep-debug ./kernel/Kernel.bin ./kernel/Kernel.symbol
	rm -rf ./kernel/Kernel.bin

${oDir}/%.o: %.cpp
	${cmd} -c $< -o $@ ${oArgs}

${oDir}/%.o: %.S
	${cmd} -c $< -o $@ ${oArgs}

mkdir: ${oDir}
	mkdir -p $(addprefix ${oDir}/, ${src})

${oDir}:
	mkdir -p ${oDir}

clean:
	rm -rf ${oDir}
	rm -rf ./kernel/KERNEL
	rm -rf ./kernel/Kernel.symbol
