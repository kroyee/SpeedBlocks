# SpeedBlocks

You need SFML, TGUI and boost to run this, libraries can be found here:

# [SFML](https://www.sfml-dev.org/index.php)

I am using version 2.4.2

# [TGUI](https://tgui.eu/)

The game runs fine with the stable 0.7.3 except for a minor bug with when window
is rezised to a none-standard ratio. If you want it to remove this bug you need the
latest master from TGUIs github repo

# [boost](http://www.boost.org/)

You only need boost/random from the boost libs. (Except MingW users, see below)

The ResourcePath.mm/hpp is only for Mac builds to be able to access the game resources.
Window/Linux uses EmptyResourcePath.h/cpp to replace it with a NULL fuction to make
source able to compile as-is on any OS.

MingwConverter.h is to address a bug in MingW32, witch doesn't give it access to
standard functions to_string() and stoi(). It's replaced with boost/lexical_cast so
if you want to compile with MingW32 you need that too.
This is again included in the source to make it compile as-is.

Please see the Wiki for additional instructions.

If you want to contribute, please make a fork where you can push until your code
is stable. I will integrate from there.

# Server
You can find the server [here](https://github.com/kroyee/SpeedBlocks-server)

We currently have a VPS running a test-server in Oslo. Connect to it with hostname: darn.site
You can only connect with guest account at the moment.
	
# Donation
We are currently collecting money for a VPS server solution. This costs around €50-100 per year.

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=YCZPUC9H3ZRXS)
