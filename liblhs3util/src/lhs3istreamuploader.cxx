#include <lhs3util/lhs3istreamuploader.h>

namespace LHS3UtilNS
{
    LHS3IStreamUploader::LHS3IStreamUploader( std::istream& _is )
    :   is( _is )
    ,   buffer()
    {}

    const std::vector< char >&
    LHS3IStreamUploader::GetContentChars( int numChars )
    {
        buffer.resize( numChars / sizeof( char ) );

        std::cerr << "LHS3IStreamUploader.GetContentChars: getting"
                  << " " << numChars << std::endl;
        
        is.read( buffer.data(), numChars / sizeof( char ) );

        buffer.resize( is.gcount() );

        return buffer;
    }

    size_t
    LHS3IStreamUploader::TotalContentLength() const
    {
        size_t length = 0;
        std::streampos pos( is.tellg() );

        std::cerr << "LHS3IStreamUploader.TotalContentLength: entering"
                  << std::endl;

        is.seekg(0, is.end);
        length = is.tellg();
        is.seekg( pos );

        std::cerr << "LHS3IStreamUploader.TotalContentLength: " << length << std::endl;

        return length;
    }
}
