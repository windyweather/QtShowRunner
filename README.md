QtShowRunner

QtShowRunner is a program to chain together multiple LibreOffice impress slide shows and loop them indefinitly.

Each show in the sequence is set under Slide Settings so that mouse clicks do not advance the slides. This is important. QtShowRunner launches each show in the sequence and then clicks the mouse every few seconds and when the last black screen is shown, the mouse click terminates the show and Impress exits. QtShowRunner notices the process exit and then starts the next show. When the last show ends, QtShowRunner just starts the list again.

This complicated process gets around the fact that LibreOffice Impress apparently has no options or scripting features available to accomplish chaining and looping of multiple slide shows together.

The reason for wanting to chain multiple shows is that when a slide show has 100 or more slides with images, the save times and editing for shows becomes cumbersome. This is quite reasonable for at this point the show file size may become 100MB or more.

QtShowRunner runs both on Windows 10 and on Linux [Linux Mint 19.3].

April 2020


Possible Enhancements:

Save / Restore show lists. Not OS independent format since paths are not OS independent.

Use X11 rather than xdotool command to click the mouse on LINUX. I could never get this to work, but xdotool works like a champ.



