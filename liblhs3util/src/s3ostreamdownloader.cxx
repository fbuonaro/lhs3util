#include <lhs3util/s3ostreamdownloader.h>

namespace LHS3UtilNS
{
    S3OStreamDownloader::S3OStreamDownloader( std::ostream& _os )
        : LHS3UtilNS::IObjectDownloader()
        , os( _os )
    {}

    S3OStreamDownloader::~S3OStreamDownloader()
    {}

    void S3OStreamDownloader::PutContentChars( int numChars, const char* chars )
    {
        if ( numChars && chars )
            os.write( chars, numChars );
    }
}