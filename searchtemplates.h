#ifndef SEARCHTEMPLATES_H
#define SEARCHTEMPLATES_H

#include <string>

class SearchTemplates
{
public:
    SearchTemplates();
    std::string getExportTemplate() const;
    std::string getSearchTemplate() const;

private:
    std::string exportTemplate;
    std::string searchTemplate;
};

#endif // SEARCHTEMPLATES_H
