debug:=-g
oArgs:=-I include -nostdlib -nostdinc -nodefaultlibs -nostartfiles -O0 -static ${debug}
eArgs:=-nostdlib -nostdinc -nodefaultlibs -nostartfiles -O0 -T linker.ld -static ${debug}
oDir:=obj
cmd:=i686-elf-g++

src := $(shell find ./src/ -maxdepth 7 -type d)

allCpp += $(foreach dir,${src}, $(wildcard ${dir}/*.cpp))
allS += $(foreach dir,${src}, $(wildcard ${dir}/*.S))

oCpp += $(patsubst %.cpp, %.o, ${allCpp})
oS += $(patsubst %.S, %.o, ${allS})

OBJS += $(addprefix ${oDir}/, ${oCpp})
OBJS += $(addprefix ${oDir}/, ${oS})

Kernel: clean mkdir ${OBJS}
	${cmd} ${OBJS} -o kernel.bin ${eArgs}

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
