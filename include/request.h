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


    class Request
    {
    public:


        inline static bool TimeLimited=true;
        inline static bool RepitRequestInBad=true;

        inline static unsigned int Request_count_max_ms=20000;

        inline static std::vector<bool (*)(Request* )> auto_producers_all;

        unsigned int code=0;
        unsigned int secs=0;

        std::vector<bool (*)(Request* )> request_ans_produce;



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

        const string get_temp_responce();

        //copy metadata
        std::pair<map<string, string>&, map<string, string>&> get_metadata();

        CURL* get_curl_handle();

        static size_t header_write(char *ptr, size_t size, size_t nmemb,
                        pair<map<string, string>&, map<string, string>& > &metadata);


        static size_t string_write(char *ptr, size_t size, size_t nmemb, string &str);
    private:
        void _init();

        static bool time_limit(Request* rq){
            if (request::Request::TimeLimited && Request_count_max_ms-rq->secs*1000<Request_count_max_ms*0.01){
                std::cout << rq->code << " Request Time Limit, repit after 10s\n";

                std::this_thread::sleep_for(std::chrono::seconds(10));
                rq->exec();
                return true;
            }
            return false;
        }

        static bool good(Request* rq){
            if (rq->code==200){
#ifdef DEBUG_STATUS
                std::cout << "good produce\n";
#endif
                return true;
            }
            return false;
        }



        static bool redirect(Request* rq){
            if (rq->code==302){
                map<string,string> t;
                std::cout<<"Redirect to " << rq->metadata.second["Location"] << "\n";
                Request(rq->handle, rq->metadata.first["Location"], t, rq->metadata.first, rq->metadata.second, rq->responce, rq->Metod).exec();
                return true;
            }
            return false;
        }

        static bool just_repit_in_wrong(Request* rq){
            if (rq->code!=200 && request::Request::RepitRequestInBad){
                std::cout << rq->code << " Bad Ans Request, repit after 10s\n";
                std::ofstream ou("log.txt"); ou << rq->str; ou.close();
                std::this_thread::sleep_for(std::chrono::seconds(10));
                rq->exec();
                return true;
            }
            return false;
        }

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
