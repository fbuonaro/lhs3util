#include <lhs3util/s3ostreamdownloader.h>

namespace LHS3UtilNS
{
    S3OstreamDownloader::S3OstreamDownloader( std::ostream& _os )
        : LHS3UtilNS::IObjectDownloader()
        , os( _os )
    {}

    S3OstreamDownloader::~S3OstreamDownloader()
    {}

    void S3OstreamDownloader::PutContentChars( int numChars, const char* chars )
    {
        if ( numChars && chars )
            os.write( chars, numChars );
    }
}