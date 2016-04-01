APP		= radio

SRC	=	src

CC		= gcc
CPP		= g++
CFLAGS	= -g -Wall -I. -I/usr/include/dbus-1.0 -I/usr/lib/arm-linux-gnueabihf/dbus-1.0/include -ISDL -D_REENTRANT -Wno-unknown-pragmas
CPPFLAGS  = $(CFLAGS) -std=gnu++0x
LDFLAGS = -L/usr/local/lib  -L-L. -Wl,-rpath,/usr/local/lib -lao -ldbus-1 -lfaad -lmp4ff -lmpg123 -lprussdrv -lpthread -lsndfile


CORE   = \

COMMON = \
	$(SRC)/ledscape.c \
	$(SRC)/pru.c \
	$(SRC)/util.c \
	
CPPSRC = \
	$(SRC)/AACDecoder.cpp \
	$(SRC)/AudioDevice.cpp \
	$(SRC)/BBBMain.cpp \
	$(SRC)/DBUS.cpp \
	$(SRC)/DBusMessage.cpp \
	$(SRC)/Decoder.cpp \
	$(SRC)/GPIO.cpp \
	$(SRC)/MP3Decoder.cpp \
	$(SRC)/Radio.cpp \
	$(SRC)/RadioDBUS.cpp \
	$(SRC)/Spectrum.cpp \
	$(SRC)/SndFile.cpp \


CORE_OBJ   = $(CORE:.c=.o)
COMMON_OBJ = $(COMMON:.c=.o)
CPP_OBJ    = $(CPPSRC:.cpp=.o)

# create firmware image from common objects and example source file
all: $(APP)
	
$(APP): $(CORE_OBJ) $(COMMON_OBJ) $(CPP_OBJ)
	$(CPP) $^ $(LDFLAGS) -o $@

# pull in dependency info for *existing* .o files
-include $(CORE_OBJ:.o=.d)
-include $(COMMON_OBJ:.o=.d)
-include $(CPP_OBJ:.o=.d)

# compile and generate dependency info
%.o: %.c
	$(CC) -c $(CFLAGS) -std=gnu99 $*.c -o $*.o
	$(CC) -MM $(CFLAGS) -std=gnu99 $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) $*.cpp -o $*.o
	$(CPP) -MM $(CPPFLAGS) $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

clean:
	rm -f $(APP) *.o *.d *.out *.hex $(SRC)/*.o $(SRC)/*.d

xfer:
	rsync -e ssh -avzu --exclude="*.d" --exclude="*.o" Makefile src rmann@arm.local:radio
	
dtbo: podtique.dts
	dtc -O dtb -o podtique-00A0.dtbo -b 0 -@ podtique.dts
	sudo cp podtique-00A0.dtbo /lib/firmware
	
