# ManagedIdentityWithC
Managed Identity with C, connecting to an Azure PostgreSQL 

This is a small sample to show an approach of wrapping the C++ Azure SDK and using it in a C application. 



## Notes:

Dependancies:
`sudo apt install -y git gcc g++ cmake make libcurl4-openssl-dev libpq-dev libxml2-dev`


Make a build dir, run cmake, and build:
```
mkdir build && cd build
cmake ..
cmake --build .
./app/SampleApp
```