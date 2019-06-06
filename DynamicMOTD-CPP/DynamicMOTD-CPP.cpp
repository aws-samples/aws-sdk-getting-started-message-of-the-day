#include <iostream>
#include <thread>

#include <aws/core/Aws.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/utils/Outcome.h>

#include <aws/identity-management/auth/PersistentCognitoIdentityProvider.h>
#include <aws/identity-management/auth/CognitoCachingCredentialsProvider.h>

#include <aws/lambda/LambdaClient.h>
#include <aws/lambda/model/InvokeRequest.h>
#include <aws/lambda/model/InvokeResult.h>

#include "settings.h"

using namespace std;

static shared_ptr<Aws::Lambda::LambdaClient> s_LambdaClient;
static string s_playerName = "Lord Default";

void SetPlayerName()
{
    cout << "Type the player name you'd like to use: ";
    cin >> s_playerName;

    cout << "OK, player name set to " << s_playerName << endl;
}

void InvokeLambda(string playername, int playerlevel, int playergold)
{
    Aws::Lambda::Model::InvokeRequest invokeRequest;
    invokeRequest.SetFunctionName("GetMOTD");
    invokeRequest.SetInvocationType(Aws::Lambda::Model::InvocationType::RequestResponse);
    std::shared_ptr<Aws::IOStream> payload = Aws::MakeShared<Aws::StringStream>("LambdaFunctionRequest");
    Aws::Utils::Json::JsonValue jsonPayload;
    jsonPayload.WithString("playername", playername);
    jsonPayload.WithInteger("playerlevel", playerlevel);
    jsonPayload.WithInteger("playergold", playergold);
    *payload << jsonPayload.View().WriteReadable();
    invokeRequest.SetBody(payload);
    invokeRequest.SetContentType("application/javascript");

    auto outcome = s_LambdaClient->Invoke(invokeRequest);

    if (outcome.IsSuccess())
    {
        auto& result = outcome.GetResult();
        Aws::Utils::Json::JsonValue resultPayload{ result.GetPayload() };
        auto jsonView = resultPayload.View();

        if (jsonView.ValueExists("body"))
        {
            cout << "Todays Messae: " << jsonView.GetString("body") << endl << endl;
        }
        else
        {
            cout << "Unable to parse todays message!" << endl << endl;
        }

    }
    else
    {
        auto error = outcome.GetError();
        cout << "Error invoking lambda: " << error.GetMessage() << endl << endl;

    }
}

bool Menu()
{
    cout << endl << "What would you like to do?" << endl;
    cout << "\t1. Set player name" << endl;
    cout << "\t2. Test level 5 player with 10 gold" << endl;
    cout << "\t3. Test level 5 player with 200 gold" << endl;
    cout << "\t4. Test level 20 player with 100000 gold" << endl;
    cout << "\t9. Quit" << endl;
    cout << endl << "Your choice? ";

    int menuSelection = 0;
    if (!(cin >> menuSelection))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    switch (menuSelection)
    {
    case 1:
        SetPlayerName();
        break;

    case 2:
        InvokeLambda(s_playerName, 5, 10);
        break;

    case 3:
        InvokeLambda(s_playerName, 5, 200);
        break;

    case 4:
        InvokeLambda(s_playerName, 20, 100000);
        break;

    case 9:
        // quit
        return false;

    default:
        cout << "That choice doesn't exist, please try again." << endl << endl;
    }
    return true;
}

int RunMainLoop()
{
    cout << "Welcome to The Game!" << endl;

    while (Menu())
    {
        std::this_thread::sleep_for(std::chrono::seconds(0));
    }
    return 0;
}

int main()
{
    Aws::SDKOptions options;

    Aws::Utils::Logging::LogLevel logLevel{ Aws::Utils::Logging::LogLevel::Error };
    options.loggingOptions.logger_create_fn = [logLevel] {return make_shared<Aws::Utils::Logging::ConsoleLogSystem>(logLevel); };

    Aws::InitAPI(options);

    auto credentialsProvider = Aws::MakeShared<Aws::Auth::CognitoCachingAnonymousCredentialsProvider>("CredentialsProvider", ACCT_ID, POOL_ID);

    s_LambdaClient = Aws::MakeShared<Aws::Lambda::LambdaClient>("LambdaClient", credentialsProvider);

    int exitStatus = RunMainLoop();

    Aws::ShutdownAPI(options);
    return exitStatus;
}

