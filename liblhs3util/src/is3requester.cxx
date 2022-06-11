#include <lhs3util/ilhs3requester.h>

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

    ILHS3Requester::ILHS3Requester()
    {}

    ILHS3Requester::~ILHS3Requester()
    {}

    ILHS3RequesterFactory::ILHS3RequesterFactory()
    {}

    ILHS3RequesterFactory::~ILHS3RequesterFactory()
    {}

    std::shared_ptr< ILHS3RequesterFactory > GetInjectedLHS3RequesterFactory()
    {
        return LHMiscUtilNS::Singleton< ILHS3RequesterFactory >::GetInstance();
    }

    std::unique_ptr< IS3Requester > CreateS3RequesterWithInjectedFactory()
    {
        auto factory( GetInjectedLHS3RequesterFactory() );
        if ( factory )
            return factory->CreateS3Requester();
        else
            return std::nullptr;
    }
}