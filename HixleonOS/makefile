oArgs:=-I include -nostdlib -nostdinc -nodefaultlibs -nostartfiles -fPIC -fPIE -O0 -g
eArgs:=-nostdlib -nostdinc -nodefaultlibs -nostartfiles -fPIC -fPIE -O0 -g -T linker.ld
oDir:=obj
cmd:=i686-elf-g++

src := $(shell find ./src/ -maxdepth 7 -type d)

allCpp += $(foreach dir,${src}, $(wildcard ${dir}/*.cpp))
allS += $(foreach dir,${src}, $(wildcard ${dir}/*.S))

oCpp += $(patsubst %.cpp, %.o, ${allCpp})
oS += $(patsubst %.S, %.o, ${allS})

OBJS += $(addprefix ${oDir}/, ${oCpp})
OBJS += $(addprefix ${oDir}/, ${oS})

Kernel: clear mkdir ${OBJS}
	${cmd} ${OBJS} -o KERNEL ${eArgs}

${oDir}/%.o: %.cpp
	${cmd} -c $< -o $@ ${oArgs}

${oDir}/%.o: %.S
	${cmd} -c $< -o $@ ${oArgs}

mkdir: ${oDir}
	mkdir $(addprefix ${oDir}/, ${src})

${oDir}:
	mkdir ${oDir}

clear:
	rm -rf ${oDir}
	rm -rf KERNEL
