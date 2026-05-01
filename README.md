# SpaceTimeHeart

## What is SpaceTimeHeart?

SpaceTimeHeart is a note taking application that is designed for creative writing where the user can interconnect various notes together. The application supports plain-text and markdown edditing, as well as special types of notes for custom calendar editing and image annotation. All notes support linking to other notes and there is a special mind-map view for you to see all note connections in a big network. The application aims to streamline creation of complex worlds where any text can be attached to a place or a specific date. Hope you enjoy using this application!

## Features:

* Plain-text editor - the user can write in a .txt file, with linking being supported both to web links and to other notes. Any .txt file can be placed in the working directory to be accesible to the application, when first encountered, a new file will be slightly modified to add specific application data to ensure note linking works correctly.
* Markdown editor - this editor is basically the same as .txt editor, but the text is shown as markdown with appropriate styling. To edit the file, the user can open the text editor to the right and use it along with buttons to ease markdown writing. 
* Calendar editor - this editor allows the user to create a custom calendar with any number of months, days, weeks or moons. Each day can have a note with some text and even a link to another note, so that the user can track events that happen throughout the days and connect the events to in depth descriptions in the plain-text editor or markdown editor. 
* Image-Annotation editor - this editor allows the user to upload any png, jpg, webp or svg image as a background image, then add markers or custom shapes on top of the background. The markers can link to other notes. This can be used to annotate a map with markers for key locations, country borders, or other information. 
* Mind-map view - when opened, the view will showcase all notes recognized by the application in a mind map, if a note has a link to another note it will have a connection to it. Clicking on nodes in the mind map will open the related note.

## Installation
Windows and Linux releases are available pre-deployed. Download the zip file and run the SpaceTimeHeart.exe file for windows, download and run the SpaceTimeHeart.Appimage for linux. You can also deploy the aplication yourself using the following guides:

### Windows
Before starting, ensure the following are installed and added to your System Path:
```
    Qt Framework: (e.g., Qt 6.x) installed via the Qt Online Installer.
```
```
    Compiler: MinGW (usually bundled with Qt) or MSVC.
```
```
    Build Tool: make (for MinGW) or nmake (for MSVC).
```
Open the QT Comand Prompt (Usually named Qt 6.x.x (MinGW 13.x.x 64-bit)) and open the directory where the source code is downloaded (cd C:/Directory/to/source/code). Ensure the path consists only of valid, ASCII-compatible characters (no cyrillic characters for example). Then one by one use the following commands:

```
    qmake SpaceTimeHeart.pro
```
```
    mingw32-make
```
```
    windeployqt ./bin/SpaceTimeHeart.exe
```

The ./bin directory will hold the resulting application in it's entirety.

## License
GNU General Public License v3.0. Developed using QT Creator.
