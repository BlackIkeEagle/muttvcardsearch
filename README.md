muttvcardsearch
============

A small mutt carddav search utility for your Owncloud or SOGo server using QT and libvard

INSTALL
------------
first, install
* libcurl (installed by default on many linux and/or BSD distributions)
* libvcard
* QT (requiered by libvcard)

then

1. clone the repository
2. cd into muttvcardsearch
3. execute qmake; make; sudo make install

Call muttvcardsearch without arguments so see how to configure it.
After that set your query command in your .muttrc
* `set query_command = "muttvcardsearch '%s'"`

If you like to have your data cached locally use *--create-local-cache* as the first and only option.
This will download all your contacts into ~/.config/muttvcardsearch/cache.dat which is
a simple textfile containing your vcards. A new search should then search the local cache first
and if your query does not return any data it will search the server.

PS:
------------
should compile and work on allmost all *NIX versions/distributions where QT is supported and maybe Win32 too,
because there is QT natively available as well as libcurl.
