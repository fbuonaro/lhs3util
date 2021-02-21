#include <sstream>

#include <libs3.h>

#include <lhs3util/lhs3processcontext.h>

namespace LHS3UtilNS
{

    LHS3ProcessContextInitializationFailed::LHS3ProcessContextInitializationFailed(
    S3Status _status )
    :   status( _status )
    {
        std::ostringstream oss;
        oss << "Failed to initialize libs3[ " 
            << S3_get_status_name( status );
        message.assign( oss.str() );
    }

    const char* LHS3ProcessContextInitializationFailed::what() const throw()
    {
        return message.c_str();
    }

    const char* LHS3ProcessContextAlreadyInitialized::what() const throw()
    {
        return "libs3 already initialized";
    }

    //STATIC API
    bool LHS3ProcessContext::processInitialized = false;
    LHS3ProcessContext LHS3ProcessContext::processInstance;

    void LHS3ProcessContext::ProcessInitialize( const std::string& defaultHostName )
    {
        if( !LHS3ProcessContext::processInitialized )
        {
            LHS3ProcessContext::processInstance.Initialize( defaultHostName );
            LHS3ProcessContext::processInitialized = true;
        }
        else
            throw LHS3ProcessContextAlreadyInitialized();
    }

    void LHS3ProcessContext::ProcessCleanup()
    {
        if( LHS3ProcessContext::processInitialized )
        {
            LHS3ProcessContext::processInstance.Cleanup();
            processInitialized = false;
        }
    }

    const std::string& LHS3ProcessContext::DefaultHostName()
    {
        return LHS3ProcessContext::processInstance.defaultHostName;
    }

    void LHS3ProcessContext::Initialize( const std::string& _defaultHostName )
    {
        S3Status status = S3_initialize(0, S3_INIT_ALL, defaultHostName.c_str() );


        if( status != S3StatusOK )
            throw LHS3ProcessContextInitializationFailed( status );

        defaultHostName = _defaultHostName;
    }

    LHS3ProcessContext::LHS3ProcessContext()
    : defaultHostName()
    {
    }

    LHS3ProcessContext::~LHS3ProcessContext()
    {
        Cleanup();
    }

    void LHS3ProcessContext::Cleanup()
    {
        S3_deinitialize();
    }
}
