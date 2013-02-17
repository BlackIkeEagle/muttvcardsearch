#include "searchtemplates.h"

SearchTemplates::SearchTemplates()
{
    exportTemplate = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
    exportTemplate += "<propfind xmlns=\"DAV:\" xmlns:CS=\"http://calendarserver.org/ns/\">";
    exportTemplate += "<prop>";
    exportTemplate += "<CS:getctag/>";
    exportTemplate += "</prop>";
    exportTemplate += "</propfind>";

    searchTemplate = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
    searchTemplate += "<C:addressbook-query xmlns:D=\"DAV:\" xmlns:C=\"urn:ietf:params:xml:ns:carddav\">";
    searchTemplate += "<D:prop>";
    searchTemplate += "<D:getetag/>";
    searchTemplate += "<C:address-data>";
    searchTemplate += "<C:prop name=\"EMAIL\"/>";
    searchTemplate += "<C:prop name=\"FN\"/>";
    searchTemplate += "</C:address-data>";
    searchTemplate += "</D:prop>";
    searchTemplate += "<C:filter test=\"anyof\">";
    searchTemplate += "<C:prop-filter name=\"FN\">";
    searchTemplate += "<C:text-match collation=\"i;unicode-casemap\" match-type=\"contains\">%1</C:text-match>";
    searchTemplate += "</C:prop-filter>";
    searchTemplate += "<C:prop-filter name=\"EMAIL\">";
    searchTemplate += "<C:text-match collation=\"i;unicode-casemap\" match-type=\"contains\">%2</C:text-match>";
    searchTemplate += "</C:prop-filter>";
    searchTemplate += "</C:filter>";
    searchTemplate += "</C:addressbook-query>";
}

std::string SearchTemplates::getExportTemplate() const {
    return exportTemplate;
}

std::string SearchTemplates::getSearchTemplate() const {
    return searchTemplate;
}
