#include "request.h"

namespace request{

    string strip(string text){
        text.resize(text.find_last_not_of(" ")+1);
        int p=text.find_first_not_of(" ");
        return text.substr(p, text.length()-p);
    }

    size_t Request::string_write(char *ptr, size_t size, size_t nmemb, string &str)
    {
        size_t total = size * nmemb;
        if (total)
            str.append(ptr, total);

        return total;
    }

    size_t Request::header_write(char *ptr, size_t size, size_t nmemb,
                               pair<map<string, string>&, map<string, string>& > &metadata)
    {
        string str;

        size_t total = size * nmemb;
        if (total)
            str.append(ptr, total);

        if (str.back() == '\n')
            str.resize(str.size() - 1);
        if (str.back() == '\r')
            str.resize(str.size() - 1);

        int separator=str.find(":");
        if (separator>0){
            string key=str.substr(0, separator);
            string value=str.substr(separator+2, str.length()-separator-2);
            if (key=="Set-Cookie" || key=="set-cookie"){
                //std::cout << value << "\n";
                int p1=0, p2=value.find(";", p1);
                while (p2!=-1){
                    string OneCookieRAW=strip(value.substr(p1, p2-p1));
                    separator=OneCookieRAW.find("=");
                    if (separator!=-1){
                        key=OneCookieRAW.substr(0, separator);
                        string cookievalue=OneCookieRAW.substr(separator+1, OneCookieRAW.length()-separator-1);
                        if (metadata.second.find(key)!=metadata.second.end()){
                            if (metadata.second[key]!=cookievalue){
                                //std::cout << "Cookie changed -- " << key << "=" << metadata.second[key] << " -> " << key << "=" << cookievalue << "\n";
                            }
                        }else{
                            //std::cout << "Cookie add -- " << key << "=" << cookievalue << "\n";
                        }
                        if (cookievalue!="" && cookievalue!=" ")
                            metadata.second[key]=cookievalue;
                    }
                    p1=p2+1;
                    p2=value.find(";", p1);
                }
                //MoodleSession=08n8jt76qmr5833fn9uf2tba11; path=/; secure
            }else{
                if (metadata.first.find(key)!=metadata.first.end()){
                    if (metadata.first[key]!=value){
                        //std::cout << "  Header changed -- " << key << "=" << metadata.first[key] << " -> " << key << "=" << value << "\n";
                    }else{
                        //std::cout << "  Header -- " << key << "=" << metadata.first[key] << "\n";
                    }
                }else{
                    //std::cout << "  Header add -- " << key << "=" << value << "\n";
                }
                if (value!="" && value!=" ")
                    metadata.first[key]=value;
            }
        }
        return total;
    }

    Request::Request(CURL* handle, string URL,
                     map<string, string>& MapParams,
                     map<string, string>& MapHeaders,
                     map<string, string>& MapCookies, stringstream* responce, string Metod):
        handle(handle),
        responce(responce),
        metadata({std::ref(MapHeaders), std::ref(MapCookies)}),
        URL(URL),
        Metod(Metod)
    {
        if (MapParams.size())
        {
            stringstream adding;
            adding << "?" << MapParams.begin()->first << (MapParams.begin()->second != "" ? ("="+MapParams.begin()->second) : "");
            for (map<string,string>::iterator it=++MapParams.begin(); it!=MapParams.end(); ++it){
                adding << "&" << it->first << (it->second != "" ? ("="+it->second) : "");
            }
            this->URL.append(adding.str());
        }
        //std::cout << "URL: "<< URL << "\n";
        _init();
    }



    Request::Request (CURL* handle, string URL,
                     map<string, string>& MapParams,
                     map<string, string>& MapHeaders,
                     map<string, string>& MapCookies, stringstream* responce, bool isPost):
        handle(handle),
        responce(responce),
        metadata({std::ref(MapHeaders), std::ref(MapCookies)}),
        URL(URL),
        Metod(isPost ? "POST" : "GET")
    {
        if (MapParams.size())
        {
            stringstream adding;
            adding << "?" << MapParams.begin()->first << (MapParams.begin()->second != "" ? ("="+MapParams.begin()->second) : "");
            for (map<string,string>::iterator it=++MapParams.begin(); it!=MapParams.end(); ++it){
                adding << "&" << it->first << (it->second != "" ? ("="+it->second) : "");
            }
            this->URL.append(adding.str());
        }
        //std::cout << "URL: "<< URL << "\n";
        _init();
    }

    Request::Request(CURL* handle, string URL,
            map<string, string>& MapHeaders,
            map<string, string>& MapCookies, stringstream* responce, bool isPost):
        handle(handle),
        responce(responce),
        metadata({std::ref(MapHeaders), std::ref(MapCookies)}),
        URL(URL),
        Metod(isPost ? "POST" : "GET")
    {
        _init();
    }

    void Request::_init(){

        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, Metod.c_str());
        curl_easy_setopt(handle, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle, CURLOPT_DEFAULT_PROTOCOL, "https");

#ifdef DEBUG
        //curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
#endif
        char curlErrorBuffer[CURL_ERROR_SIZE];
        curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, curlErrorBuffer);

        if (headers!=NULL) curl_slist_free_all(headers);
        headers = NULL;
        for (const auto &now : metadata.first){
            headers = curl_slist_append(headers, (now.first+": "+now.second).c_str());
        }
        //if (responce!=nullptr) str=responce->str();
        stringstream cookies;
        for (const auto &now: metadata.second){
            cookies << now.first << "=" << now.second << "; ";
        }
        if (cookies.str().length())
        headers = curl_slist_append(headers, std::format("Cookie: {}", cookies.str().substr(0, cookies.str().length()-2)).c_str());

        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, string_write);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &str);
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, header_write);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &metadata);

        curl_easy_setopt(handle, CURLOPT_AUTOREFERER, 1L);
        request_ans_produce.push_back(redirect);

        if (metadata.first.find("User-Agent")!=metadata.first.end())
            curl_easy_setopt(handle, CURLOPT_USERAGENT, (metadata.first)["User-Agent"].c_str());

        //curl_easy_setopt(handle, CURLOPT_MAXREDIRS , 100L);
        //curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "")
        //curl_easy_setopt(handle, CURLOPT_READDATA, )
        //curl_easy_setopt(handle, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
        //curl_easy_setopt(handle, CURLOPT_FTP_SKIP_PASV_IP, 1L);
        curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, 1L);


    #ifdef DEBUG
        std::cout << format("URL: {}\nBODY: {}\n", URL, responce->str());
        std::cout << "Headers:\n";
        auto temp=headers;
        while (temp){
            std::cout << temp->data << "\n";
            temp=temp->next;
        }
        //std::cout << responce->str();
        std::cout << "End Headers\n\n";
    #endif
        if (responce != nullptr){
            some=responce->str();
            data=some.c_str();
        }

        if (!responce->str().empty() && Metod=="POST"){
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
        }
        //curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, Request_count_max_ms);
        if (TimeLimited){
            curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, Request_count_max_ms);
            request_ans_produce.push_back(time_limit);
        }else
            curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, -1);
        request_ans_produce.insert(request_ans_produce.end(), auto_producers_all.begin(), auto_producers_all.end());
        request_ans_produce.push_back(good);

    }

    Request& Request::exec(map<string, string>& MapHeaders, map<string, string>& MapCookies){
        metadata=std::pair<map<string, string>&, map<string, string>&> ({MapHeaders, MapCookies});
        if (headers!=NULL) curl_slist_free_all(headers);
        headers = NULL;
        for (const auto &now : MapHeaders){
            headers = curl_slist_append(headers, (now.first+": "+now.second).c_str());
        }
        stringstream cookies;
        for (const auto &now: MapCookies){
            cookies << now.first << "=" << now.second << "; ";
        }
        if (cookies.str().length())
            headers = curl_slist_append(headers, std::format("Cookie: {}", cookies.str().substr(0, cookies.str().length()-2)).c_str());

        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &metadata);
        return exec();
    }

    Request& Request::exec(){
#ifdef DEBUG
        //curl_easy_setopt(handle, CURLOPT_URL, URL.c_str());
        std::cout << format("URL: {}\nBODY: {}\n", URL, responce->str());
        std::cout << "Headers:\n";
        auto temp=headers;
        while (temp){
            std::cout << temp->data << "\n";
            temp=temp->next;
        }
        //std::cout << responce->str();
        std::cout << "End Headers\n\n";
#endif
        str="";
        if (request_ans_produce.back()!=just_repit_in_wrong && request::Request::RepitRequestInBad){
            request_ans_produce.push_back(just_repit_in_wrong);
        }


        int res=curl_easy_perform(handle);
        double secs;
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &res);

        code=res;
#ifdef DEBUG_STATUS
        std::cout << res << " CODE\n";
#endif
        curl_easy_getinfo(handle, CURLINFO_TOTAL_TIME, &secs);
        this->secs=secs;
        //std::cout << "Time " << secs << "-s\n"; std::cout.flush();
        unsigned int counter=0;
#ifdef DEBUG_STATUS
            std::cout << request_ans_produce.size() << " - count of producers\n";
#endif
        for (auto producer: request_ans_produce){
#ifdef DEBUG_STATUS
            std::cout << ++counter << "\n";
#endif
            if (producer(this)){
                if(responce!=nullptr) *responce=stringstream(str);
                if (headers!=NULL) curl_slist_free_all(headers);
                return *this;
            }
        }
        std::cout << res << "undefined_behavior\n";
        std::ofstream ou("undefined_behavior.txt"); ou << str; ou.close();

#ifdef DEBUG
        std::cout << res << "\n";
#endif

#ifdef DEBUG
        std::cout << "Answer Headers:\n";
        for (auto &h : metadata.first) {
            std::cout << h.first << ": " << h.second << "\n";
        }
        std::cout << "\n";
#endif
        if(responce!=nullptr) *responce=stringstream(str);

        if (headers!=NULL) curl_slist_free_all(headers);
        return *this;
    }

    const stringstream* Request::result(){return responce;}

    const string Request::get_temp_responce(){return str;}

    CURL* Request::get_curl_handle(){return this->handle;}

    std::pair<map<string, string>&, map<string, string>&> Request::get_metadata(){return {this->metadata.first, this->metadata.second};}
}

//32 байта в пинг = 66мс
