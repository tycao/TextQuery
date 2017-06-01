#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
using namespace std;

using line_no = std::vector<std::string>::size_type;

class QueryResult;
class TextQuery
{
public:
    TextQuery(std::ifstream&);
    QueryResult query(const std::string&) const;
private:
    std::shared_ptr<std::vector<std::string>> file;
    std::map<std::string, std::shared_ptr<std::set<line_no>>> wm;
};
TextQuery::TextQuery(std::ifstream& in) : file(make_shared<std::vector<std::string>>())
{
    std::string text;
    while (getline(in, text))
    {
        file->push_back(text);
        std::vector<std::string>::size_type n = file->size() - 1;
        istringstream line(text);
        string word;
        while (line >> word)
        {
            std::shared_ptr<std::set<line_no>> &lines = wm[word];
            if (!lines)
                lines.reset(new std::set<line_no>);
            lines->insert(n);
        }
    }
}

class QueryResult
{
    friend std::ostream& print(std::ostream&, const QueryResult&);
public:
    QueryResult(std::string s, std::shared_ptr<std::set<line_no>> p, std::shared_ptr<std::vector<std::string>> f):
        sought(s), lines(p), file(f) { }

private:
    std::string sought;
    std::shared_ptr<std::set<line_no>> lines;
    std::shared_ptr<std::vector<std::string>> file;
};

QueryResult TextQuery::query(const string& s) const
{
    static shared_ptr<std::set<line_no>> nodata(new std::set<line_no>);
    auto loc = wm.find(s);
    if (loc == wm.end())
        return QueryResult(s, nodata, file);
    else
        return QueryResult(s, loc->second, file);
}

std::ostream& print(std::ostream& os, const QueryResult& qr)
{
    os << qr.sought << " occurs " << qr.lines->size() << ((qr.lines->size() > 1)? " times." : " time.")  << endl;
    for (auto num : *qr.lines)
        os << "\t(line " << num + 1 << ") " << *(qr.file->begin() + num) << endl;
}



int main()
{
    ifstream fd("D:\\aa.txt");
    TextQuery q(fd);
    QueryResult qr = q.query("computer");
    print(cout, qr);
    return 0;
}
