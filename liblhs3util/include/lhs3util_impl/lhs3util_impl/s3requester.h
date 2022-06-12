#ifndef __LHS3UTIL_IMPL_S3REQUESTER_H__
#define __LHS3UTIL_IMPL_S3REQUESTER_H__

#include <libs3.h>

#include <lhs3util/is3requester.h>

namespace LHS3UtilImplNS
{
    class S3Requester : public LHS3UtilNS::IS3Requester
    {
    public:
        S3Requester();
        ~S3Requester();

        LHS3UtilNS::S3Ret GetBuckets( const LHS3UtilNS::S3RequestContext& requestContext,
            std::vector< std::string >& outBuckets );

        LHS3UtilNS::S3Ret CreateBucket( const LHS3UtilNS::S3RequestContext& requestContext,
            const std::string& bucketName );

        LHS3UtilNS::S3Ret DeleteBucket( const LHS3UtilNS::S3RequestContext& requestContext,
            const std::string& bucketName );

        LHS3UtilNS::S3Ret BucketExists( const LHS3UtilNS::S3RequestContext& requestContext,
            const std::string& bucketName );

        LHS3UtilNS::S3Ret GetObjectsInBucket( const LHS3UtilNS::S3RequestContext& requestContext,
            const std::string& bucketName,
            std::vector< std::string >& outObjects );

        //Upload Object into Bucket By Name
        LHS3UtilNS::S3Ret UploadObject( const LHS3UtilNS::S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            LHS3UtilNS::IObjectUploader& objectUploader );

        //Download Object from Bucket By Name
        LHS3UtilNS::S3Ret DownloadObject( const LHS3UtilNS::S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            LHS3UtilNS::IObjectDownloader& objectDownloader );

        LHS3UtilNS::S3Ret DownloadObject( const LHS3UtilNS::S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            std::vector< char >& chars );

        LHS3UtilNS::S3Ret CheckObject( const LHS3UtilNS::S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName );

        LHS3UtilNS::S3Ret CheckObject( const LHS3UtilNS::S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            uint64_t& contentLengthBytes );

        LHS3UtilNS::S3Ret DeleteObjectFromBucket(
            const LHS3UtilNS::S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName );

        S3Protocol GetProtocol() const;
    };

    class S3RequesterFactory : public LHS3UtilNS::IS3RequesterFactory
    {
    public:
        S3RequesterFactory();
        ~S3RequesterFactory();

        std::unique_ptr< LHS3UtilNS::IS3Requester > CreateS3Requester() const;
    };

    class GlobalS3RequesterFactory : public LHS3UtilNS::IS3RequesterFactory
    {
    public:
        GlobalS3RequesterFactory( const std::string& defaultHostName );
        ~GlobalS3RequesterFactory();

        GlobalS3RequesterFactory( const GlobalS3RequesterFactory& other ) = delete;
        GlobalS3RequesterFactory& operator=( const GlobalS3RequesterFactory& other ) = delete;

        std::unique_ptr< LHS3UtilNS::IS3Requester > CreateS3Requester() const;

    private:
        S3RequesterFactory s3RequesterFactory;
    };
}

#include <lhmiscutil/onetimecreate.h>

namespace LHMiscUtilNS
{
    EnableClassAsOneTimeCreate( LHS3UtilImplNS::GlobalS3RequesterFactory );
}

#endif