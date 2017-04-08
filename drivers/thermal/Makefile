#
# Makefile for sensor chip drivers.
#
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

obj-$(CONFIG_THERMAL)		+= thermal_sys.o
thermal_sys-y			+= thermal_core.o

# governors
thermal_sys-$(CONFIG_THERMAL_GOV_FAIR_SHARE)	+= fair_share.o
thermal_sys-$(CONFIG_THERMAL_GOV_STEP_WISE)	+= step_wise.o
thermal_sys-$(CONFIG_THERMAL_GOV_USER_SPACE)	+= user_space.o

# cpufreq cooling
thermal_sys-$(CONFIG_CPU_THERMAL)	+= cpu_cooling.o

# platform thermal drivers
obj-$(CONFIG_SPEAR_THERMAL)	+= spear_thermal.o
obj-$(CONFIG_RCAR_THERMAL)	+= rcar_thermal.o
obj-$(CONFIG_KIRKWOOD_THERMAL)  += kirkwood_thermal.o
obj-$(CONFIG_EXYNOS_THERMAL)	+= exynos_thermal.o
obj-$(CONFIG_DOVE_THERMAL)  	+= dove_thermal.o
obj-$(CONFIG_DB8500_THERMAL)	+= db8500_thermal.o
obj-$(CONFIG_ARMADA_THERMAL)	+= armada_thermal.o
obj-$(CONFIG_DB8500_CPUFREQ_COOLING)	+= db8500_cpufreq_cooling.o
obj-$(CONFIG_INTEL_POWERCLAMP)	+= intel_powerclamp.o
obj-$(CONFIG_ARB_THERMAL)	+= arb/msm_thermal.o
obj-$(CONFIG_THERMAL_TSENS8974)	+= msm8974-tsens.o
obj-$(CONFIG_THERMAL_QPNP)	+= qpnp-temp-alarm.o
obj-$(CONFIG_THERMAL_QPNP_ADC_TM)	+= qpnp-adc-tm.o
obj-$(CONFIG_THERMAL_MONITOR)	+= msm_thermal.o msm_thermal-dev.o
obj-$(CONFIG_LIMITS_MONITOR)	+= lmh_interface.o
obj-$(CONFIG_LIMITS_LITE_HW)	+= lmh_lite.o
obj-$(CONFIG_SUPPLY_LM_MONITOR)	+= supply_lm_core.o
