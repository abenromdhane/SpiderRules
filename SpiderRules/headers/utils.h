/*
The MIT License (MIT)

Copyright (c) 2014 Ahmed Ben Romdhane

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <iostream>
#include <fstream>      // fstream
#include <vector>
#include <string>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <cerrno>
#include <boost/tokenizer.hpp>


std::wstring getInputJSONFile(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    return(std::wstring((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
  }
  throw(errno);
}

std::vector<std::vector<std::string>> parseRulesFromCSV(std::string path)
{
    using namespace std;
    using namespace boost;

    ifstream in(path.c_str());
    
    typedef tokenizer< escaped_list_separator<char> > Tokenizer;

    escaped_list_separator<char> sep('\\', ';', '\"');

    vector< string > vec;
    string line;
    string buffer;
	vector< vector< string > > returnVector;

    bool inside_quotes(false);
    size_t last_quote(0);

    while (getline(in,buffer))
    {
        // --- deal with line breaks in quoted strings

        last_quote = buffer.find_first_of('"');
        while (last_quote != string::npos)
        {
            inside_quotes = !inside_quotes;
            last_quote = buffer.find_first_of('"',last_quote+1);
        }

        line.append(buffer);

        if (inside_quotes)
        {
            line.append("\n");
            continue;
        }
        // ---

        Tokenizer tok(line, sep);
        vec.assign(tok.begin(),tok.end());

        line.clear(); // clear here, next check could fail

        // example checking
        // for correctly parsed 3 fields per record
        if (vec.size() < 3) continue;

		returnVector.push_back(vec);
    }

    in.close();

    return returnVector;
}