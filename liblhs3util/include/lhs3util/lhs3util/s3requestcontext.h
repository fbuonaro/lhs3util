#ifndef __LHS3UTIL_S3REQUESTCONTEXT_H__
#define __LHS3UTIL_S3REQUESTCONTEXT_H__

#include <string>

namespace LHS3UtilNS
{
    class S3RequestContext
    {
    public:
        //CTOR
        S3RequestContext( const std::string& host,
            const std::string& accessKey,
            const std::string& secretKey );

        const std::string& Host() const;
        const std::string& AccessKey() const;
        const std::string& SecretKey() const;

        const std::string& SecurityToken() const;
        void SetSecurityToken( const std::string& _securityToken );

        int TimeoutMS() const;
        void SetTimeoutMS( int _timeoutMS );

        const std::string& AuthRegion() const;
        void SetAuthRegion( const std::string& _authRegion );

    private:
        S3RequestContext() = delete;

        //MEMBERS
        std::string host;
        std::string accessKey;
        std::string secretKey;
        std::string securityToken;
        int timeoutMS;
        std::string authRegion;
    };
}

#endif