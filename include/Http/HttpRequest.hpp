/*** 
 * @Author: Zty
 * @Date: 2022-02-19 11:54:27
 * @LastEditTime: 2022-02-19 20:30:21
 * @LastEditors: Zty
 * @Description: 
 * @FilePath: /multhread/src/Http/HttpRequest.hpp
 */

#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include <string>
#include <map>
#include "../Timer/TimeStamp.hpp"

class HttpRequest {
    public:
        enum Method {
            kInvalid,
            kGet,
            kPost,
            kHead,
            kPut,
            kDelete
        };
        enum Version {
            kUnkown,
            kHttp10,
            kHttp11
        };

        HttpRequest() : 
            method_(kInvalid),
            version_(kUnkown) {
            
        }

        void setVersion(Version v) {
            version_ = v;
        }

        Version getVersion() const {
            return version_;
        }

        bool setMethod(const char* start, const char* end) {
            std::string m(start, end);
            if (m == "GET") {
                method_ = kGet;
            } else if (m == "POST") {
                method_ = kPost;
            } else if (m == "HEAD") {
                method_ = kHead;
            } else if (m == "PUT") {
                method_ = kPut;
            } else if (m == "DELETE") {
                method_ = kDelete;
            }

            return method_ != kInvalid;
        }

        Method getmethod() const {
            return method_; 
        }

        const char* methodString() const {
            const char* res = "UNKNOWN";
            switch (method_) {
                case kGet:
                    res = "GET";
                    break;
                case kPost:
                    res = "POST";
                    break;
                case kHead:
                    res = "HEAD";
                    break;
                case kPut:
                    res = "PUT";
                    break;
                case kDelete:
                    res = "DELETE";
                    break;
            }
            return res;
        }

        void setPath(const char* start, const char* end) {
            path_.assign(start, end);
        }

        const std::string& GetPath() const {
            return path_;
        }

        void setQuery(const char* start, const char* end) {
            query_.assign(start, end);
        }

        const std::string Query() const {
            return query_;
        }

        void setReceiveTime(TimeStamp t) {
            receTime_ = t;
        }

        TimeStamp getReceiveTime() const {
            return receTime_;
        }

        void addHeader(const char* start, const char* colon, const char* end) {
            std::string field(start, colon);
            ++colon;
            while (colon < end && isspace(*colon)) {
                ++colon;
            }
            std::string value(colon, end);
            while (!value.empty() && isspace(value[value.size()-1])) {
                value.resize(value.size()-1);
            }
            headers_[field] = value;
        }

        std::string getHeader(const std::string& field) const {
            std::string res;
            std::map<std::string, std::string>::const_iterator it = headers_.find(field);
            if (it != headers_.end()) {
                res = it->second;
            }
            return res;
        }

        const std::map<std::string, std::string>& Header() const {
            return headers_;
        }

        void swap(HttpRequest& that) {
            std::swap(method_, that.method_);
            std::swap(version_, that.version_);
            path_.swap(that.path_);
            query_.swap(that.query_);
            receTime_.swap(that.receTime_);
            headers_.swap(that.headers_);
        }
    
    private:
        Method method_;
        Version version_;
        std::string path_;
        std::string query_;
        TimeStamp receTime_;
        std::map<std::string, std::string> headers_;
};

#endif