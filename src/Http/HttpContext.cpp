#include "HttpContext.hpp"

#include <string.h>
#include <unistd.h>

#include "Json_data.hpp"

bool HttpContext::processRequestLine(const char* begin, const char* end) {
  bool successed = false;
  const char* start = begin;
  const char* space = std::find(start, end, ' ');
  if (space != end && request_.setMethod(start, space)) {
    start = space + 1;
    space = std::find(start, end, ' ');
    if (space != end) {
      const char* question = std::find(start, space, '?');
      if (question != space) {
        request_.setPath(start, question);
        request_.setQuery(question, space);
      } else {
        request_.setPath(start, space);
      }

      start = space + 1;
      successed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
      if (successed) {
        if (*(end - 1) == '1') {
          request_.setVersion(HttpRequest::kHttp11);
        } else if (*(end - 1) == '0') {
          request_.setVersion(HttpRequest::kHttp10);
        } else {
          successed = false;
        }
      }
    }
  }
  return successed;
}

bool HttpContext::parseRequest(Buffer* m_buf, TimeStamp recetime) {
  bool ok = true;
  bool hasMore = true;
	char* method = nullptr;
  while (hasMore) {
    if (contextState_ == kExpectRequestLine) {
      const char* crlf = m_buf->findCRLF();
			if (method == nullptr) {
				method = strstr(const_cast<char*>(m_buf->peek()), "GET");
			}
      if (crlf) {
        ok = processRequestLine(m_buf->peek(), crlf);
        if (ok) {
          request_.setReceiveTime(recetime);
          m_buf->retrieveUntil(crlf + 2);
          contextState_ = kExpectHeaders;
        } else {
          hasMore = false;
        }
      } else {
        hasMore = false;
      }
    } else if (contextState_ == kExpectHeaders) {
      const char* crlf = m_buf->findCRLF();
      if (crlf) {
        const char* colon = std::find(m_buf->peek(), crlf, ':');
        if (colon != crlf) {
          request_.addHeader(m_buf->peek(), colon, crlf);
        } else {
          char* curly_brackes = strstr(const_cast<char*>(crlf), "{");
          if (curly_brackes && !method) {
            contextState_ = kExpectBody;
          } else {
            contextState_ = kGotAll;
            hasMore = false;
          }
        }
        m_buf->retrieveUntil(crlf + 2);
      } else {
        hasMore = false;
      }
    } else if (contextState_ == kExpectBody) {
      if (!request_.setRequestBody(m_buf->peek())) {
				request_.setRequestBody("NULL");
        hasMore = false;
      } else {
				std::string str = m_buf->peek();
				m_buf->retrieveUntil(m_buf->peek() + str.size());
        contextState_ = kGotAll;
        hasMore = false;
      }
    }
  }
  return ok;
}
