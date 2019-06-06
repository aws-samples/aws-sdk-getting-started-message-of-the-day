using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Amazon;
using Amazon.CognitoIdentity;
using Amazon.Lambda;
using Amazon.Lambda.Model;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace DynamicMOTD_NET
{
    class Program
    {
        private static IAmazonLambda lambdaClient;
        private static string s_playerName = "Lord Default";

        private static void SetPlayerName()
        {
            Console.Write("Type the player name you'd like to use: ");
            s_playerName = Console.ReadLine();
            Console.WriteLine($"OK, player name set to {s_playerName}");
        }

        private static void InvokeLambda(string playername, int playerlevel, int playergold)
        {
            JObject jsonPayload = new JObject {
                { "playername", playername },
                { "playerlevel", playerlevel },
                { "playergold", playergold } };

            InvokeRequest request = new InvokeRequest
            {
                FunctionName = "GetMOTD",
                InvocationType = InvocationType.RequestResponse,
                Payload = jsonPayload.ToString()
            };
            InvokeResponse response = lambdaClient.Invoke(request);
            if(response.StatusCode == 200)
            {
                var payloadStreamReader = new StreamReader(response.Payload);
                var jsonReader = new JsonTextReader(payloadStreamReader);
                var jsonObj = (JObject)(new JsonSerializer().Deserialize(jsonReader));
                Console.WriteLine($"Todays Message: {jsonObj["body"].Value<string>()}");
            }
            else
            {
                Console.WriteLine($"Lambda error: {response.FunctionError}");
            }
        }

        private static bool Menu()
        {
            Console.WriteLine("What would you like to do?");
            Console.WriteLine("\t1. Set player name");
            Console.WriteLine("\t2. Test level 5 player with 10 gold");
            Console.WriteLine("\t3. Test level 5 player with 200 gold");
            Console.WriteLine("\t4. Test level 20 player with 100000 gold");
            Console.WriteLine("\t9. Quit");

            string selection = Console.ReadLine();

            switch (selection)
            {
                case "1":
                    SetPlayerName();
                    break;
                case "2":
                    InvokeLambda(s_playerName, 5, 10);
                    break;
                case "3":
                    InvokeLambda(s_playerName, 5, 200);
                    break;
                case "4":
                    InvokeLambda(s_playerName, 20, 100000);
                    break;
                case "9":
                    return false;
                default:
                    Console.WriteLine("That choice doesn't exist, please try again.");
                    break;
            }
            return true;
        }

        public static void RunMainLoop()
        {
            Console.WriteLine("Welcome to The Game!");
            while(Menu())
            {
                System.Threading.Thread.Sleep(0);
            }
        }

        public static void Main(string[] args)
        {
            CognitoAWSCredentials credentials = new CognitoAWSCredentials(
                "us-east-1:xx0000x0-0x00-0000-0000-0x0000000xxx", // Identity pool ID
                RegionEndpoint.USEast1 // Region
            );

            lambdaClient = new AmazonLambdaClient(credentials, RegionEndpoint.USEast1);

            RunMainLoop();
        }
    }
}
