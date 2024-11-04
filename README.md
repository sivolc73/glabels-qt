## A note from this branch's maintainer

I am currently trying to revive this amazing software that has not received updates in over 4 years. 

Being build with QT5 this app is slowly getting less compatible and present some challenge for Ubuntu users using Wayland.
The current goal is to port it to QT6 and make sure it is stable on as many plasftorm and distro as possible.
Once ported I'll try to agregate the various fixes and translations that other branches have brought over time, a lot of 
deprecated calls are still present in the code and fixing all this will be a good first step

When all of this is done and if possible it would be amazing to update the original glabel in the ubuntu repository, as amazing as 
this software is it will never get used widely if you need to compile it or install manually dependencies in order to run it.

As a long term goal it would be interesting to see if support for Brother P-touch printers could be added, there is currently no
alternative running in Linux to the Windows/Mac software from Brother.

*******************************************************************************

## What is gLabels-qt?

gLabels-qt is the development version of the next major version of gLabels (a.k.a. glabels-4).

*******************************************************************************

![gLabels Label Designer](glabels/images/glabels-label-designer.png)

![Cover Image](docs/images/cover-image.png)

*******************************************************************************


## What's new in gLabels 4?

- A complete rewrite, based on the Qt5 framework.
- A new UI layout based on common activities.
- Cross-platform support
- User-defined variables
- Support for continuous-roll labels
- Many new product templates


## Download

### Latest Release

There are currently no official releases of gLabels 4.

## Build Instructions

- [Linux Build Instructions](docs/BUILD-INSTRUCTIONS-LINUX.md)
- [Windows Build Instructions](docs/BUILD-INSTRUCTIONS-WINDOWS.md)
- [Mac Build Instructions](docs/BUILD-INSTRUCTIONS-MACOS.md)


## Help Needed

Please see [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md).


## License

gLabels-qt is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

gLabels-qt is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

See [LICENSE](LICENSE) in this directory.

The following sub-components are also made available under less
restrictive licensing:

### Glbarcode

   gLabels-qt currently includes a version of the glbarcode++ library, located in
   the "glbarcode/" subdirectory.  It is licensed under the GNU LESSER GENERAL
   PUBLIC LICENSE (LGPL); either version 3 of the License, or (at your option)
   any later version.  See [glbarcode/LICENSE](glbarcode/LICENSE).

### Template Database

   The XML files in the "templates/" subdirectory constitute the glabels
   label database.  No copyright is claimed on the facts contained within
   the database and can be used for any purpose.  The files themselves are
   licensed using the MIT/X license.  See [templates/LICENSE](templates/LICENSE).
