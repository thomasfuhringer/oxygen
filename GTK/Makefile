# Oxygen Makefile

DEPS    = ./Oxygen/*.h
OBJS	= ./Obj/*.o
FINAL	= ./Hello/hello
CC	    = gcc
LD		= gcc

CFLAGS	= `pkg-config --cflags gtk+-3.0`
LDFLAGS	= -o $(FINAL)  $(OBJS) `pkg-config --libs gtk+-3.0`


Obj/%.o: Oxygen/%.c   $(DEPS)
	$(CC) $(CFLAGS)  -I Oxygen -c -o $@ $<
    
Obj/%.o: Hello/%.c   $(DEPS)
	$(CC) $(CFLAGS)  -I Oxygen -c -o $@ $<


$(FINAL): $(OBJS)
	$(LD) $(LDFLAGS) 

clean:
	rm ./Obj/*.o

run: $(FINAL)
	 /lib64/ld-linux-x86-64.so.2 $(FINAL)
    
