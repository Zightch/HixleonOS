debug:=-g
libs:=
commonArgs:=-nostdlib -nostdinc -nodefaultlibs -nostartfiles -O0 -static -msse -mfpmath=sse
oArgs:=-I include -I class -I ../boot ${commonArgs} ${debug} ${libs}
eArgs:=${commonArgs} -T linker.ld ${debug} ${libs}
oDir:=obj
cmd:=i686-elf-g++

src := $(shell find ./src/ -maxdepth 7 -type d)
src += $(shell find ./class/ -maxdepth 7 -type d)

allCpp += $(foreach dir,${src}, $(wildcard ${dir}/*.cpp))
allS += $(foreach dir,${src}, $(wildcard ${dir}/*.S))

oCpp += $(patsubst %.cpp, %.o, ${allCpp})
oS += $(patsubst %.S, %.o, ${allS})

OBJS += $(addprefix ${oDir}/, ${oCpp})
OBJS += $(addprefix ${oDir}/, ${oS})

Kernel: clean mkdir ${OBJS}
	${cmd} ${OBJS} -o Kernel.bin ${eArgs}
	objcopy --strip-debug Kernel.bin KERNEL
	objcopy --only-keep-debug Kernel.bin Kernel.symbol
	rm -rf Kernel.bin

${oDir}/%.o: %.cpp
	${cmd} -c $< -o $@ ${oArgs}

${oDir}/%.o: %.S
	${cmd} -c $< -o $@ ${oArgs}

mkdir: ${oDir}
	mkdir $(addprefix ${oDir}/, ${src})

${oDir}:
	mkdir ${oDir}

clean:
	rm -rf ${oDir}
	rm -rf KERNEL
	rm -rf Kernel.symbol
