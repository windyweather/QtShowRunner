/*
 * OS Specific Mouse Click Solutions
 * And failures of those solutions
 */

#include <QMainWindow>

// stub this module out to leave it for documentation of failure
#if 0
//#ifdef Q_OS_LINUX
#include <QDebug>
#include <QString>

#include <mouseclick.h>

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif // stub out

// These are still called, but do nothing for solutions that work.
// leave them in for some future where X11 or some other solution works.
// open and close do nothing.
// we are going to open the display each time we click.

bool mouseopen()
{
    return true;
}

bool mouseclose()
{
    return true;
}

//
// this does work on windows
//
#ifdef Q_OS_WINDOWS // Windows Only
// found on the internet, sigh
// Because the SendInput function is only supported in
// Windows 2000 and later, WINVER needs to be set as
// follows so that SendInput gets defined when windows.h
// is included below.
#define WINVER 0x0500
#include "Windows.h"

//
// Set the slide show to not advance on mouse clicks
// and then just click away every second until at the
// end of the show, it terminates and exits itself.
// What we do to make it work. Wheeew.
//
bool mouseclick()
{
    INPUT Inputs[3];

    Inputs[0] = {}; // probably a better way to zero out this array
    Inputs[1] = {};
    Inputs[2] = {};

    Inputs[0].type = INPUT_MOUSE;
    // I'm not clear on coords that this uses, but 500 is not very far,
    // so use 2000. Clearly it's not pixels.
    Inputs[0].mi.dx = 2000; // desired X coordinate
    Inputs[0].mi.dy = 2000; // desired Y coordinate
    Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    Inputs[1].type = INPUT_MOUSE;
    Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    Inputs[2].type = INPUT_MOUSE;
    Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(3, Inputs, sizeof(INPUT));
    return true;
}

#endif // Q_OS_WINDOWS




// never needed for solutions that work.
// This is what we tried for Q_OS_LINUX
// It never worked and no clue why
#if 0
//
// Let's try the more straightforward X11 API to click the mouse
// This example code was harvested from the internet
//
// https://www.linuxquestions.org/questions/programming-9/simulating-a-mouse-click-594576/
//
// Another link is here
// https://stackoverflow.com/questions/27984220/x11-sending-a-mouse-click-without-moving-a-pointer
//
bool mouseclick()
{

    //xLookAtWindows();


    int button = Button1; // just click button one. I'm sure it will be fine.
    Display *display = XOpenDisplay(NULL);

    XEvent event;

    if(display == NULL)
    {
        qDebug() << QString("XOpenDisplay returned NULL");
        return false;
        //fprintf(stderr, "Errore nell'apertura del Display !!!\n");
        //exit(EXIT_FAILURE);
    }

    memset(&event, 0x00, sizeof(event));

    event.type = ButtonPress;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;

    XQueryPointer(display, RootWindow(display, DefaultScreen(display)),
                  &event.xbutton.root, &event.xbutton.window,
                  &event.xbutton.x_root, &event.xbutton.y_root,
                  &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;
    qDebug() << "Mouse Window " << event.xbutton.window;

#if 0 // don't look for a subwindow. Just use display. Is this Root?
    while(event.xbutton.subwindow)
    {
        qDebug()<<"Mouse Subwindow "<<event.xbutton.subwindow;
        event.xbutton.window = event.xbutton.subwindow;

        XQueryPointer(display, event.xbutton.window, &event.xbutton.root,
                      &event.xbutton.subwindow, &event.xbutton.x_root,
                      &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }
#endif

    qDebug() << "Mouse Window " << event.xbutton.window;

    // lets try to GET the input focus to see if that helps
    int revert;
    if ( XGetInputFocus(display, &event.xbutton.window, &revert) == 0 )
    {
        qDebug() << QString("XGetInputFocus error ")<<errno;
    }
    qDebug() << "XGetInputFocus window: "<<event.xbutton.window;
    // just set subwindow to the window as well
    //event.xbutton.subwindow = event.xbutton.window;
    event.xbutton.subwindow = 0; // try to set subwindow to nothing and see what happens

    // make this a key release of KEY_ESC and see what happens
    event.type = KeyPress;
    event.xkey.state = ShiftMask;
    event.xkey.keycode = 1; // looked it up on google KEY_ESC;
    event.xkey.same_screen = True;

    if(XSendEvent(display, event.xbutton.window, True, 0xfff, &event) == 0)
    {
        qDebug() << QString("XSendEvent error XS1 ")<<errno;
        //fprintf(stderr, "Errore nell'invio dell'evento !!!\n");
    }
    else
    {
        qDebug() << "ESC KeyPress"; //"Button Press";
    }

    XFlush(display);

    usleep(100000);

    // just do it twice
    //event.type = ButtonRelease;
    //event.xbutton.state = 0x100;
    event.type = KeyRelease;

    if(XSendEvent(display, event.xbutton.window, True, 0xfff, &event) == 0)
    {
        qDebug() << QString("XSendEvent error XS2");
        //fprintf(stderr, "Errore nell'invio dell'evento !!!\n");
    }
    else
    {
        qDebug() << "ESC KeyRelease"; //"Button Release";
    }

    XFlush(display);

    XCloseDisplay(display);
    return true;
}


#endif // never worked






// This is a kernel API solution for Linux that never worked
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <signal.h>

#include <QDebug>

#if 0 // not really going to die
#define die(str, args...) do { \
        perror(str); \
        exit(EXIT_FAILURE); \
    } while(0)
#endif


 static   int                    fd;
#if 0
static void signal_handler(int signo)
{
    printf("\nCaught SIGINT\n");
        if(ioctl(fd, UI_DEV_DESTROY) < 0)
           die("error: cannot destroy uinput device\n");
    else printf("Destroyed uinput_user_dev\n\n");
    close(fd);
    exit(EXIT_SUCCESS);
}
#endif
// try to report the error via Qt console
#define die(str) {qDebug()<<str; qDebug()<<errno;return false;}

void mouseemit(int fd, int type, int code, int val)
{
   struct input_event ie;

   ie.type = type;
   ie.code = code;
   ie.value = val;
   /* timestamp values below are ignored */
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fd, &ie, sizeof(ie));
}



bool mouseopen(void)
{
#if 0 // first try at code
    struct uinput_user_dev uidev;
    //struct input_event     ev;
    //int                    x, y;
    //int                    i;

    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(fd < 0)
        die("error: open");

    qDebug() << "Mouse Open Worked fd="<<fd;
    if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        die("error: ioctl o1");
   // if(ioctl(fd, UI_SET_KEYBIT, BTN_MOUSE) < 0)
    //    die("error: ioctl o2");
    if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
        die("error: ioctl o3");
    if(ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT) < 0)
        die("error: ioctl o4");

    if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
        die("error: ioctl o5");
    if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
        die("error: ioctl o6");
    if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
        die("error: ioctl o7");

    if(ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0)
        die("error: ioctl o8");
    if(ioctl(fd, UI_SET_ABSBIT,ABS_X) < 0)
        die("error: ioctl");
    if(ioctl(fd, UI_SET_ABSBIT, ABS_Y) < 0)
        die("error: ioctl o9");

    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    uidev.absmin[ABS_X]=0;
    uidev.absmax[ABS_X]=1023;
    uidev.absfuzz[ABS_X]=0;
    uidev.absflat[ABS_X ]=0;
    uidev.absmin[ABS_Y]=0;
    uidev.absmax[ABS_Y]=767;
    uidev.absfuzz[ABS_Y]=0;
    uidev.absflat[ABS_Y ]=0;

    if(write(fd, &uidev, sizeof(uidev)) < 0)
        die("error: write0");

    if(ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: ioctl o10");
    return true;
#endif // end of first try

    struct uinput_setup usetup;
    //int i = 50;

    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

    /* enable mouse button left and relative events */
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);

    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);

    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; /* sample vendor */
    usetup.id.product = 0x5678; /* sample product */
    strcpy(usetup.name, "Example device");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    /*
     * On UI_DEV_CREATE the kernel will create the device node for this
     * device. We are inserting a pause here so that userspace has time
     * to detect, initialize the new device, and can start listening to
     * the event, otherwise it will not notice the event we are about
     * to send. This pause is only needed in our example code!
     */

    return true;
}

bool mouseclose(void)
{
    qDebug() << "Mouse Close fd="<<fd;
    if(ioctl(fd, UI_DEV_DESTROY) < 0)
       die("error: cannot destroy uinput device\n");
    close(fd);
   qDebug() << "Mouse Close Worked";
    return true;
}

#undef die
// try to report the error via Qt console
// don't stop this time
#define die(str) {qDebug()<<str; qDebug()<<errno;}

bool mouseclick(void)
{

    struct uinput_user_dev uidev;
    struct input_event     ev;
    int                    x = 500;
    int                    y = 500;
    //int                    i;

    qDebug() << "Mouse click fd="<<fd;
    if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
        die("error: ioctl c1");
   // if(ioctl(fd, UI_SET_KEYBIT, BTN_MOUSE) < 0)
    //    die("error: ioctl c2");
    if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)
        die("error: ioctl c3");
    if(ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT) < 0)
        die("error: ioctl c4");

    if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)
        die("error: ioctl c5");
    if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)
        die("error: ioctl c6");
    if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)
        die("error: ioctl c7");

    if(ioctl(fd, UI_SET_EVBIT, EV_ABS) < 0)
        die("error: ioctl c8");
    if(ioctl(fd, UI_SET_ABSBIT,ABS_X) < 0)
        die("error: ioctl c9");
    if(ioctl(fd, UI_SET_ABSBIT, ABS_Y) < 0)
        die("error: ioctl c10");

    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    uidev.absmin[ABS_X]=0;
    uidev.absmax[ABS_X]=1920;
    uidev.absfuzz[ABS_X]=0;
    uidev.absflat[ABS_X ]=0;
    uidev.absmin[ABS_Y]=0;
    uidev.absmax[ABS_Y]=1080;
    uidev.absfuzz[ABS_Y]=0;
    uidev.absflat[ABS_Y ]=0;

    if(write(fd, &uidev, sizeof(uidev)) < 0)
        die("error: write0");

    if(ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: ioctl c11");

    memset(&ev, 0, sizeof(struct input_event));
    gettimeofday(&ev.time,NULL);
    ev.type = EV_ABS;
    ev.code = ABS_X;
    ev.value = x;
    if(write(fd, &ev, sizeof(struct input_event)) < 0)
        die("error: write1");
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    if(write(fd, &ev, sizeof(struct input_event)) < 0)
        die("error: write4");

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_ABS;
    ev.code = ABS_Y;
    ev.value = y;
    if(write(fd, &ev, sizeof(struct input_event)) < 0)
        die("error: write2");
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    if(write(fd, &ev, sizeof(struct input_event)) < 0)
        die("error: write3");

    return true;
}
#endif // not using kernel API on linux
