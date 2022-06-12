#include <lhs3util/s3istreamuploader.h>

namespace LHS3UtilNS
{
    S3IStreamUploader::S3IStreamUploader( std::istream& _is )
        : is( _is )
        , buffer()
    {}

    const std::vector< char >&
        S3IStreamUploader::GetContentChars( int numChars )
    {
        buffer.resize( numChars / sizeof( char ) );

        std::cerr << "S3IStreamUploader.GetContentChars: getting"
            << " " << numChars << std::endl;

        is.read( buffer.data(), numChars / sizeof( char ) );

        buffer.resize( is.gcount() );

        return buffer;
    }

    size_t
        S3IStreamUploader::TotalContentLength() const
    {
        size_t length = 0;
        std::streampos pos( is.tellg() );

        std::cerr << "S3IStreamUploader.TotalContentLength: entering"
            << std::endl;

        is.seekg( 0, is.end );
        length = is.tellg();
        is.seekg( pos );

        std::cerr << "S3IStreamUploader.TotalContentLength: " << length << std::endl;

        return length;
    }
}
