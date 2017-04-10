CFLAGS_KERNEL := -fno-graphite \
                 -fno-graphite-identity \
                 -fno-loop-flatten \
                 -fno-tree-loop-linear \
                 -fno-loop-interchange \
                 -fno-loop-strip-mine \
                 -fno-loop-block \
                 -fno-loop-nest-optimize \
                 -fno-loop-unroll-and-jam \
                 -fno-loop-parallelize-all \
                 -ftree-parallelize-loops=0 \
                 -fno-openmp \
                 -Wno-unused-function
obj-$(CONFIG_STATE_HELPER)		  += state_helper.o
obj-$(CONFIG_ALUCARD_HOTPLUG)   += alucard_hotplug.o
obj-$(CONFIG_ASMP)	              += autosmp.o
obj-$(CONFIG_THUNDERPLUG_CONTROL) += thunderplug.o
