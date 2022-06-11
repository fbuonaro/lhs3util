#ifndef __LHS3UTIL_IMPL_LHS3REQUESTERIMPL_H__
#define __LHS3UTIL_IMPL_LHS3REQUESTERIMPL_H__

#include <lhs3util/lhs3requester.h>

namespace LHS3UtilImplNS
{
    class LHS3RequesterImpl : public LHS3UtilNS::ILHS3Requester
    {
    public:
        LHS3RequesterImpl();
        ~LHS3RequesterImpl();

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
            IObjectUploader& objectUploader );

        //Download Object from Bucket By Name
        LHS3Ret DownloadObject( const LHS3RequestContext& requestContext,
            const std::string& bucketName,
            const std::string& objectName,
            IObjectDownloader& objectDownloader );

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

    class LHS3RequesterFactoryImpl : public LHS3UtilNS::ILHS3RequesterFactory
    {
    public:
        LHS3RequesterFactoryImpl();
        ~LHS3RequesterFactoryImpl();

        std::unique_ptr< ILHS3Requester > CreateLHS3Requester() const;
    };
}

#endif