APP		= radio

SRC	=	src

CC		= gcc
CPP		= g++
CFLAGS	= -g -Wall -I. -ISDL -I../libxively/src/libxively -Wno-unknown-pragmas -D_REENTRANT
CPPFLAGS  = $(CFLAGS) -std=gnu++0x
LDFLAGS = -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lao -lmpg123 -lsndfile -lpthread


CORE   = \

COMMON = \
	

CPPSRC = \
	$(SRC)/AudioDevice.cpp \
	$(SRC)/BBBMain.cpp \
	$(SRC)/MP3Decoder.cpp \
	$(SRC)/Radio.cpp \
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
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d
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
	rm -f *.o *.d *.out *.hex $(SRC)/*.o $(SRC)/*.d

xfer:
	rsync -e ssh -avz --exclude="*.d" --exclude="*.o" Makefile SPSDisplayApp.* BBBDisplayApp.* TristarMPPT.* ChargeController.* SDL spa.* fonts beaglebone.local:app
	
