#include <QMainWindow>
#include <QDebug>
#include <QString>

// stub this out to leave for documentation
// This worked, but never helped us solve the failure of the X11 solution
// for Q_OS_LINUX
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>


// ERROR HANDLER, GENERIC
static int ErrorHandler (Display *display, XErrorEvent *error)
{
   (void)error;
   //printf ("\r\n error! \r\n");
   qDebug()<<"Error Handler: display "<<display;
   return 0;
}
// END ERROR HANDLER

// GET THE ACTUAL TEXT!!! - This is where it all comes unstuck
void GetWindowProperties (Display *display, Window window)
{
   Atom *atoms;
   int i, j;
   //Atom type;
   //int format, result
   int status;
   //unsigned long len, bytesLeft;
   //unsigned char *data;
   char *atomName;
   XTextProperty textData;
   qDebug()<<"GetWinProps Display:"<<display<<" Window:"<<window;
   atoms = XListProperties (display, window, &i);
   if (i)
   {
      for (j=0; j < i; j++)
      {
         atomName = XGetAtomName(display, atoms[i]);
       if (atomName)
       {
            //printf ("Atom name: %s\r\n", atomName);
           qDebug()<<"Atom Name "<<atomName;
          }
       status = XGetTextProperty (display, window, &textData, atoms[i]);
       if (status)
       {
        printf ("Atom text: %s\r\n", textData.value);
        qDebug()<<"Atom "<<atomName<<":"<<textData.value;
      }
      }
   }
}
// END GET THE ACTUAL TEXT!!! - This is where it all comes unstuck

// ENUMARATE THROUGH WINDOWS AND DISPLAY THEIR TITLES
void EnumerateWindows (Display *display, Window rootWindow, int showErrors, int showStatus)
{
   static int level = 0;
   Window parent;
   Window *children;
   //Window *child;
   unsigned int noOfChildren;
   int status;
   unsigned int i;

   XTextProperty wmName;
   //char **list;

   qDebug()<<"EnumWindows of root "<<rootWindow;
   status = XGetWMName (display, rootWindow, &wmName);
   if ((status) && (wmName.value) && (wmName.nitems))
   {
       qDebug()<<"wmName "<<wmName.value;
#if 0 // just dummy out for now
      //status = XmbTextPropertyToTextList (display, &wmName, &list, &i);
      if ((status >= Success) && (i) && (*list))
      {
         //printf ("INFO - Found window with name '%s' \r\n", (char*) strdup (*list));
          qDebug()<<"EnumWin "<<(char*) strdup (*list);
      }
#endif // dummy
   }

   //GetWindowProperties (display, rootWindow);

   level++;

   status = XQueryTree (display, rootWindow, &rootWindow, &parent, &children, &noOfChildren);

   if (status == 0)
   {
      if (showErrors)
      {
         //printf ("ERROR - Could not query the window tree. Aborting.\r\n");
         qDebug()<<"xQueryTree Error display "<<rootWindow;
      }
      return;
   }

   if (noOfChildren == 0)
   {
      if (showErrors)
      {
         //printf ("ERROR - No children found. Aborting.\r\n");
          qDebug()<<"xQueryTree no children "<<rootWindow;
      }
      return;
   }
   else
   {
      if (showStatus)
      {
         //printf ("STATUS - %i number of child windows found.\r\n", noOfChildren);
          qDebug()<<"  Child Windows "<<noOfChildren;

      }
   }

   for (i=0; i < noOfChildren; i++)
   {
      EnumerateWindows (display, children[i], showErrors, showStatus);
   }

   XFree ((char*) children);
}
// END ENUMERATE WINDOWS

void xLookAtWindows()
{
   // CONNECT TO THE XSERVER
   Display *display;
   int depth;
   int screen;
   int connection;

   display = XOpenDisplay (NULL);
   screen = DefaultScreen (display);
   depth = DefaultDepth (display, screen);
   connection = ConnectionNumber (display);
   XSetErrorHandler (ErrorHandler);
#if 0
   printf ("XWinSpy - by van\r\n");
   printf ("----------------\r\n");
   printf ("Display: %s\r\n", XDisplayName((char*)display));
   printf ("Width: %d\r\n", DisplayWidth(display, screen));
   printf ("Height: %d\r\n", DisplayHeight(display, screen));
   printf ("Connection: %d\r\n", connection);
   printf ("Color Depth: %d\r\n", depth);
#endif

   qDebug()<<"Display:"<<XDisplayName((char*)display);
   qDebug()<<"Screen:  wXh:"<<DisplayWidth(display, screen)<<"X"<<DisplayHeight(display, screen);
   qDebug()<<"Connection:"<<connection<<" Depth "<<depth;
   // END CONNECT TO THE XSERVER

   // RETRIEVE ROOT WINDOW
   Window rootWindow;

   rootWindow = RootWindow (display, screen);
   qDebug()<<"RootWindow: "<<rootWindow;
   // END RETRIEVE ROOT WINDOW

   // LOOP THROUGH ALL WINDOWS
   EnumerateWindows (display, rootWindow, 0, 0);
   // END LOOP THROUGH ALL WINDOWS

   // DISCONNECT FROM THE XSERVER
   XCloseDisplay (display);
   // END DISCONNECT FROM THE XSERVER

  //return EXIT_SUCCESS;
}

#endif
// stubbed out diagnostic code for Q_OS_LINUX
