MLSC_OBJS := main.o network.o
MLSC_CFLAGS := $(CFLAGS) -std=gnu99 -fstrict-aliasing -Iinclude
MLSC_LDFLAGS := $(LDFLAGS) -L. -lpolarssl
MLSC_DATA_BACKEND := $(BACKENDS)

ifneq ($(filter iwinfo,$(MLSC_DATA_BACKEND)),)
	MLSC_LDFLAGS += -liwinfo
	MLSC_CFLAGS += -DUSE_IWINFO
	MLSC_OBJS += backend_iwinfo.o
endif

%.o: %.c
	$(CC) $(MLSC_CFLAGS) $(FPIC) -c -o $@ $<

compile: clean $(MLSC_OBJS)
	$(CC) $(MLSC_LDFLAGS) -o mlsc $(MLSC_OBJS)

clean:
	rm -f *.o mlsc
