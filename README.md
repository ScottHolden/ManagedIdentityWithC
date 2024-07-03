# Managed Identity With C
Managed Identity with C, connecting to an Azure PostgreSQL 

This is a small sample to show an approach of wrapping the C++ Azure SDK and using it in a C application. 

## Getting Started
### Locally
1. Install the dependencies required to build:
```bash
# Ubuntu/Debian
apt install -y git gcc g++ cmake make libcurl4-openssl-dev libpq-dev libxml2-dev

### OR ###

# RedHat/CentOS/Fedora
dnf install -y git gcc g++ cmake make libcurl-devel libpq-devel libxml2-devel openssl-devel
```
2. Clone the repository:
```bash
git clone https://github.com/ScottHolden/ManagedIdentityWithC.git
```
3. Build the project:
```bash
mkdir build && cd build
cmake ..
cmake --build .
```
4. Run the project:
```bash
# You will either need to be on an Azure VM with managed identity enabled, or have the Azure CLI installed and be logged in as a user with access to the Azure PostgreSQL Flex server.
DB_NAME=postgres DB_HOSTNAME=example.postgres.database.azure.com DB_USERNAME=example@example.com ./app/SampleApp
```

### On Azure (proof of concept)
You can use the "Deploy to Azure" button below to deploy an Azure Container App running this code, and an Azure PostgreSQL Flex server.

[![Deploy To Azure](https://aka.ms/deploytoazurebutton)](https://portal.azure.com/#create/Microsoft.Template/uri/https%3A%2F%2Fraw.githubusercontent.com%2FScottHolden%2FManagedIdentityWithC%2Fmain%2Fdeploy%2Fdeploy.generated.json)  
`Estimated deployment time: 8 minutes` 

By default it will use a pre-built container image & a burstable B1ms Azure PostgreSQL Flex SKU, but you can modify these settings on the parameters page.

**Note:** _Due to the order of resources being deployed the first execution of the Container App may fail. This is because the administrator resource that gives the Container App may not be created before the Container App is started. If this happens, simply restart the Container App._  

Once deployed you can open the Container App resource (named `cmsipg`), click "Log Stream" under "Monitoring" to see the output of the application.

## How it works
Within the [lib](lib/) directory there is a small example wrapper that exposes a method to request an Access Token using the Azure Identity SDK. This uses the [DefaultAzureCredential](https://devblogs.microsoft.com/azure-sdk/authentication-and-the-azure-sdk/) implementation to authenticate, which will try to authenticate using environment variables, managed identity (including workload identity), and the Azure CLI. 

This process means the code does not need to change between running locally on a developers machine using their local identity (via Azure CLI) and a service within Azure with Managed Identity enabled. 

The [app](app/) directory contains an example application written in C that uses this wrapper to authenticate and connect to an Azure PostgreSQL Flex server. You can specify `DB_NAME`, `DB_HOSTNAME`, and `DB_USERNAME` environment variables to connect. 

Finally, there is a [Dockerfile](Dockerfile) that builds the application and its dependencies into a container image. This can be used to run the application in a container, either locally or in Azure.