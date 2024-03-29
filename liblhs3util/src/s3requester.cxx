#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <lhs3util/s3istreamuploader.h>
#include <lhs3util/s3ostreamdownloader.h>
#include <lhs3util_impl/s3requester.h>

#include <sys/stat.h>

namespace LHS3UtilImplNS
{
    namespace
    {
        // TODO - replace with something common
        bool FileExists( const std::string& filePath )
        {
            struct stat statRet;
            return ( stat( filePath.c_str(), &statRet ) == 0 );
        }


        const char* cstrOrDefault( const std::string& str, const char* dflt )
        {
            return str.size() > 0 ? str.c_str() : dflt;
        }

        const char* cstrOrNULL( const std::string& str )
        {
            return cstrOrDefault( str, NULL );
        }

        //Passed to response and properties callbacks
        struct S3_cb_data
        {
            LHS3UtilNS::S3Ret& lhs3Ret;
            uint64_t contentLength;

            S3_cb_data( LHS3UtilNS::S3Ret& _lhs3Ret )
                : lhs3Ret( _lhs3Ret )
                , contentLength( 0 )
            {
            }

            S3_cb_data() = delete;
        };

        S3Status lhs3ResponsePropertiesCallback(
            const S3ResponseProperties* properties,
            void* callbackData )
        {
            S3_cb_data* lhs3CBData( (S3_cb_data*)callbackData );
            try
            {
                lhs3CBData->contentLength = properties->contentLength;
            }
            catch ( std::exception e )
            {
                if ( lhs3CBData != nullptr )
                {
                    try
                    {
                        lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInRspCallback;
                        lhs3CBData->lhs3Ret.second = "exception in props callback";
                    }
                    catch ( ... )
                    {
                        ;
                    }
                }
            }
            catch ( ... )
            {
                if ( lhs3CBData != nullptr )
                {
                    try
                    {
                        lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInAppCallback;
                        lhs3CBData->lhs3Ret.second =
                            "unknown exception in props callback";
                    }
                    catch ( ... )
                    {
                        ;
                    }
                }
            }

            return  S3StatusOK;
        }

        void lhs3ResponseCompleteCallback(
            S3Status status,
            const S3ErrorDetails *error,
            void* callbackData )
        {
            S3_cb_data* lhs3CBData( (S3_cb_data*)callbackData );
            try
            {
                if ( status != S3StatusOK )
                {
                    std::ostringstream oss;

                    oss << S3_get_status_name( status );
                    if ( error != nullptr )
                    {
                        if ( error->message != nullptr )
                            oss << "|" << error->message;

                        if ( error->resource != nullptr )
                            oss << "|" << error->resource;
                    }

                    if ( ( status == S3StatusErrorBucketAlreadyExists ) ||
                        ( status == S3StatusErrorBucketAlreadyOwnedByYou ) )
                        lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::BucketExists;
                    else if ( ( status == S3StatusInvalidBucketNameTooLong ) ||
                        ( status == S3StatusInvalidBucketNameFirstCharacter ) ||
                        ( status == S3StatusInvalidBucketNameCharacter ) ||
                        ( status ==
                            S3StatusInvalidBucketNameCharacterSequence ) ||
                        ( status == S3StatusInvalidBucketNameTooShort ) ||
                        ( status == S3StatusErrorInvalidBucketName ) ||
                        ( status == S3StatusInvalidBucketNameDotQuadNotation ) )
                        lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::BucketBadName;
                    else if ( status == S3StatusErrorNoSuchBucket )
                        lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::BucketNotFound;
                    else if ( status == S3StatusErrorBucketNotEmpty )
                        lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::BucketNotEmpty;
                    else if ( ( status == S3StatusErrorNoSuchKey ) ||
                        ( status == S3StatusHttpErrorNotFound ) )
                        lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::ObjectNotFound;
                    else
                        lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::Error;
                    lhs3CBData->lhs3Ret.second.assign( oss.str() );
                }
                else
                    lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::OK;
            }
            catch ( std::exception e )
            {
                if ( lhs3CBData != nullptr )
                {
                    try
                    {
                        lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInRspCallback;
                        lhs3CBData->lhs3Ret.second = e.what();
                    }
                    catch ( ... )
                    {
                        ;
                    }
                }
            }
            catch ( ... )
            {
                if ( lhs3CBData != nullptr )
                {
                    try
                    {
                        lhs3CBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInAppCallback;
                        lhs3CBData->lhs3Ret.second = "unknown exception";
                    }
                    catch ( ... )
                    {
                        ;
                    }
                }
            }
        }

        S3ResponseHandler lhs3ResponseHandler =
        {
            &lhs3ResponsePropertiesCallback,
            &lhs3ResponseCompleteCallback
        };

        //Passed to list service callback
        struct S3_list_service_cb_data : S3_cb_data
        {
            std::vector< std::string >& outBuckets;

            S3_list_service_cb_data( LHS3UtilNS::S3Ret& _lhs3Ret,
                std::vector< std::string >& _outBuckets )
                : S3_cb_data( _lhs3Ret )
                , outBuckets( _outBuckets )
            {
            };

            S3_list_service_cb_data() = delete;
        };

        S3Status lhs3ListServiceCallback( const char* ownerID,
            const char* ownerDisplayName,
            const char* bucketName,
            int64_t creationDateSeconds,
            void* callbackData )
        {
            S3Status status = S3StatusHttpErrorUnknown;
            S3_list_service_cb_data* lhs3ListCBData(
                (S3_list_service_cb_data*)callbackData );

            (void)( ownerID );
            (void)( ownerDisplayName );
            (void)( creationDateSeconds );

            try
            {
                lhs3ListCBData->outBuckets.emplace_back( bucketName );
                status = S3StatusOK;
            }
            catch ( std::exception e )
            {
                try
                {
                    lhs3ListCBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInAppCallback;
                    lhs3ListCBData->lhs3Ret.second = e.what();
                }
                catch ( ... )
                {
                    ;
                }

                status = S3StatusAbortedByCallback;
            }
            catch ( ... )
            {
                try
                {
                    lhs3ListCBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInAppCallback;
                    lhs3ListCBData->lhs3Ret.second = "unknown exception";
                }
                catch ( ... )
                {
                    ;
                }

                status = S3StatusAbortedByCallback;
            }

            return status;
        }

        S3ListServiceHandler lhs3ListServiceHandler =
        {
            lhs3ResponseHandler,
            &lhs3ListServiceCallback
        };

        //Passed to put object callbacks
        struct S3_put_object_cb_data : S3_cb_data
        {
            LHS3UtilNS::IObjectUploader& objectUploader;

            S3_put_object_cb_data( LHS3UtilNS::S3Ret& _lhs3Ret,
                LHS3UtilNS::IObjectUploader& _objectUploader )
                : S3_cb_data( _lhs3Ret )
                , objectUploader( _objectUploader )
            {
            };

            S3_put_object_cb_data() = delete;
        };

        //-1 indicates abort, 0 indicates finished
        int lhs3PutObjectCallback( int bufferSize, char* buffer, void* callbackData )
        {
            int ret = -1;
            S3_put_object_cb_data* lhs3PutObjectCBData(
                (S3_put_object_cb_data*)callbackData );

            try
            {
                if ( bufferSize == 0 )
                    ret = 0;
                else
                {
                    const std::vector< char >& objectContents(
                        lhs3PutObjectCBData->objectUploader.GetContentChars( bufferSize ) );
                    const char* charObjectContents( objectContents.data() );

                    memcpy( buffer, charObjectContents, objectContents.size() );

                    ret = objectContents.size();
                }
            }
            catch ( std::exception e )
            {
                try
                {
                    lhs3PutObjectCBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInAppCallback;
                    lhs3PutObjectCBData->lhs3Ret.second = e.what();
                }
                catch ( ... )
                {
                    ;
                }

                ret = -1;
            }
            catch ( ... )
            {
                try
                {
                    lhs3PutObjectCBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInAppCallback;
                    lhs3PutObjectCBData->lhs3Ret.second = "unknown exception";
                }
                catch ( ... )
                {
                    ;
                }

                ret = -1;
            }

            return ret;
        }

        S3PutObjectHandler lhs3PutObjectHandler =
        {
            lhs3ResponseHandler,
            &lhs3PutObjectCallback
        };

        //Passed to list bucket objects callback
        struct S3_list_bucket_cb_data : S3_cb_data
        {
            std::vector< std::string >& outObjects;

            S3_list_bucket_cb_data( LHS3UtilNS::S3Ret& _lhs3Ret,
                std::vector< std::string >& _outObjects )
                : S3_cb_data( _lhs3Ret )
                , outObjects( _outObjects )
            {
            };

            S3_list_bucket_cb_data() = delete;
        };

        S3Status lhs3ListBucketCallback( int isTruncated,
            const char* nextMarker,
            int contentsCount,
            const S3ListBucketContent* contents,
            int commonePrefixesCount,
            const char** commonPrefixes,
            void* callbackData )
        {
            (void)( isTruncated );
            (void)( nextMarker );
            (void)( commonePrefixesCount );
            (void)( commonPrefixes );

            S3Status status = S3StatusHttpErrorUnknown;
            S3_list_bucket_cb_data* lhs3ListCBData(
                (S3_list_bucket_cb_data*)callbackData );

            try
            {
                for ( int i = 0; i < contentsCount; ++i )
                {
                    const S3ListBucketContent *content = &( contents[ i ] );

                    if ( ( content != nullptr ) && ( content->key != nullptr ) )
                    {
                        lhs3ListCBData->outObjects.emplace_back( content->key );
                    }
                }

                status = S3StatusOK;
            }
            catch ( std::exception e )
            {
                try
                {
                    lhs3ListCBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInAppCallback;
                    lhs3ListCBData->lhs3Ret.second = e.what();
                }
                catch ( ... )
                {
                    ;
                }

                status = S3StatusAbortedByCallback;
            }
            catch ( ... )
            {
                try
                {
                    lhs3ListCBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInAppCallback;
                    lhs3ListCBData->lhs3Ret.second = "unknown exception";
                }
                catch ( ... )
                {
                    ;
                }

                status = S3StatusAbortedByCallback;
            }


            return status;
        }

        S3ListBucketHandler lhs3ListBucketHandler =
        {
            lhs3ResponseHandler,
            &lhs3ListBucketCallback
        };

        //Passed to get object callback
        struct S3_get_object_cb_data : S3_cb_data
        {
            LHS3UtilNS::IObjectDownloader& objectDownloader;

            S3_get_object_cb_data( LHS3UtilNS::S3Ret& _lhs3Ret,
                LHS3UtilNS::IObjectDownloader& _objectDownloader )
                : S3_cb_data( _lhs3Ret )
                , objectDownloader( _objectDownloader )
            {
            };

            S3_get_object_cb_data() = delete;
        };


        //return S3StatusAbortedByCallback to cancel
        S3Status lhs3GetObjectDataCallback(
            int bufferSize,
            const char *buffer,
            void* callbackData )
        {
            S3Status status = S3StatusHttpErrorUnknown;
            S3_get_object_cb_data* lhs3GetCBData(
                (S3_get_object_cb_data*)callbackData );

            try
            {

                lhs3GetCBData->objectDownloader.PutContentChars( bufferSize, buffer );

                status = S3StatusOK;
            }
            catch ( std::exception e )
            {
                try
                {
                    lhs3GetCBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInAppCallback;
                    lhs3GetCBData->lhs3Ret.second = e.what();
                }
                catch ( ... )
                {
                }

                status = S3StatusAbortedByCallback;
            }
            catch ( ... )
            {
                try
                {
                    lhs3GetCBData->lhs3Ret.first = LHS3UtilNS::S3Status::ExceptionInAppCallback;
                    lhs3GetCBData->lhs3Ret.second = "unknown exception";
                }
                catch ( ... )
                {
                }

                status = S3StatusAbortedByCallback;
            }

            return status;
        }

        S3GetObjectHandler lhs3GetObjectHandler =
        {
            lhs3ResponseHandler,
            &lhs3GetObjectDataCallback
        };

        class CharVectorObjectDownloader : public LHS3UtilNS::IObjectDownloader
        {
        public:
            CharVectorObjectDownloader( std::vector< char >& _downloadedChars )
                : LHS3UtilNS::IObjectDownloader()
                , downloadedChars( _downloadedChars )
            {}

            ~CharVectorObjectDownloader()
            {}

            void PutContentChars( int numChars, const char* chars )
            {
                std::vector< char >::size_type originalSize( downloadedChars.size() );

                downloadedChars.resize( downloadedChars.size() +
                    numChars );

                memcpy( downloadedChars.data() + originalSize,
                    chars,
                    numChars );
            }

        private:
            std::vector< char >& downloadedChars;
        };
    }

    S3Requester::S3Requester()
        : LHS3UtilNS::IS3Requester()
    {}

    S3Requester::~S3Requester()
    {}

    LHS3UtilNS::S3Ret S3Requester::GetBuckets( const LHS3UtilNS::S3RequestContext& requestContext,
        std::vector< std::string >& outBuckets )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::Error, std::string() );
        S3_list_service_cb_data lhs3ListCBData( lhs3Ret, outBuckets );

        S3_list_service(
            GetProtocol(),
            cstrOrNULL( requestContext.AccessKey() ),
            cstrOrNULL( requestContext.SecretKey() ),
            cstrOrNULL( requestContext.SecurityToken() ),
            cstrOrNULL( requestContext.Host() ),
            cstrOrNULL( requestContext.AuthRegion() ),
            nullptr,
            requestContext.TimeoutMS(),
            &lhs3ListServiceHandler,
            &lhs3ListCBData );

        return lhs3Ret;
    }

    LHS3UtilNS::S3Ret S3Requester::CreateBucket( const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::Error, std::string() );
        S3_cb_data lhs3CBData( lhs3Ret );
        S3Status s3Status = S3StatusOK;

        s3Status = S3_validate_bucket_name( cstrOrDefault( bucketName, "" ),
            S3UriStylePath );
        if ( s3Status == S3StatusOK )
        {
            S3_create_bucket(
                GetProtocol(),
                cstrOrNULL( requestContext.AccessKey() ),
                cstrOrNULL( requestContext.SecretKey() ),
                cstrOrNULL( requestContext.SecurityToken() ),
                cstrOrNULL( requestContext.Host() ),
                cstrOrNULL( bucketName ),
                cstrOrNULL( requestContext.AuthRegion() ),
                S3CannedAclPrivate, //TODO - what is this? Put in RequestContext
                nullptr,
                nullptr,
                requestContext.TimeoutMS(),
                &lhs3ResponseHandler,
                &lhs3CBData );
        }
        else
        {
            lhs3Ret.first = LHS3UtilNS::S3Status::BucketBadName;
            lhs3Ret.second.assign( S3_get_status_name( s3Status ) );
        }

        return lhs3Ret;
    }

    LHS3UtilNS::S3Ret S3Requester::BucketExists( const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::BucketNotFound, std::string() );
        S3_cb_data lhs3CBData( lhs3Ret );

        if ( bucketName.size() > 0 )
            S3_test_bucket( GetProtocol(),
                S3UriStylePath, //TODO - what is this? Putin RequestContext
                cstrOrNULL( requestContext.AccessKey() ),
                cstrOrNULL( requestContext.SecretKey() ),
                cstrOrNULL( requestContext.SecurityToken() ),
                cstrOrNULL( requestContext.Host() ),
                bucketName.c_str(),
                nullptr,
                0,
                nullptr,
                nullptr,
                requestContext.TimeoutMS(),
                &lhs3ResponseHandler,
                &lhs3CBData );

        return lhs3Ret;
    }

    LHS3UtilNS::S3Ret S3Requester::DeleteBucket( const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::Error, std::string() );
        S3_cb_data lhs3CBData( lhs3Ret );

        if ( bucketName.size() > 0 )
            S3_delete_bucket( GetProtocol(),
                S3UriStylePath, //TODO - what isthis?Putin RequestContext
                cstrOrNULL( requestContext.AccessKey() ),
                cstrOrNULL( requestContext.SecretKey() ),
                cstrOrNULL( requestContext.SecurityToken() ),
                cstrOrNULL( requestContext.Host() ),
                bucketName.c_str(),
                nullptr,
                nullptr,
                requestContext.TimeoutMS(),
                &lhs3ResponseHandler,
                &lhs3CBData );

        return lhs3Ret;
    }

    LHS3UtilNS::S3Ret S3Requester::GetObjectsInBucket(
        const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName,
        std::vector< std::string >& outObjects )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::Error, std::string() );
        S3BucketContext bucketContext =
        {
            requestContext.Host().c_str(),
            bucketName.c_str(),
            GetProtocol(),
            S3UriStylePath,
            requestContext.AccessKey().c_str(),
            requestContext.SecretKey().c_str(),
            cstrOrNULL( requestContext.SecurityToken() ),
            cstrOrNULL( requestContext.AuthRegion() )
        };
        S3_list_service_cb_data lhs3ListCBData( lhs3Ret, outObjects );

        S3_list_bucket(
            &bucketContext,
            nullptr,    //TODO - prefix for matching keys
            nullptr,    //TODO - marker, only keys after this key are returned
            nullptr,    //TODO - delimiter, keys that contain same string between
            //       prefix and delimiter are rolled into a single
            //       result element
            0,          //TODO - max keys to return
            nullptr,
            requestContext.TimeoutMS(),
            &lhs3ListBucketHandler,
            &lhs3ListCBData );

        return lhs3Ret;
    }

    LHS3UtilNS::S3Ret S3Requester::UploadObject( const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName,
        const std::string& objectName,
        LHS3UtilNS::IObjectUploader& objectUploader )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::Error, std::string() );
        S3BucketContext bucketContext =
        {
            requestContext.Host().c_str(),
            bucketName.c_str(),
            GetProtocol(),
            S3UriStylePath,
            requestContext.AccessKey().c_str(),
            requestContext.SecretKey().c_str(),
            cstrOrNULL( requestContext.SecurityToken() ),
            cstrOrNULL( requestContext.AuthRegion() )
        };
        S3_put_object_cb_data data( lhs3Ret, objectUploader );


        if ( ( bucketName.size() > 0 ) && ( objectName.size() > 0 ) )
            S3_put_object(
                &bucketContext,
                objectName.c_str(),
                objectUploader.TotalContentLength(),
                nullptr,
                nullptr,
                requestContext.TimeoutMS(),
                &lhs3PutObjectHandler,
                &data );

        return lhs3Ret;
    }

    LHS3UtilNS::S3Ret S3Requester::DownloadObject( const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName,
        const std::string& objectName,
        LHS3UtilNS::IObjectDownloader& objectDownloader )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::Error, std::string() );
        S3BucketContext bucketContext =
        {
            requestContext.Host().c_str(),
            bucketName.c_str(),
            GetProtocol(),
            S3UriStylePath,
            requestContext.AccessKey().c_str(),
            requestContext.SecretKey().c_str(),
            cstrOrNULL( requestContext.SecurityToken() ),
            cstrOrNULL( requestContext.AuthRegion() )
        };
        S3_get_object_cb_data data( lhs3Ret, objectDownloader );


        if ( ( bucketName.size() > 0 ) && ( objectName.size() > 0 ) )
            S3_get_object(
                &bucketContext,
                objectName.c_str(),
                nullptr,    //TODO - support get conditions
                0,          //TODO - support start byte
                0,          //TODO - support byte count
                nullptr,
                requestContext.TimeoutMS(),
                &lhs3GetObjectHandler,
                &data );


        return lhs3Ret;
    }

    LHS3UtilNS::S3Ret S3Requester::DownloadObject( const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName,
        const std::string& objectName,
        std::vector< char >& chars )
    {
        CharVectorObjectDownloader charsObjectDownloader( chars );
        return DownloadObject(
            requestContext,
            bucketName,
            objectName,
            charsObjectDownloader );
    }

    LHS3UtilNS::S3Ret S3Requester::DeleteObjectFromBucket(
        const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName,
        const std::string& objectName )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::Error, std::string() );
        S3BucketContext bucketContext =
        {
            requestContext.Host().c_str(),
            bucketName.c_str(),
            GetProtocol(),
            S3UriStylePath,
            requestContext.AccessKey().c_str(),
            requestContext.SecretKey().c_str(),
            cstrOrNULL( requestContext.SecurityToken() ),
            cstrOrNULL( requestContext.AuthRegion() )
        };
        S3_cb_data lhs3CBData( lhs3Ret );


        if ( ( bucketName.size() > 0 ) && ( objectName.size() > 0 ) )
            S3_delete_object(
                &bucketContext,
                objectName.c_str(),
                nullptr,
                requestContext.TimeoutMS(),
                &lhs3ResponseHandler,
                &lhs3CBData );

        return lhs3Ret;
    }

    LHS3UtilNS::S3Ret S3Requester::CheckObject(
        const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName,
        const std::string& objectName )
    {
        uint64_t contentLengthBytes( 0 );
        return CheckObject( requestContext,
            bucketName, objectName, contentLengthBytes );
    }

    LHS3UtilNS::S3Ret S3Requester::CheckObject(
        const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName,
        const std::string& objectName,
        uint64_t& contentLengthBytes )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::Error, std::string() );
        S3BucketContext bucketContext =
        {
            requestContext.Host().c_str(),
            bucketName.c_str(),
            GetProtocol(),
            S3UriStylePath,
            requestContext.AccessKey().c_str(),
            requestContext.SecretKey().c_str(),
            cstrOrNULL( requestContext.SecurityToken() ),
            cstrOrNULL( requestContext.AuthRegion() )
        };
        S3_cb_data lhs3CBData( lhs3Ret );


        contentLengthBytes = 0;

        if ( ( bucketName.size() > 0 ) && ( objectName.size() > 0 ) )
            S3_head_object(
                &bucketContext,
                objectName.c_str(),
                nullptr,
                requestContext.TimeoutMS(),
                &lhs3ResponseHandler,
                &lhs3CBData );

        contentLengthBytes = lhs3CBData.contentLength;

        return lhs3Ret;
    }

    LHS3UtilNS::S3Ret S3Requester::DownloadObjectToFile(
        const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName,
        const std::string& objectName,
        const std::string& filePath )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::Error, std::string() );

        if ( FileExists( filePath ) )
        {
            lhs3Ret.first = LHS3UtilNS::S3Status::Error;
            lhs3Ret.second = "file exists";
        }
        else
        {
            std::ofstream ofs( filePath );
            if ( ofs.is_open() )
            {
                LHS3UtilNS::S3OStreamDownloader ostreamDownloader( ofs );
                lhs3Ret = DownloadObject(
                    requestContext, bucketName, objectName, ostreamDownloader );
            }
            else
            {
                lhs3Ret.first = LHS3UtilNS::S3Status::Error;
                lhs3Ret.second = "failed to open file for writing";
            }
        }

        return lhs3Ret;
    }

    LHS3UtilNS::S3Ret S3Requester::UploadFileToObject(
        const LHS3UtilNS::S3RequestContext& requestContext,
        const std::string& bucketName,
        const std::string& objectName,
        const std::string& filePath )
    {
        LHS3UtilNS::S3Ret lhs3Ret( LHS3UtilNS::S3Status::Error, std::string() );

        std::ifstream ifs( filePath );
        if ( ifs.is_open() )
        {
            LHS3UtilNS::S3IStreamUploader istreamUploader( ifs );
            lhs3Ret = UploadObject(
                requestContext, bucketName, objectName, istreamUploader );
        }
        else
        {
            lhs3Ret.first = LHS3UtilNS::S3Status::Error;
            lhs3Ret.second = "failed to open file for reading";
        }

        return lhs3Ret;
    }

    S3Protocol S3Requester::GetProtocol() const
    {
        return S3ProtocolHTTP;
    }

    S3RequesterFactory::S3RequesterFactory()
        : LHS3UtilNS::IS3RequesterFactory()
    {}

    S3RequesterFactory::~S3RequesterFactory()
    {}

    std::unique_ptr< LHS3UtilNS::IS3Requester > S3RequesterFactory::CreateS3Requester() const
    {
        return std::unique_ptr< S3Requester >( new S3Requester() );
    }

    GlobalS3RequesterFactory::GlobalS3RequesterFactory( const std::string& defaultHostName )
        : s3RequesterFactory()
    {
        S3Status status = S3_initialize( 0, S3_INIT_ALL, defaultHostName.c_str() );
        if ( status != S3StatusOK )
        {
            std::ostringstream oss;
            oss << "Failed to initialize libs3[ " << S3_get_status_name( status ) << "]";
            throw std::runtime_error( oss.str() );
        }
    }

    GlobalS3RequesterFactory::~GlobalS3RequesterFactory()
    {
        S3_deinitialize();
    }

    std::unique_ptr< LHS3UtilNS::IS3Requester > GlobalS3RequesterFactory::CreateS3Requester() const
    {
        return s3RequesterFactory.CreateS3Requester();
    }
}

namespace LHS3UtilNS
{
    std::shared_ptr< IS3RequesterFactory > CreateStandardS3RequesterFactoryOnce( const S3RequesterFactoryParams& params )
    {
        return LHMiscUtilNS::OneTimeCreate< LHS3UtilImplNS::GlobalS3RequesterFactory >::Create( params.defaultHostName );
    }
}