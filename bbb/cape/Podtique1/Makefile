







all:
	dtc -O dtb -o P1-00A0.dtbo -b 0 -@ P1.dts
	dtc -O dtb -o BB-ENABLE-PRU-00A0.dtbo -b 0 -@ BB-ENABLE-PRU.dts
	#dtc -O dtb -o LZ-PODTIQUE1-00A0.dtbo -b 0 -@ LZ-PODTIQUE1.dts
	sudo cp P1-00A0.dtbo BB-ENABLE-PRU-00A0.dtbo /lib/firmware
	
