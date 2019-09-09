all:
	gcc imageviewer.c -o imageviewer `pkg-config --cflags --libs gtk+-3.0` -w
