#include <map>
#include <string>

#include "object.hpp"

using namespace std;

typedef tuple<Environment*, vector<string>, Error*> incl_result;

class Reader{
    public:
        virtual string read(string) = 0;
        virtual bool file_exists(string) = 0;
};

class FileReader: public Reader{
    public:
        string read(string);
        bool file_exists(string);
};

class MockReader: public Reader{
    private:
        string mock_file;
    public:
        string read(string);
        bool file_exists(string){return true;};
        MockReader(string txt){mock_file = txt;};
};

class Includer{
    private:
        Reader* reader;
        map<string, Environment*> included;
    public:
        Includer(Reader*);
        incl_result include(string);
};