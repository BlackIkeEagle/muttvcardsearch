qvcardsearch
============

A small mutt carddav search utility for an Owncloud server using QT and libvard

INSTALL
------------
first, install
* libcurl (installed by default on many linux and/or BSD distributions)
* libvcard
* QT (requiered by libvcard)

then

1. clone the repository
2. cd into qvcardsearch
3. execute qmake; make; sudo make install

Call qvcardsearch without arguments so see how to configure it.
After that set your query command in your .muttrc
* `set query_command = "qvcardsearch '%s'"`

If you like to have your data cached locally use *--create-local-cache* as the first and only option.
This will download all your contacts into ~/.config/qvcardsearch/cache.dat which is
a simple textfile containing your vcards. A new search should then search the local cache first
and if your query does not return any data it will search the server.

NOTICE
------------

this is a **very basic and ugly try** to show how to search a carddav server using C++ and
put the results back tu Mutt. But it works for me.

PS:
------------
should compile and work on allmost all *NIX versions/distributions where QT is supported and maybe Win32 too!?
