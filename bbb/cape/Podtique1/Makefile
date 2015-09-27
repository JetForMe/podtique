







all: LZ-P1-00A0.dtbo BB-ENABLE-PRU-00A0.dtbo
	
LZ-P1-00A0.dtbo: LZ-P1.dts  Makefile
	cpp -Wp,-MD,$(<).dep -nostdinc -I/home/debian/dtb-rebuilder/include -undef -x assembler-with-cpp -o $(<).tmp $(<)
	dtc -O dtb -b 0 -@ -I dts -o $(@) $(<).tmp
	sudo cp $(@) /lib/firmware

BB-ENABLE-PRU-00A0.dtbo: BB-ENABLE-PRU.dts Makefile
	cpp -Wp,-MD,$(<).dep -nostdinc -I/home/debian/dtb-rebuilder/include -undef -x assembler-with-cpp -o $(<).tmp $(<)
	dtc -O dtb -b 0 -@ -I dts -o $(@) $(<).tmp
	sudo cp $(@) /lib/firmware

clean:
	rm -f *.dtbo *.dep *.tmp
	
