#ifndef __LHS3UTIL_LHS3REQUESTER_H__
#define __LHS3UTIL_LHS3REQUESTER_H__

#include <string>
#include <vector>

#include <lhs3util/lhs3requestcontext.h>

namespace LHS3UtilNS
{
    enum class LHS3Status : std::uint16_t
    {
        OK,
        BucketNotFound,
        BucketExists,
        BucketBadName,
        BucketNotEmpty,
        ObjectNotFound,
        Error,
        ExceptionInAppCallback,
        ExceptionInRspCallback
    };

    typedef std::pair< LHS3Status, std::string > LHS3Ret;

    class ObjectUploader
    {
        public:
            //callback has signature int bufferSize, char* buffer, void* data
            virtual const std::vector< char >& GetContentChars( int numChars ) = 0;
            virtual size_t TotalContentLength() const = 0;
            virtual ~ObjectUploader();
    };

    class ObjectDownloader
    {
        public:
            virtual void PutContentChars( int numChars, const char* chars ) = 0;
            virtual ~ObjectDownloader();
    };

    class LHS3Requester
    {
        public:
            LHS3Ret GetBuckets( const LHS3RequestContext& requestContext,
                                std::vector< std::string >& outBuckets );

            LHS3Ret CreateBucket( const LHS3RequestContext& requestContext,
                                  const std::string& bucketName );

            LHS3Ret DeleteBucket( const LHS3RequestContext& requestContext,
                                  const std::string& bucketName );

            LHS3Ret BucketExists( const LHS3RequestContext& requestContext,
                                  const std::string& bucketName );

            LHS3Ret GetObjectsInBucket( const LHS3RequestContext& requestContext,
                                        const std::string& bucketName,
                                        std::vector< std::string >& outObjects );

            //Upload Object into Bucket By Name
            LHS3Ret UploadObject( const LHS3RequestContext& requestContext,
                                  const std::string& bucketName,
                                  const std::string& objectName,
                                  ObjectUploader& objectUploader );

            //Download Object from Bucket By Name
            LHS3Ret DownloadObject( const LHS3RequestContext& requestContext,
                                    const std::string& bucketName,
                                    const std::string& objectName,
                                    ObjectDownloader& objectDownloader );

            LHS3Ret DownloadObject( const LHS3RequestContext& requestContext,
                                    const std::string& bucketName,
                                    const std::string& objectName,
                                    std::vector< char >& chars );

            LHS3Ret CheckObject( const LHS3RequestContext& requestContext,
                                 const std::string& bucketName,
                                 const std::string& objectName );

            LHS3Ret CheckObject( const LHS3RequestContext& requestContext,
                                 const std::string& bucketName,
                                 const std::string& objectName,
                                 uint64_t& contentLengthBytes );

            LHS3Ret DeleteObjectFromBucket(
                const LHS3RequestContext& requestContext,
                const std::string& bucketName,
                const std::string& objectName );

    };
}

#endif
