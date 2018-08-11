CFLSGS = -I/usr/local/include/SDL2
LDFLAGS = `sdl2-config --libs` -lSDL2_image
ALL_OBJS = tools.o parts.o places.o outer.o main.o game.o
game: $(ALL_OBJS)
	$(CC) -Wall $(LDFLAGS) -o game $(ALL_OBJS)
%.o: %.c %.h game.h
	$(CC) -Wall -c -o $@ $<
clean:
	rm -f *.o
