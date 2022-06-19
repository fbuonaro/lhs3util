#ifndef __LHS3UTIL_S3OSTREAMDOWNLOADER_H__
#define __LHS3UTIL_S3OSTREAMDOWNLOADER_H__

#include <iostream>

#include <lhs3util/is3requester.h>

namespace LHS3UtilNS
{
    class S3OStreamDownloader : public LHS3UtilNS::IObjectDownloader
    {
    public:
        S3OStreamDownloader( std::ostream& _os );

        ~S3OStreamDownloader();

        void PutContentChars( int numChars, const char* chars );

    private:
        std::ostream& os;
    };
}

#endif