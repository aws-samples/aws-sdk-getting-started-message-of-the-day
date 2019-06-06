# AWS SDK Getting Started Message Of The Day

A simple project for either C++ or .Net that uses AWS Lambda, Amazon S3 and Amazon Cognito anonymous authentication to show how you can quickly and easily integrate Amazon Web Services in to your game.

# Requirements
- An AWS account with access to Amazon Cognito, AWS Lambda and Amazon S3: https://aws.amazon.com/getting-started/
- Microsoft Visual Studio 2017 or higher (any edition): https://visualstudio.microsoft.com/

# Contents
<pre>
├── DynamicMOTD-CPP/         # C++ version of the client
├── DynamicMOTD-NET/         # Contains solution file for .NET/C# as well as the client project
│    └── DynamicMOTD-NET/    # Contains the client project and source code
└── s3_files/                # Demo files to upload to S3 (required by the AWS Lambda configured in steps below)
</pre>

# Building the sample

## Set up AWS resources
1. Create a unique S3 bucket.
2. Create a Lambda and call it GetMOTD. Choose Node.js 8.10 as the runtime.
3. When creating the Lambda, create a new IAM role that will give the Lambda read only access to the S3 bucket created in step 1.
4. Add the following code to the Lambda:
<pre>
var AWS = require('aws-sdk');
var S3 = new AWS.S3();

var motdKeys = {
    newbNeedsGold: "newb_needs_fakegold.txt",
    newbNeedsSword: "newb_needs_better_weapon.txt",
    prodMultiplayer: "multi_player_prod.txt",
    default: "default.txt"
}
var motdBucket = "motd.yourorg.com"; // replace with your bucket name
exports.handler = async (event, context, callback) => {
    var motdKey = motdKeys.default;
    if(event.playerlevel > 10){
        motdKey = motdKeys.prodMultiplayer;
    }
    else {
        if(event.playergold < 100){
            motdKey = motdKeys.newbNeedsGold;
        }
        else {
            motdKey = motdKeys.newbNeedsSword;
        }
    }
    var response;
    await S3.getObject({
        Bucket: motdBucket,
        Key: motdKey
    }).promise().then(data => {
        var motdText = data.Body.toString('ascii');
        motdText = motdText.replace("@playername@", event.playername);
        response = {
            statusCode: 200,
            body: motdText
        };
    }).catch(err => {
        response = err;
    });
    return response;
};
</pre>
5. Create an Amazon Cognito identity pool that allow access from unathenticated identities.
6. Give permissions for unathenticated identities to invoke the Lambda function created in step 3.

## Build the sample

### .NET/C#
1. Load the solution DynamicMOTD-NET.sln in Visual Studio.
2. Open the Program.cs file.
3. Edit the CognitoAWSCredentials constructor to use the identity pool ID created above.
4. Build and run.

### C++
1. Load the solution DynamicMOTD-CPP.sln in Visual Studio.
2. Open the Settings.h file.
3. Replace the ACCT_ID string with your AWS account ID.
4. Replace the POOL_ID string with the ID of the identity pool created above.
5. Build and run.

# For more information or questions
- The steps in this file are condensed from the article found here: https://aws.amazon.com/blogs/gametech/
- Please contact gametech@amazon.com for any comments or requests regarding this content

# License Summary
This sample code is made available under the MIT-0 license. See the LICENSE file.
