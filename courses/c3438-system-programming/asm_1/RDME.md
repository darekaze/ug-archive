# COMP3438 Assignment 1

JAHJA Darwin, 16094501d

## Instructions

1. Put the 'jahja_adc.c' file in 'arm-board/linux-3.0.8/drivers/char/'

2. Open the 'arm-board/linux-3.0.8/drivers/char/Makefile' and add the following line at the end.

```tex
obj-$(CONFIG_JAHJA_ADC) += jahja_adc.o
```

3. Open the "arm-board/linux-3.0.8/drivers/char/Kconfig" and add the following lines at the end before "endmenu".

```tex
...

config JAHJA_ADC
    tristate "JAHJA_ADC"
    depends on CPU_S5PV210

endmenu
```

4. In the terminal, run:

```bash
cd ~/arm-board/linux-3.0.8
make menuconfig
```

5. In the menu, Go to *Device Drivers > character devices > JAHJA_ADC*. Press 'M', save and exit.

6. Run `make` to compile the driver. It will create the driver files in "arm-board/linux-3.0.8/drivers/char"

7. Open putty and mount "/home/comp3438/arm-board" to the embedded board. And dynamically load the driver in kernel.

```bash
# Run in putty terminal
mount –t nfs 192.168.1.1:/home/comp3438/arm-board /mnt/nfs –o nolock
cd /mnt/nfs/linux-3.0.8/drivers/char
insmod jahja_adc.ko
```

8. Link the driver to a device file.

```bash
mknod /dev/jahja_adc c 250 1
```

Now, the device is attached to the file. we can read from the file using unix system calls.

9. Put "jahja_app.c" in 'arm-board/' and compile the code.

```bash
cd arm-board/
arm-linux-gnueabi-gcc -o jahja_app jahja_app.c
```

3. In putty terminal, go to "/mnt/nfs" and run the compiled file.

```bash
cd /mnt/nfs/
./jahja_app
```
