# SpeedBlocks
This game tries to be the unofficial successor of Cultris II since the development of this awesome game seems to be dead for many years. If you want to help us, please read the "How to contribute" section below.

# Requirements

- [SFML](https://www.sfml-dev.org/index.php)
Version 2.4.2 is used by this project and it's highly recommended not to use an older version.

- [TGUI](https://tgui.eu/)
Version 0.7.4 or a higher 0.7.x version is required.

- [curl](https://curl.haxx.se/libcurl/)

- [cmake](https://cmake.org/download/) This project is using cmake as a build system.

### Remarks

The ResourcePath.mm/hpp is only for Mac builds to be able to access the game resources.
Window/Linux uses EmptyResourcePath.h/cpp to replace it with a NULL function to make
source able to compile as-is on any OS.

MingwConverter.h is to address a bug in MingW32, witch doesn't give it access to
standard functions to_string() and stoi(). It's replaced with boost/lexical_cast so
if you want to compile with MingW32 you need that too.
This is again included in the source to make it compile as-is.

# Building

Please see the Wiki for additional instructions.

# How to contribute

Please read the following before you start working on something.

- If you are willing to contribute to this project but you don't have an idea where to start, have a look at the issue tracker.

- An issue which is labeled with "help wanted" either needs your opinion or an implementation/fix.

- Our line endings are linux style. Please configure your editor or git to respect these. We won't merge pull requests with windows style line breaks.

- If you start writing code for a specific issue it is a good idea to mention this in the issue so that other contributors can see that you are working on this.

- If you are implementing a new feature, please create a new branch for this. If you don't do this, it is possible that we won't merge this.

- All ongoing development is done in the `develop` branch and will be merged into master when it's stable.

- If you need help with some implementation details, feel free to ask by opening a new issue. But please note that we can not give support for vendor specific questions like "how to use library X" or basic questions about c++.

- It is also a moving target to increase the code quality of this project so any help in refactoring code to make it more readable and maintainable is very appreciated.



# Server
You can find the server [here](https://github.com/kroyee/SpeedBlocks-server)

The server is running on a VPS in Stockholm. To play with an account sign up on the [forum](https://speedblocks.se/forum) and use the username and password in the game client.
	
# Donation
We are currently collecting money for a VPS server solution. This costs around €50-100 per year.

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=YCZPUC9H3ZRXS)
