#ifndef __LHS3UTIL_IS3REQUESTER_H__
#define __LHS3UTIL_IS3REQUESTER_H__

#include <memory>
#include <string>
#include <vector>

#include <lhs3util/s3requestcontext.h>

namespace LHS3UtilNS
{
    enum class S3Status : std::uint16_t
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

    typedef std::pair< S3Status, std::string > S3Ret;

    class IObjectUploader
    {
    public:
        IObjectUploader();
        virtual ~IObjectUploader();

        //callback has signature int bufferSize, char* buffer, void* data
        virtual const std::vector< char >& GetContentChars( int numChars ) = 0;
        virtual size_t TotalContentLength() const = 0;
    };

    class IObjectDownloader
    {
    public:
        IObjectDownloader();
        virtual ~IObjectDownloader();

        virtual void PutContentChars( int numChars, const char* chars ) = 0;
    };

    class IS3Requester
    {
    public:
        IS3Requester();
        virtual ~IS3Requester();

        virtual S3Ret GetBuckets( const S3RequestContext& requestContext,
            std::vector< std::string >& outBuckets ) = 0;

        virtual S3Ret CreateBucket( const S3RequestContext& requestContext,
            const std::string& bucketName ) = 0;

        virtual S3Ret DeleteBucket( const S3RequestContext& requestContext,
            const std::string& bucketName ) = 0;

        virtual S3Ret BucketExists( const S3RequestContext& requestContext,
            const std::string& bucketName ) = 0;

        virtual S3Ret GetObjectsInBucket( const S3RequestContext& requestContext,
            const std::string& bucketName,
            std::vector< std::string >& outObjects ) = 0;

        //Upload Object into Bucket By Name
        virtual S3Ret UploadObject( const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            IObjectUploader& objectUploader ) = 0;

        //Download Object from Bucket By Name
        virtual S3Ret DownloadObject( const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            IObjectDownloader& objectDownloader ) = 0;

        virtual S3Ret DownloadObject( const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            std::vector< char >& chars ) = 0;

        virtual S3Ret CheckObject( const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName ) = 0;

        virtual S3Ret CheckObject( const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            uint64_t& contentLengthBytes ) = 0;

        virtual S3Ret DeleteObjectFromBucket(
            const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName ) = 0;

        virtual S3Ret DownloadObjectToFile(
            const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            const std::string& filePath ) = 0;

        virtual S3Ret UploadFileToObject(
            const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            const std::string& filePath ) = 0;
    };

    class IS3RequesterFactory
    {
    public:
        IS3RequesterFactory();
        virtual ~IS3RequesterFactory();

        virtual std::unique_ptr< IS3Requester > CreateS3Requester() const = 0;
    };

    struct S3RequesterFactoryParams
    {
        std::string defaultHostName;
    };

    std::shared_ptr< IS3RequesterFactory > CreateStandardS3RequesterFactoryOnce( const S3RequesterFactoryParams& params );
    std::shared_ptr< IS3RequesterFactory > GetInjectedS3RequesterFactory();
    std::unique_ptr< IS3Requester > CreateS3RequesterWithInjectedFactory();
}

#include <lhmiscutil/singleton.h>

namespace LHMiscUtilNS
{
    EnableClassAsSingleton( LHS3UtilNS::IS3RequesterFactory, SingletonCanBeSet::WhenEmpty );
}

#endif