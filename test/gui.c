#include <stdio.h>
#include <ui.h>
#include <Windows.h>

int onClosing(uiWindow *w, void *data)
{
	uiQuit();
	return 1;
}

int main(int argc, char *argv[]) 
{
	uiInitOptions o = {0};
	const char *err;
	uiWindow *w;
	uiLabel *l;
	//uiWindowKeepAbove(w);
	err = uiInit(&o);
	if (err != NULL) {
		fprintf(stderr, "Error initializing libui-ng: %s\n", err);
		uiFreeInitError(err);
		return 1;
	}

	// Create a new window
	w = uiNewWindow("Hello World!", 300, 30, 0);
	uiWindowOnClosing(w, onClosing, NULL);

	l = uiNewLabel("Hello, World!");
	uiWindowSetChild(w, uiControl(l));

	uiControlShow(uiControl(w));
	uiMain();
	uiUninit();
	return 0;
}
