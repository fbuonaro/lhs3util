#ifndef __LHS3UTIL_LHS3REQUESTCONTEXT_H__
#define __LHS3UTIL_LHS3REQUESTCONTEXT_H__

#include <string>

#include <libs3.h>

namespace LHS3UtilNS
{
    class LHS3RequestContext
    {
        public:
            //CTOR
            LHS3RequestContext( const std::string& host,
                                const std::string& accessKey,
                                const std::string& secretKey );

            const std::string& Host() const;
            const std::string& AccessKey() const;
            const std::string& SecretKey() const;

            S3Protocol Protocol() const;

            const std::string& SecurityToken() const;

            int TimeoutMS() const;
            void SetTimeoutMS( int _timeoutMS );

            const std::string& AuthRegion() const;

        private:
            LHS3RequestContext() = delete;

            //MEMBERS
            std::string host;
            std::string accessKey;
            std::string secretKey;
            S3Protocol protocol;
            std::string securityToken;
            int timeoutMS;
            std::string authRegion;
    };
}

#endif
