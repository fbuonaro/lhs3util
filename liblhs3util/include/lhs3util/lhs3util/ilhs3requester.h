#ifndef __LHS3UTIL_ILHS3REQUESTER_H__
#define __LHS3UTIL_ILHS3REQUESTER_H__

#include <memory>
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

    class ILHS3Requester
    {
    public:
        ILHS3Requester();
        virtual ~ILHS3Requester();

        virtual LHS3Ret GetBuckets( const LHS3RequestContext& requestContext,
            std::vector< std::string >& outBuckets ) = 0;

        virtual LHS3Ret CreateBucket( const LHS3RequestContext& requestContext,
            const std::string& bucketName ) = 0;

        virtual LHS3Ret DeleteBucket( const LHS3RequestContext& requestContext,
            const std::string& bucketName ) = 0;

        virtual LHS3Ret BucketExists( const LHS3RequestContext& requestContext,
            const std::string& bucketName ) = 0;

        virtual LHS3Ret GetObjectsInBucket( const LHS3RequestContext& requestContext,
            const std::string& bucketName,
            std::vector< std::string >& outObjects ) = 0;

        //Upload Object into Bucket By Name
        virtual LHS3Ret UploadObject( const LHS3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            IObjectUploader& objectUploader ) = 0;

        //Download Object from Bucket By Name
        virtual LHS3Ret DownloadObject( const LHS3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            IObjectDownloader& objectDownloader ) = 0;

        virtual LHS3Ret DownloadObject( const LHS3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            std::vector< char >& chars ) = 0;

        virtual LHS3Ret CheckObject( const LHS3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName ) = 0;

        virtual LHS3Ret CheckObject( const LHS3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            uint64_t& contentLengthBytes ) = 0;

        virtual LHS3Ret DeleteObjectFromBucket(
            const LHS3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName ) = 0;
    };

    class ILHS3RequesterFactory
    {
    public:
        ILHS3RequesterFactory();
        virtual ~ILHS3RequesterFactory();

        virtual std::unique_ptr< ILHS3Requester > CreateLHS3Requester() const = 0;
    };

    std::shared_ptr< ILHS3RequesterFactory > GetInjectedLHS3RequesterFactory();
    std::shared_ptr< ILHS3RequesterFactory > CreateStandardLHS3RequesterFactoryOnce();
}

#include <lhmiscutil/singleton.h>

namespace LHMiscUtilNS
{
    EnableClassAsSingleton( LHS3UtilNS::ILHS3RequesterFactory, SingletonCanBeSet::WhenEmpty );
}

#endif