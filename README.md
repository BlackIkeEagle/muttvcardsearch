qvcardsearch
============

A small mutt carddav search utility for an Owncloud server using QT, pugixml and libvard

INSTALL
------------
first, install
* libcurl (installed by default on many linux distributions)
* libvcard
* pugixml
* QT (requiered by libvcard)

then

1. clone the repository
2. cd into qvcardsearch
3. execute qmake; make; sudo make install

Call qvcardsearch without arguments so see how to configure it.
After that set your query command in your .muttrc
* `set query_command = "qvcardsearch '%s'"`

NOTICE
------------

this is a **very basic and ugly try** to show how to search a carddav server using C++ and
put the results back tu Mutt. But it works for me.
