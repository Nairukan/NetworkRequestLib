#ifndef REQUEST_H
#define REQUEST_H

#include <curl/curl.h>
#include <map>
#include <string>
#include <format>
#include <bits/stdc++.h>

using std::map;
using std::string;
using std::stringstream;
using std::pair;

namespace request{

    static uint Request_count_max_ms=8000;

    class Request
    {
    public:
        string URL;
        Request(CURL* , string ,
                map<string, string>& ,
                map<string, string>& , stringstream* =nullptr, bool =false); //Curl Handle, Url, headers map, cookies map, body(optional), isPost(optional)

        Request& exec(map<string, string>&, map<string, string>& );
        Request& exec();

        const stringstream* result();

        static size_t header_write(char *ptr, size_t size, size_t nmemb,
                        pair<map<string, string>&, map<string, string>& > &metadata);


        static size_t string_write(char *ptr, size_t size, size_t nmemb, string &str);
    private:
        CURL* handle;
        stringstream* responce = nullptr;
        string str;
        curl_slist* headers = NULL;
        std::pair<map<string, string>&, map<string, string>&> metadata;
        bool isPost;
        const char* data;
        string some;
    };

}

#endif // REQUEST_H
