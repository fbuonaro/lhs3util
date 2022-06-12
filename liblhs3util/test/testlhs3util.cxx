#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>

#include <gtest/gtest.h>

#include <lhs3util/s3requestcontext.h>
#include <lhs3util_impl/s3requester.h>
#include <lhs3util/s3istreamuploader.h>

const char access_key[] = "minio-dev-access";
const char secret_key[] = "minio-dev-secret";
const char host[] = "minio-dev:9000";

namespace TestLHS3UtilNS
{

    std::string testBucketName( const char* postfix )
    {

        time_t tt;
        std::tm struct_tm;
        char timestr[ 64 ] = { 0 };
        std::ostringstream oss;

        tt = std::time( nullptr );
        gmtime_r( &tt, &struct_tm );

        strftime( timestr, 64, "%y%m%d%H%M%S", &struct_tm );

        oss << timestr << postfix;

        return oss.str();
    }

    class TestLHS3 : public ::testing::Test
    {
    protected:
        std::string emptyString;
        std::unique_ptr< LHS3UtilNS::S3RequestContext > requestContext;
        std::string inputFile1;
        std::string inputFile2;

        virtual void SetUp()
        {
            std::string hostName( host );
            std::string accessKey( access_key );
            std::string secretKey( secret_key );
            std::string resourceDir(
                std::getenv( "LHS3UTIL_RESOURCE_DIR" ) );
            std::ostringstream oss;


            emptyString = std::string( "" );
            requestContext = std::unique_ptr< LHS3UtilNS::S3RequestContext >(
                new LHS3UtilNS::S3RequestContext(
                    hostName, accessKey, secretKey ) );
            requestContext->SetTimeoutMS( 0 );

            oss << resourceDir << "/small_blob.bin";
            inputFile1 = oss.str();

            oss.str( std::string() );
            oss << resourceDir << "/large_blob.bin";
            inputFile2 = oss.str();
            std::cout << "using small/large file paths["
                << inputFile1 << "|" << inputFile2 << "]"
                << std::endl;
        }

        virtual void TearDown()
        {
        }
    };

    TEST_F( TestLHS3, TestRequestContext )
    {
        std::string hostName( "someHost" );
        std::string accessKey( "accessKey" );
        std::string secretKey( "secretKey" );
        int timeoutMS( 500 );
        bool thrown = false;

        try
        {
            LHS3UtilNS::S3RequestContext requestContext( hostName,
                accessKey,
                secretKey );

            ASSERT_EQ( hostName, requestContext.Host() );
            ASSERT_EQ( accessKey, requestContext.AccessKey() );
            ASSERT_EQ( secretKey, requestContext.SecretKey() );
            ASSERT_EQ( timeoutMS, requestContext.TimeoutMS() );
            ASSERT_EQ( emptyString, requestContext.SecurityToken() );
            ASSERT_EQ( emptyString, requestContext.AuthRegion() );

            thrown = false;
        }
        catch ( ... )
        {
            thrown = true;
        }
        ASSERT_FALSE( thrown ) << "Unexpected exception";

        try
        {
            LHS3UtilNS::S3RequestContext requestContext( emptyString,
                accessKey,
                secretKey );

            thrown = false;
        }
        catch ( ... )
        {
            thrown = true;
        }
        ASSERT_TRUE( thrown ) << "Empty host did not throw exception";

        try
        {
            LHS3UtilNS::S3RequestContext requestContext( hostName,
                emptyString,
                secretKey );

            thrown = false;
        }
        catch ( ... )
        {
            thrown = true;
        }
        ASSERT_TRUE( thrown ) << "Empty access key did not throw exception";

        try
        {
            LHS3UtilNS::S3RequestContext requestContext( hostName,
                accessKey,
                emptyString );

            thrown = false;
        }
        catch ( ... )
        {
            thrown = true;
        }
        ASSERT_TRUE( thrown ) << "Empty secret key did not throw exception";

    }

    TEST_F( TestLHS3, TestCreateBucket )
    {
        std::string hostName( "minio-dev" );
        std::string bucketName1( testBucketName( "testcreatebucket1" ) );
        std::string bucketName2( testBucketName( "testcreatebucket2" ) );
        std::string bucketName3( testBucketName( "testcreatebucket3" ) );
        std::string badBucketName1( "!!!@#@!#!@#@!#" );
        LHS3UtilNS::S3Ret ret;
        LHS3UtilImplNS::S3Requester requester;

        ret = requester.CreateBucket( *requestContext, bucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 1[" << ret.second << "]";

        ret = requester.CreateBucket( *requestContext, bucketName2 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 2[" << ret.second << "]";

        ret = requester.CreateBucket( *requestContext, bucketName3 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 3[" << ret.second << "]";

        ret = requester.CreateBucket( *requestContext, bucketName3 );
        ASSERT_EQ( LHS3UtilNS::S3Status::BucketExists, ret.first ) <<
            "Created duplicate bucket 3[" << ret.second << "]";

        ret = requester.CreateBucket( *requestContext, badBucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::BucketBadName, ret.first ) <<
            "Unexpected status resulted from creating bucket with bad name[" << ret.second << "]";

        ret = requester.CreateBucket( *requestContext, emptyString );
        ASSERT_EQ( LHS3UtilNS::S3Status::BucketBadName, ret.first ) <<
            "Unexpected status resulted from creating bucket with empty name[" << ret.second << "]";

    }

    TEST_F( TestLHS3, TestGetBuckets )
    {
        std::string hostName( "minio-dev" );
        std::string bucketName1( testBucketName( "testgetbucket1" ) );
        std::string bucketName2( testBucketName( "testgetbucket2" ) );
        std::string bucketName3( testBucketName( "testgetbucket3" ) );
        LHS3UtilNS::S3Ret ret;
        LHS3UtilImplNS::S3Requester requester;
        std::vector< std::string > buckets;
        bool foundBucket1 = false;
        bool foundBucket2 = false;
        bool foundBucket3 = false;

        ret = requester.CreateBucket( *requestContext, bucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 1[" << ret.second << "]";

        ret = requester.CreateBucket( *requestContext, bucketName2 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 2[" << ret.second << "]";

        ret = requester.CreateBucket( *requestContext, bucketName3 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 3[" << ret.second << "]";

        ret = requester.GetBuckets( *requestContext, buckets );

        ASSERT_TRUE( buckets.size() >= 3 ) <<
            "Not enough buckets found[" << buckets.size() << "]";

        for ( auto it = buckets.cbegin(); it != buckets.cend(); ++it )
        {
            if ( *it == bucketName1 )
                foundBucket1 = true;
            else if ( *it == bucketName2 )
                foundBucket2 = true;
            else if ( *it == bucketName3 )
                foundBucket3 = true;
        }

        ASSERT_TRUE( foundBucket1 ) << "Bucket 1 not found";
        ASSERT_TRUE( foundBucket2 ) << "Bucket 2 not found";
        ASSERT_TRUE( foundBucket3 ) << "Bucket 3 not found";

    }

    TEST_F( TestLHS3, TestBucketExists )
    {
        std::string hostName( "minio-dev" );
        std::string bucketName1( testBucketName( "testbucketexists1" ) );
        std::string bucketName2( testBucketName( "testbucketexists2" ) );
        std::string badBucketName1( "!!!@#@!#!@#@!#" );
        LHS3UtilNS::S3Ret ret;
        LHS3UtilImplNS::S3Requester requester;

        ret = requester.CreateBucket( *requestContext, bucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 1[" << ret.second << "]";

        ret = requester.BucketExists( *requestContext, bucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Bucket 1 does not exist[" << ret.second << "]";

        ret = requester.BucketExists( *requestContext, bucketName2 );
        ASSERT_EQ( LHS3UtilNS::S3Status::BucketNotFound, ret.first ) <<
            "Bucket 2 exists or unexpected status[" << ret.second << "]";

        ret = requester.BucketExists( *requestContext, badBucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::BucketBadName, ret.first ) <<
            "Bad Bucket exists or unexpected status[" << ret.second << "]";

        ret = requester.BucketExists( *requestContext, emptyString );
        ASSERT_EQ( LHS3UtilNS::S3Status::BucketNotFound, ret.first ) <<
            "Empty bucket name exists or unexpected status[" << ret.second << "]";


    }

    TEST_F( TestLHS3, TestDeleteBucket )
    {
        std::string hostName( "minio-dev" );
        std::string bucketName1( testBucketName( "testdeletebucket1" ) );
        std::string bucketName2( testBucketName( "testdeletebucket2" ) );
        std::string badBucketName1( "!!!@#@!#!@#@!#" );
        LHS3UtilNS::S3Ret ret;
        LHS3UtilImplNS::S3Requester requester;

        ret = requester.CreateBucket( *requestContext, bucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 1[" << ret.second << "]";

        ret = requester.DeleteBucket( *requestContext, bucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Cannot delete bucket 1[" << ret.second << "]";

        ret = requester.DeleteBucket( *requestContext, bucketName2 );
        ASSERT_EQ( LHS3UtilNS::S3Status::BucketNotFound, ret.first ) <<
            "Bucket 2 deletion unexpected status[" << ret.second << "]";

        ret = requester.BucketExists( *requestContext, badBucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::BucketBadName, ret.first ) <<
            "Bad Bucket deletion unexpected status[" << ret.second << "]";

        ret = requester.DeleteBucket( *requestContext, emptyString );
        ASSERT_EQ( LHS3UtilNS::S3Status::Error, ret.first ) <<
            "Empty bucket deletion unexpected status[" << ret.second << "]";


    }

    TEST_F( TestLHS3, TestUploadObject )
    {
        std::ifstream ifs( inputFile2 );
        std::ifstream ifs2( inputFile1 );
        LHS3UtilNS::S3IStreamUploader isu( ifs );
        LHS3UtilNS::S3IStreamUploader isu2( ifs2 );
        LHS3UtilNS::S3Ret ret;
        LHS3UtilImplNS::S3Requester requester;
        std::string bucketName1( testBucketName( "testuploadobject1" ) );
        std::string objectName1( "object" );

        ret = requester.CreateBucket( *requestContext, bucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 1[" << ret.second << "]";

        ret = requester.UploadObject( *requestContext, bucketName1, objectName1, isu );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to upload object 1[" << ret.second << "]";

        ret = requester.UploadObject( *requestContext, bucketName1, objectName1, isu );
        ASSERT_EQ( LHS3UtilNS::S3Status::Error, ret.first ) <<
            "Succeeded in reuploading zero-sized object[" << ret.second << "]";

        ret = requester.UploadObject( *requestContext, bucketName1, objectName1, isu2 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed in reuploading object 1 under same key[" << ret.second << "]";
    }

    TEST_F( TestLHS3, TestDownloadObject )
    {
        std::ifstream ifs( inputFile2 );
        LHS3UtilNS::S3IStreamUploader isu( ifs );
        LHS3UtilNS::S3Ret ret;
        LHS3UtilImplNS::S3Requester requester;
        std::string bucketName1( testBucketName( "testdownloadobject1" ) );
        std::string objectName1( "object" );
        std::string objectName2( "noSuchObject" );
        std::vector< char > downloadedChars;
        std::ifstream ifs2( inputFile1 );
        LHS3UtilNS::S3IStreamUploader isu2( ifs2 );

        ret = requester.CreateBucket( *requestContext, bucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 1[" << ret.second << "]";

        ret = requester.UploadObject( *requestContext, bucketName1, objectName1, isu );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to upload object 1[" << ret.second << "]";

        ret = requester.DownloadObject(
            *requestContext,
            bucketName1,
            objectName1,
            downloadedChars );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to download object 1[" << ret.second << "]";

        ASSERT_EQ( downloadedChars.size(), isu.TotalContentLength() ) <<
            "Downloaded chars are less than total content length";

        ifs.seekg( 0 );
        {
            const std::vector< char >& fileChars( isu.GetContentChars( downloadedChars.size() ) );
            auto downloadedCharIter = downloadedChars.cbegin();
            auto fileCharIter = fileChars.cbegin();
            unsigned long long pos = 0;

            ASSERT_EQ( downloadedChars.size(), fileChars.size() ) <<
                "Downloaded chars are not the same as the file chars";

            while ( ( downloadedCharIter != downloadedChars.cend() ) &&
                ( fileCharIter != fileChars.cend() ) )
            {
                ASSERT_EQ( *downloadedCharIter, *fileCharIter ) <<
                    "Downloaded char does not match file char[" << pos << "]";
                ++downloadedCharIter;
                ++fileCharIter;
                ++pos;
            }
        }

        ret = requester.DownloadObject(
            *requestContext,
            bucketName1,
            objectName2,
            downloadedChars );
        ASSERT_EQ( LHS3UtilNS::S3Status::ObjectNotFound, ret.first ) <<
            "Successfully downloaded object that should not exist[" << ret.second << "]";

        ret = requester.UploadObject( *requestContext, bucketName1, objectName1, isu2 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to replace object 1[" << ret.second << "]";

        downloadedChars.clear();
        ret = requester.DownloadObject(
            *requestContext,
            bucketName1,
            objectName1,
            downloadedChars );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to download updated object 1[" << ret.second << "]";

        ASSERT_EQ( downloadedChars.size(), isu2.TotalContentLength() ) <<
            "Downloaded chars are less than total content length";

        ifs2.seekg( 0 );
        {
            const std::vector< char >& fileChars( isu2.GetContentChars( downloadedChars.size() ) );
            auto downloadedCharIter = downloadedChars.cbegin();
            auto fileCharIter = fileChars.cbegin();
            unsigned long long pos = 0;

            ASSERT_EQ( downloadedChars.size(), fileChars.size() ) <<
                "Downloaded chars are not the same as the file chars";

            while ( ( downloadedCharIter != downloadedChars.cend() ) &&
                ( fileCharIter != fileChars.cend() ) )
            {
                ASSERT_EQ( *downloadedCharIter, *fileCharIter ) <<
                    "Downloaded char does not match file char[" << pos << "]";
                ++downloadedCharIter;
                ++fileCharIter;
                ++pos;
            }
        }
    }

    TEST_F( TestLHS3, TestCheckObject )
    {
        std::ifstream ifs( inputFile2 );
        std::ifstream ifs2( inputFile1 );
        LHS3UtilNS::S3IStreamUploader isu( ifs );
        LHS3UtilNS::S3IStreamUploader isu2( ifs2 );
        LHS3UtilNS::S3Ret ret;
        LHS3UtilImplNS::S3Requester requester;
        std::string bucketName1( testBucketName( "testcheckobject" ) );
        std::string objectName1( "object" );
        std::string objectName2( "object2" );
        std::string objectName3( "objectDoesNotExist" );
        uint64_t checkSize1( 0 );
        uint64_t checkSize2( 0 );

        ret = requester.CreateBucket( *requestContext, bucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 1[" << ret.second << "]";

        ret = requester.UploadObject( *requestContext, bucketName1, objectName1, isu );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to upload object 2[" << ret.second << "]";

        ret = requester.UploadObject( *requestContext, bucketName1, objectName2, isu2 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to upload object 1[" << ret.second << "]";

        ret = requester.CheckObject(
            *requestContext, bucketName1, objectName1, checkSize1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to check if object 1 exists[" << ret.second << "]";
        ASSERT_EQ( 387024, checkSize1 );

        ret = requester.CheckObject(
            *requestContext, bucketName1, objectName2, checkSize2 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to check if object 2 exists[" << ret.second << "]";
        ASSERT_EQ( 477, checkSize2 );

        ret = requester.CheckObject( *requestContext, bucketName1, objectName3 );
        ASSERT_EQ( LHS3UtilNS::S3Status::ObjectNotFound, ret.first ) <<
            "Failed to check if object 3 exists or object 3 actually exists[" << ret.second << "]";
    }

    TEST_F( TestLHS3, TestGetObjects )
    {
        std::ifstream ifs( inputFile2 );
        std::ifstream ifs2( inputFile1 );
        LHS3UtilNS::S3IStreamUploader isu( ifs );
        LHS3UtilNS::S3IStreamUploader isu2( ifs2 );
        LHS3UtilNS::S3Ret ret;
        LHS3UtilImplNS::S3Requester requester;
        std::string bucketName1( testBucketName( "testgetobjects" ) );
        std::string objectName1( "object" );
        std::string objectName2( "object2" );
        std::vector< std::string > objects;
        int numExpectedFound = 0;

        ret = requester.CreateBucket( *requestContext, bucketName1 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to create bucket 1[" << ret.second << "]";

        ret = requester.UploadObject( *requestContext, bucketName1, objectName1, isu );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to upload object 1[" << ret.second << "]";

        ret = requester.UploadObject( *requestContext, bucketName1, objectName2, isu2 );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to upload object 2[" << ret.second << "]";

        ret = requester.GetObjectsInBucket( *requestContext, bucketName1, objects );
        ASSERT_EQ( LHS3UtilNS::S3Status::OK, ret.first ) <<
            "Failed to get object names[" << ret.second << "]";

        ASSERT_EQ( 2, objects.size() ) << "Unexpected number of objects found";

        for ( auto it = objects.cbegin(); it != objects.cend(); ++it )
        {
            if ( *it == objectName1 )
                numExpectedFound += 1;
            else if ( *it == objectName2 )
                numExpectedFound += 3;
        }

        ASSERT_EQ( 4, numExpectedFound ) << "Did not find one of the objects";
    }
}