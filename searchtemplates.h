#ifndef SEARCHTEMPLATES_H
#define SEARCHTEMPLATES_H

#include <string>

class SearchTemplates
{
public:
    SearchTemplates();
    std::string getDefaultExportTemplate() const;
    std::string getDefaultSearchTemplate() const;

private:
    std::string exportTemplate;
    std::string searchTemplate;
};

#endif // SEARCHTEMPLATES_H
