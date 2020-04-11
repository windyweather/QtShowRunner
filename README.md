QtShowRunner



QtShowRunner is a program to chain together multiple LibreOffice impress slide shows.

Each show in the sequence is set under Slide Settings so that mouse clicks do not advancethe slides. This is important. QtShowRunner launches each show in the sequence and then clicks the mouse every few seconds and when the last black screen is shown, the mouse click terminates the show and Impress exits. QtShowRunner notices the process exit and then starts the next show. When the last show ends, QtShowRunner just starts the list again.

This complicated process gets around the fact that LibreOffice Impress apparently has no options or scripting features available to accomplish chaining and looping of multiple slide shows together.

The reason for wanting to chain multiple shows is that after 100 or so slides with images the save times and editing for shows becomes cumbersome. This is quite reasonable for at this point the show file size may become 100MB or more.
Features

    Launches Impress Slide Shows
    Plays a sequence of slide shows
    Loops the list
    Requires no interaction between shows

https://sourceforge.net/projects/qtshowrunner/
