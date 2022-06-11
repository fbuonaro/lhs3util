#ifndef __LHS3UTIL_LHS3ISTREAMUPLOADER_H__
#define __LHS3UTIL_LHS3ISTREAMUPLOADER_H__

#include <iostream>
#include <vector>

#include <lhs3util/ilhs3requester.h>

namespace LHS3UtilNS
{
    class LHS3IStreamUploader : public IObjectUploader
    {
    public:
        LHS3IStreamUploader( std::istream& _is );

        const std::vector< char >& GetContentChars( int numChars );

        size_t TotalContentLength() const;

    private:
        std::istream& is;
        std::vector< char > buffer;
    };

}

#endif