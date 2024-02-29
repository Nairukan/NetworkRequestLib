#ifndef REQUEST_H
#define REQUEST_H

#include <curl/curl.h>
#include <map>
#include <string>
#include <format>
#include <bits/stdc++.h>
#include <atomic>

using std::map;
using std::string;
using std::stringstream;
using std::pair;

namespace request{

    bool TimeLimited=true;
    bool RepitRequestInBad=true;

    static uint Request_count_max_ms=20000;

    class Request
    {
    public:
        string URL;
        Request(CURL* , string ,
                map<string, string>& ,
                map<string, string>& ,
                map<string, string>& , stringstream* =nullptr, string = "GET");

        [[deprecated("USE METHOD NAME LIKE AS GET/POST/DELETE, covered string(<MetodName>)")]] Request(CURL* , string ,
                map<string, string>& ,
                map<string, string>& ,
                map<string, string>& , stringstream* =nullptr, bool = false);


        [[deprecated("use map of params for request")]] Request(CURL* , string ,
                map<string, string>& ,
                map<string, string>& , stringstream* =nullptr, bool = false);

        Request& exec(map<string, string>&, map<string, string>& );
        Request& exec();

        const stringstream* result();

        static size_t header_write(char *ptr, size_t size, size_t nmemb,
                        pair<map<string, string>&, map<string, string>& > &metadata);


        static size_t string_write(char *ptr, size_t size, size_t nmemb, string &str);
    private:
        void _init();

        CURL* handle;
        stringstream* responce = nullptr;
        string str;
        curl_slist* headers = NULL;
        std::pair<map<string, string>&, map<string, string>&> metadata;
        string Metod;
        const char* data;
        string some;
    };

}

#endif // REQUEST_H
