#Configure next parameter
TULIO_LIBS=../../../../libs/libtulio/

INCLUDES = -I$(TULIO_LIBS)/hal/include \
	-I$(TULIO_LIBS)/libcc1110/include \
	-I../include 

LIB_DIR = -L$(TULIO_LIBS)/libcc1110 \
	-L$(TULIO_LIBS)/hal

LIB = libcc1110_3.lib hal.lib 

LFLAGS = --xram-loc 0xF000 --code-loc 0x0000
CFLAGS = --model-large --less-pedantic -Dchip=1110 -DROUTE_NOT_WAIT_ACK $(NODEADDR) $(NODECHANNEL) $(NODEBRIGHT) $(NODEPOWER) $(NODESAMPLE) $(NODETX) $(NODEV) $(NPRESS_SENSOR_NUM) $(NODECURR_SENSORS)
CC = sdcc
LD = aslink


TARGETS= 6term-1h.bin \
	6term.bin \
	curr.bin \
	1pres.bin \
	6curr-test.bin \
	board-test.bin
#do not delete this line

#SETTING DEVICE ADDR. CHOOSING KIND OF SENSORS
ifdef TH_ADDR
NODEADDR=-DTH_ADDR=$(TH_ADDR)
endif
ifdef T6_ADDR
NODEADDR=-DT6_ADDR=$(T6_ADDR)
endif
ifdef CURR_ADDR
NODEADDR=-DCURR_ADDR=$(CURR_ADDR)
endif
ifdef PRESS_ADDR
NODEADDR=-DPRESS_ADDR=$(PRESS_ADDR)
endif

#SETTING DEVICE ADDR. ABSOLUTE ADDRESSING
ifdef ABS_ADDR
NODEADDR=-DABS_ADDR=$(ABS_ADDR)
endif

#SETTING DEVICE CHANNEL
ifdef RF_CH
NODECHANNEL=-DRF_CH=$(RF_CH)
endif

#CHOOSING IF LEDS SHINE OR NOT IN TRANSMMITION
ifdef SHINE
NODEBRIGHT=-DSHINE=$(SHINE)
endif

#CHOOSING SAMPLE TIME
ifdef SAMPLE_RATE
NODESAMPLE=-DSAMPLE_RATE=$(SAMPLE_RATE)
endif

#CHOOSING TX RATE
ifdef TX_RATE
NODETX=-DTX_RATE=$(TX_RATE)
endif

#SETTING DEVICE TRANSMISSION POWER
ifdef POWER
NODEPOWER=-DPOWER=$(POWER)
endif

#CHOOSING PCB VERSION
ifdef V 			# V is version 1 o 2 of GreenNodes
NODEV=-DV=$(V)
endif

#INDICATING PRESSURE SENSOR NUMBER
ifdef PRESS_SENSOR_NUM
NPRESS_SENSOR_NUM=-DPRESS_SENSOR_NUM=$(PRESS_SENSOR_NUM)
endif

# INDICATING CURRENT SENSORS AVAILABLE
ifdef CURR_SENSORS
NODECURR_SENSORS=-DCURR_SENSORS=$(CURR_SENSORS)
endif

%.rel: %.c
	$(CC) -c $< $(INCLUDES) $(CFLAGS) -o $@


all: $(TARGETS)


%.hex: %.ihx
	packihx $< > $@

%.ihx: %.c
	$(CC) $(LIB) $< $(CFLAGS) $(INCLUDES) $(LIB_DIR) $(LFLAGS)

%.bin: %.hex
	objcopy --gap-fill 0xFF -I ihex -O binary $< $@

clean:
	$(RM) *.rel *.asm *.lst *.sym *.hex *.ihx *.rst *.mem *.map *.lnk *.omf *.cdb *.lk 

mrproper: clean
	$(RM) *.bin

