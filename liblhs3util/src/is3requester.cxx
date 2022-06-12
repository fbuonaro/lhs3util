#include <lhs3util/is3requester.h>

namespace LHS3UtilNS
{
    IObjectUploader::IObjectUploader()
    {}

    IObjectUploader::~IObjectUploader()
    {}

    IObjectDownloader::IObjectDownloader()
    {}

    IObjectDownloader::~IObjectDownloader()
    {}

    IS3Requester::IS3Requester()
    {}

    IS3Requester::~IS3Requester()
    {}

    IS3RequesterFactory::IS3RequesterFactory()
    {}

    IS3RequesterFactory::~IS3RequesterFactory()
    {}

    std::shared_ptr< IS3RequesterFactory > GetInjectedS3RequesterFactory()
    {
        return LHMiscUtilNS::Singleton< IS3RequesterFactory >::GetInstance();
    }

    std::unique_ptr< IS3Requester > CreateS3RequesterWithInjectedFactory()
    {
        return GetInjectedS3RequesterFactory()->CreateS3Requester();
    }
}