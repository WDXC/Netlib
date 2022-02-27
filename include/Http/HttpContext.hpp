
#ifndef HTTPCONTEXT_H_
#define HTTPCONTEXT_H_

#include "HttpRequest.hpp"
#include "../Socket/Buffer.hpp"

class HttpContext {
    public:
        enum HttpRequestParseState {
            kExpectRequestLine,
            kExpectHeaders,
            kExpectBody,
            kGotAll
        };
        HttpContext() : contextState_(kExpectRequestLine) {
        }

        bool parseRequest(Buffer* buf, TimeStamp receTime);
        bool gotAll() const {
            return contextState_ == kGotAll;
        }

        void reset() {
            contextState_ = kExpectRequestLine;
            HttpRequest dummy;
            request_.swap(dummy);
        }

        HttpRequest& getRequest() {
            return request_;
        }

        const HttpRequest& getRequest() const {
            return request_;
        }

    private:
        bool processRequestLine(const char* begin, const char* end);

    private:
        HttpRequestParseState contextState_;
        HttpRequest request_;
};

#endif