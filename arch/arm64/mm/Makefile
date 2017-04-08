CFLAGS_KERNEL :=-fno-graphite \
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

obj-y				:= dma-mapping.o extable.o fault.o init.o \
				   cache.o copypage.o flush.o \
				   ioremap.o iomap.o mmap.o pgd.o mmu.o \
				   context.o proc.o pageattr.o
obj-$(CONFIG_HUGETLB_PAGE)	+= hugetlbpage.o
obj-$(CONFIG_ARM64_PTDUMP)	+= dump.o
