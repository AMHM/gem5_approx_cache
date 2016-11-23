BUILD_CORES=8

all: 
		#scons -j $(BUILD_CORES) build/ARM/gem5.debug
		scons -j $(BUILD_CORES) build/ARM/gem5.opt
		#scons -j $(BUILD_CORES) build/ALPHA/gem5.debug
		#scons -j $(BUILD_CORES) build/ALPHA/gem5.opt

clean:
		rm -rf  build
