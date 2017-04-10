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
obj-$(CONFIG_MSM_MDSS_PLL) += mdss-pll-util.o
obj-$(CONFIG_MSM_MDSS_PLL) += mdss-pll.o
obj-$(CONFIG_MSM_MDSS_PLL) += mdss-dsi-pll-util.o mdss-dsi-20nm-pll-util.o
obj-$(CONFIG_MSM_MDSS_PLL) += mdss-dsi-pll-28hpm.o
obj-$(CONFIG_MSM_MDSS_PLL) += mdss-dsi-pll-28lpm.o
obj-$(CONFIG_MSM_MDSS_PLL) += mdss-dsi-pll-20nm.o
obj-$(CONFIG_MSM_MDSS_PLL) += mdss-edp-pll-28hpm.o
obj-$(CONFIG_MSM_MDSS_PLL) += mdss-hdmi-pll-28hpm.o
obj-$(CONFIG_MSM_MDSS_PLL) += mdss-hdmi-pll-20nm.o
