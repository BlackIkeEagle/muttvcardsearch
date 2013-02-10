#include "cache.h"

Cache::Cache()
{
    cache_file = cfg.getConfigDir();
    cache_file.append("/cache.sqlite3");
    db = NULL;
}

Cache::~Cache() {
    if (db) {
        int retVal = sqlite3_close_v2(db);
        if( SQLITE_OK != retVal ) {
            std::cerr << "Oops: failed to properly close database: " << sqlite3_errstr(retVal) << std::endl;
        }
    }
}

bool Cache::finalizeSqlite() {
    int retVal = sqlite3_finalize(stmt);
    if(SQLITE_OK != retVal) {
        std::cerr << "Unable to finalize: " << sqlite3_errstr(retVal) << std::endl;
        return false;
    }

    return true;
}

bool Cache::stepSqlite(const std::string &errMsg) {
    int retVal = sqlite3_step(stmt);
    if(SQLITE_DONE != retVal) {
        std::cerr << errMsg << ": " << sqlite3_errstr(retVal) << std::endl;
        return false;
    }

    return true;
}

bool Cache::prepSqlite(const std::string &query) {
    int retVal = sqlite3_prepare_v2(db, query.data(), -1, &stmt, NULL);
    if(SQLITE_OK != retVal) {
        std::cerr << "Failed to prepare statement: " << query << " - Message:" << sqlite3_errstr(retVal) << std::endl;
        return false;
    }

    return true;
}

std::wstring Cache::buildDateTimeString(const std::wstring &dtString) {
    std::wstring result = dtString;
    int pos = result.find('+');

    if(pos > 0) {
        result = result.substr(0, pos);
    }

    pos = result.find('T');
    std::wstring date = result.substr(0, pos);
    std::wstring time = result.substr(pos+1);

    if(date.length() != 10) {
        // hmm, invalid! what to to?
        date = L"1971-01-01";
    }

    if(time.length() != 8) {
        time = L"00:00:00";
    }

    result = date + L"T" + time;
    return result;
}

bool Cache::initSqlite() {
    int retVal = sqlite3_initialize();
    if(SQLITE_OK != retVal) {
        std::cerr << "Can't initialize sqlite3: " << sqlite3_errstr(retVal) << std::endl;
        return false;
    }

    return true;
}

// explicit open - necessary on update or select (search)
bool Cache::openDatabase() {
    // allready open?
    if(db) return true;

    if(false == initSqlite())
        return false;

    int retVal = sqlite3_open_v2(cache_file.c_str(), &db, SQLITE_OPEN_READWRITE, NULL);
    if(SQLITE_OK != retVal) {
        std::cerr << "Can't open/create database (RW) in " << cache_file << ": " << sqlite3_errstr(retVal) << std::endl;
        return false;
    }

    return true;
}

// see http://stackoverflow.com/questions/4804298/c-how-to-convert-wstring-into-string
std::string Cache::toNarrow(const std::wstring& text)
{
    std::setlocale(LC_ALL, "");
    const std::locale locale("");
    typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
    const converter_type& converter = std::use_facet<converter_type>(locale);

    std::vector<char> to(text.length() * converter.max_length());

    std::mbstate_t state;
    const wchar_t* from_next;
    char* to_next;
    const converter_type::result result = converter.out(state, text.data(), text.data() + text.length(), from_next, &to[0], &to[0] + to.size(), to_next);
    if (result == converter_type::ok or result == converter_type::noconv) {
        const std::string s(&to[0], to_next);
        return s;
    }

    return "";
}

void Cache::addEmails(const std::vector<std::string> &emails, int rowID) {
    for(unsigned int i=0; i<emails.size(); i++) {
        std::string email = emails.at(i);

        std::stringstream ss;
        ss << "insert into emails (vcardid, mail) values(";
        ss << rowID;
        ss << ", '" << email << "')";

        prepSqlite(ss.str());
        stepSqlite("Failed to add email to database");
        finalizeSqlite();
    }
}

void Cache::addVCard(const std::wstring &fn, const std::wstring &ln, const std::vector< std::string > &emails, const std::wstring &data, const std::wstring &updatedAt) {
    if(fn.length() == 0) {
        std::cerr << "Firstname is empty!" << std::endl;
        return;
    }

    if(ln.length() == 0) {
        std::cerr << "Lastname is empty!" << std::endl;
        return;
    }

    if(emails.size() == 0) {
        std::cerr << "Email is empty!" << std::endl;
        return;
    }

    if(data.length() == 0) {
        std::cerr << "Data is empty!" << std::endl;
        return;
    }

    if(db == NULL) {
        std::cerr << "Database not open!" << std::endl;
        return;
    }

    std::wstring dt = buildDateTimeString(updatedAt);
    std::wstringstream ss;

    // happy sql injecting ;)
    ss << "insert into vcards (FirstName, LastName, VCard, UpdatedAt) values (";
    ss << "'" << fn << "','" << ln << "','" << data << "','" << dt << "')";
    std::string query = toNarrow(ss.str());

    bool b = prepSqlite(query);
    if(b) {
        b = stepSqlite("Failed to add new record to cache database");
        if(b) {
            sqlite3_int64 rowid = sqlite3_last_insert_rowid(db);
            addEmails(emails, rowid);
        } else {
            finalizeSqlite(); // finalize on error only
        }
    }
}

bool Cache::createDatabase() {

    // stdio standard function
    if( remove (cache_file.c_str()) == 0 ) {
        std::cout << "Old cache database removed" << std::endl;
    } else {
        std::cerr << "Failed to remove old cache database" << std::endl;
        return false;
    }

    if(false == initSqlite())
        return false;

    int retVal = sqlite3_open_v2(cache_file.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if(SQLITE_OK != retVal) {
        std::cerr << "Can't open/create database in " << cache_file << ": " << sqlite3_errstr(retVal) << std::endl;
        return false;
    }

    // create the main table
    bool b = prepSqlite("CREATE TABLE vcards(VCardID INTEGER PRIMARY KEY, FirstName STRING, LastName STRING, VCard TEXT, UpdatedAt STRING)");
    if(false == b) return b;
    b = stepSqlite("Can't step to create table 'vcards' in cache database");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // create the mail table
    b = prepSqlite("CREATE TABLE emails(ID INTEGER PRIMARY KEY, VCardID INTEGER, Mail STRING)");
    if(false == b) return b;
    b = stepSqlite("Can't step to create table 'emails' in cache database");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // index on raw vcard data
    b = prepSqlite("CREATE INDEX vcard_idx ON vcards (vcard)");
    if(false == b) return b;
    b = stepSqlite("Can't step on index for table 'vcards', column 'vcard'");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // index on email data
    b = prepSqlite("CREATE INDEX email_idx ON emails (mail)");
    if(false == b) return b;
    b = stepSqlite("Can't step on index for table 'emails', column 'mail'");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // index on first name
    b = prepSqlite("CREATE INDEX firstname_idx ON vcards (firstname)");
    if(false == b) return b;
    b = stepSqlite("Can't step on index for table 'vcards', column 'firstname'");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // index on last name
    b = prepSqlite("CREATE INDEX lastname_idx ON vcards (lastname)");
    if(false == b) return b;
    b = stepSqlite("Can't step on index for table 'vcards', column 'lastname'");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    // index on updatedat
    b = prepSqlite("CREATE INDEX updatedat_idx ON vcards (updatedat)");
    if(false == b) return b;
    b = stepSqlite("Can't step on index for table 'vcards', column 'updatedat'");
    if(false == b) return b;
    b = finalizeSqlite();
    if(false == b) return b;

    std::cout << "New cache database created in '" << cache_file << "'" << std::endl;
    return b;
}
