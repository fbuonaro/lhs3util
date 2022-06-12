#include <stdexcept>

#include <lhs3util/s3requestcontext.h>

namespace LHS3UtilNS
{
    S3RequestContext::S3RequestContext( const std::string& _host,
        const std::string& _accessKey,
        const std::string& _secretKey )
        : host( _host )
        , accessKey( _accessKey )
        , secretKey( _secretKey )
        , securityToken()
        , timeoutMS( 500 )
        , authRegion()
    {
        if ( host.size() <= 0 )
        {
            throw std::runtime_error( "host cannot be empty" );
        }
        else if ( accessKey.size() <= 0 )
        {
            throw std::runtime_error( "accessKey cannot be empty" );
        }
        else if ( secretKey.size() <= 0 )
        {
            throw std::runtime_error( "secretKey cannot be empty" );
        }
    }

    void S3RequestContext::SetSecurityToken( const std::string& _securityToken )
    {
        securityToken = _securityToken;
    }

    const std::string& S3RequestContext::SecurityToken() const
    {
        return securityToken;
    }

    void S3RequestContext::SetTimeoutMS( int _timeoutMS )
    {
        timeoutMS = _timeoutMS;
    }

    int S3RequestContext::TimeoutMS() const
    {
        return timeoutMS;
    }

    void S3RequestContext::SetAuthRegion( const std::string& _authRegion )
    {
        authRegion = _authRegion;
    }

    const std::string& S3RequestContext::AuthRegion() const
    {
        return authRegion;
    }

    const std::string& S3RequestContext::Host() const
    {
        return host;
    }

    const std::string& S3RequestContext::AccessKey() const
    {
        return accessKey;
    }

    const std::string& S3RequestContext::SecretKey() const
    {
        return secretKey;
    }
}