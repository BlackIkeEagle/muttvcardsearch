muttvcardsearch
============

ABOUT
------------
muttvcardsearch is a small mutt carddav search utility for your Owncloud or SOGo server.
It is written in C++ and depends *only* on [libcurl](http://curl.haxx.se/libcurl/)
and [sqlite3](http://www.sqlite.org/). It supports multiple servers, i.e. you can have as
many carddav resources (url's) as you like.

The vcard code is entirely based on [libvcard](http://code.google.com/p/libvcard), but does not
need QT and also fixes some major bugs I found in [libvcard](http://code.google.com/p/libvcard/)
(parsing vcards from file, parsing attribute values containing a colon...). It is not feature
complete, i.e. not all functions from *libvcard* are implemented at this time but it works quiet
well for me. I plan to implement all features from the original *libvcard*-library (currently 1.0.2).

BUILD
------------
You can use one of the following tools to build the project
* QtCreator
* cmake

INSTALL
------------
first, install
* libcurl / libcurl-dev
* sqlite3 / sqlite3-dev

then

1. clone the repository
2. cd into the new repository
3. compile
    * for QtCreator execute `qmake; make; sudo make install`
    * for cmake execute `mkdir build; cd build; cmake -DCMAKE_BUILD_TYPE=Release ..; make; sudo make install`

USAGE
------------
Set your query command in your .muttrc
* `set query_command = "muttvcardsearch '%s'"`

muttvcardsearch will automatically insert new records not found in the cache but found online.

CONFIGURE
------------
Call muttvcardsearch without arguments to see how to configure it.
There are 4 options to add/edit carddav url's

1. `--name=` - set a label under which this entry will be stored and identified for updates
2. `--server=` - provide the carddav url here
3. `--username=` - your username
4. `--password=` - your secret password

There is one option which can be used to create a local cache

1. `--create-local-cache` This will download all your contacts into ~/.config/muttvcardsearch/cache.sqlite3.
  A new search should then search the local cache first and if your query does not return any data it will search the server(s).
  The cache will combine all results found in all your servers / carddav resources

Note:

* Include the values in double quotes if you have whitespace in it.
* The options don't have to be in a particular order
* To change a setting just reconfigure and pass `--name=your-label` to identify the entry
  and then apply the changes you have. Always provide all settings - even if you only
  would like to change a single setting.
* To add an entry (i.e. a new vcard list) provide a new label, i.e. `--name=your-new-label`
* There is currently no option to show a single entry, show all entries or delete one (or all)
  This can all be done manually using your favorite editor or `less`... see TODO.

UPGRADE
------------
If you upgrade from version 1.4 or earlier, remove your config file first
and reconfigure the application.

TODO
------------
* add option *--update-local-cache* to refresh the cache instead to force you to re-create it every time you made a change on your server. This will only work after the migration away from libvcard (this requirement has been met with version 1.4)
* add option to save a contact into your vcard store (just like abook)
* add option to show a config entry (or all entries) and delete an entry
