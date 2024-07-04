#include "msi_auth_lib.h"

#include <string>
#include <memory>
#include <azure/core.hpp>
#include <azure/identity.hpp>

int getEntraIDToken(const char * tokenScope, char** token, char** errors)
{
    // Check we have params
    if (tokenScope == nullptr || token == nullptr || errors == nullptr) {
        return -1;
    }
    try
    {
        // Build a string from the provided scope
        std::string scope(tokenScope);

        // Set up our credential, here we are using DefaultAzureCredential to allow the code to be run anywhere
        //  But this could be switched to WorkloadIdentityCredential to only use that one auth method
        auto azureCredential = std::make_shared<Azure::Identity::DefaultAzureCredential>();

        // Build out our request, specifying the token scope
        Azure::Core::Credentials::TokenRequestContext requestContext;
        requestContext.Scopes = {scope};

        // Request the access token
        auto accessToken{azureCredential->GetToken(requestContext, Azure::Core::Context::ApplicationContext)};

        // Copy out the token, could also pass back expiry
        *token = new char[accessToken.Token.length() + 1];
        std::strcpy(*token, accessToken.Token.c_str());

        return 0;
    }
    catch (const Azure::Core::Credentials::AuthenticationException& exception)
    {
        // Get the auth error message
        auto errorMessage = exception.what();

        // Copy it out
        *errors = new char[strlen(errorMessage) + 1];
        std::strcpy(*errors, errorMessage);       
    }

    return 1;
}