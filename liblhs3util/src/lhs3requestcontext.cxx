#include <stdexcept>

#include <lhs3util/lhs3requestcontext.h>

namespace LHS3UtilNS
{
    LHS3RequestContext::LHS3RequestContext( const std::string& _host,
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

    void SetSecurityToken( const std::string& _securityToken )
    {
        securityToken = _securityToken;
    }

    const std::string& LHS3RequestContext::SecurityToken() const
    {
        return securityToken;
    }

    void LHS3RequestContext::SetTimeoutMS( int _timeoutMS )
    {
        timeoutMS = _timeoutMS;
    }

    int LHS3RequestContext::TimeoutMS() const
    {
        return timeoutMS;
    }

    void SetAuthRegion( const std::string& _authRegion )
    {
        authRegion = _authRegion;
    }

    const std::string& LHS3RequestContext::AuthRegion() const
    {
        return authRegion;
    }

    const std::string& LHS3RequestContext::Host() const
    {
        return host;
    }

    const std::string& LHS3RequestContext::AccessKey() const
    {
        return accessKey;
    }

    const std::string& LHS3RequestContext::SecretKey() const
    {
        return secretKey;
    }
}