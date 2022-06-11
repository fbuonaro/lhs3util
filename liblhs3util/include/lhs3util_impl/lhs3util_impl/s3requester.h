#ifndef __LHS3UTIL_IMPL_S3REQUESTERIMPL_H__
#define __LHS3UTIL_IMPL_S3REQUESTERIMPL_H__

#include <lhs3util/lhs3requester.h>

namespace LHS3UtilImplNS
{
    class S3RequesterImpl : public LHS3UtilNS::IS3Requester
    {
    public:
        S3RequesterImpl();
        ~S3RequesterImpl();

        S3Ret GetBuckets( const S3RequestContext& requestContext,
            std::vector< std::string >& outBuckets );

        S3Ret CreateBucket( const S3RequestContext& requestContext,
            const std::string& bucketName );

        S3Ret DeleteBucket( const S3RequestContext& requestContext,
            const std::string& bucketName );

        S3Ret BucketExists( const S3RequestContext& requestContext,
            const std::string& bucketName );

        S3Ret GetObjectsInBucket( const S3RequestContext& requestContext,
            const std::string& bucketName,
            std::vector< std::string >& outObjects );

        //Upload Object into Bucket By Name
        S3Ret UploadObject( const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            IObjectUploader& objectUploader );

        //Download Object from Bucket By Name
        S3Ret DownloadObject( const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            IObjectDownloader& objectDownloader );

        S3Ret DownloadObject( const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            std::vector< char >& chars );

        S3Ret CheckObject( const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName );

        S3Ret CheckObject( const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            uint64_t& contentLengthBytes );

        S3Ret DeleteObjectFromBucket(
            const S3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName );
    };

    class S3RequesterFactoryImpl : public LHS3UtilNS::IS3RequesterFactory
    {
    public:
        S3RequesterFactoryImpl();
        ~S3RequesterFactoryImpl();

        std::unique_ptr< IS3Requester > CreateS3Requester() const;
    };

    class GlobalS3RequesterFactoryImpl : public LHS3UtilNS::IS3RequesterFactory
    {
    public:
        GlobalS3RequesterFactoryImpl( const std::string& defaultHostName );
        ~GlobalS3RequesterFactoryImpl();

        GlobalS3RequesterFactoryImpl( const GlobalS3RequesterFactoryImpl& other ) = delete;
        GlobalS3RequesterFactoryImpl& operator=( const GlobalS3RequesterFactoryImpl& other ) = delete;

        std::unique_ptr< IS3Requester > CreateS3Requester() const;

    private:
        S3RequesterFactoryImpl s3RequesterFactoryImpl;
    };
}

#include <lhmiscutil/onetimecreate.h>

namespace LHMiscUtilNS
{
    EnableClassAsOneTimeCreate( LHS3UtilImplNS::GlobalS3RequesterFactoryImpl );
}

#endif