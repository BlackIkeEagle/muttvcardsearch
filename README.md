muttvcardsearch
============

A small mutt carddav search utility for your Owncloud or SOGo server

INSTALL
------------
first, install
* libcurl / libcurl-dev
* sqlite3 / sqlite3-dev

then

1. clone the repository
2. cd into the new repository
3. execute `qmake; make; sudo make install`

Call muttvcardsearch without arguments so see how to configure it.
After that set your query command in your .muttrc
* `set query_command = "muttvcardsearch '%s'"`

If you like to have your data cached locally use *--create-local-cache* as the first and only option.
This will download all your contacts into ~/.config/muttvcardsearch/cache.sqlite3. A new search should
then search the local cache first and if your query does not return any data it will search the server.

The utility will automatically insert new records not found in the cache but found online.

TODO
------------
* add option to refresh the cache instead of redownload it every time you made a change on your server
* add option to save a contact into your vcard store (just like abook)

PS:
------------
should compile and work on allmost all *NIX versions/distributions where QT is supported and maybe Win32 too,
because there is QT natively available as well as libcurl.
