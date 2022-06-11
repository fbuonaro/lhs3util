#ifndef __LHS3UTIL_IMPL_LHS3PROCESSCONTEXT_H__
#define __LHS3UTIL_IMPL_LHS3PROCESSCONTEXT_H__

#include <exception>
#include <string>

#include <libs3.h>

namespace LHS3UtilImplNS
{
    class LHS3ProcessContext
    {
    public:
        static void ProcessInitialize( const std::string& defaultHostName );
        static void ProcessCleanup();
        static const std::string& DefaultHostName();

    private:
        //STATIC DATA
        static LHS3ProcessContext processInstance;
        static bool processInitialized;

        std::string defaultHostName;

    protected:
        //CTOR/DTOR
        LHS3ProcessContext();
        ~LHS3ProcessContext();

        //Not allowed
        LHS3ProcessContext( const LHS3ProcessContext& other ) = delete;
        LHS3ProcessContext( LHS3ProcessContext&& other ) = delete;
        LHS3ProcessContext& operator=( const LHS3ProcessContext& other ) = delete;
        LHS3ProcessContext& operator=( LHS3ProcessContext&& other ) = delete;

        //Methods
        void Initialize( const std::string& defaultHostName );
        void Cleanup();
    };
}

#endif