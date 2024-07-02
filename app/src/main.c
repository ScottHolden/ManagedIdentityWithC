#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <msi_auth_lib.h>

int main() {
    const char* dbHostname = "hostname.postgres.database.azure.com";
    const char* dbName = "postgres";
    const int   dbPort = 5432;
    const char* dbUsername = "username@tenant.com";

    const char* tokenScope = "https://ossrdbms-aad.database.windows.net/.default";

    // Get the token to use as the password
    char *token = NULL;
    char *errors = NULL;
    printf("Requesting token...\n");
    int result = getEntraIDToken(tokenScope, &token, &errors);

    // Check for errors
    if (result != 0 || errors != NULL) {
        fprintf(stderr, "Error getting token: %s\n", errors);
        free(errors);
        return 1;
    }
    printf("Token recieved\n");

    // Format our connection string
    const char* connectionStringFormat = "host=%s dbname=%s port=%d user=%s password=%s";
    int requiredSize = snprintf(NULL, 0, connectionStringFormat, dbHostname, dbName, dbPort, dbUsername, token) + 1; // +1 for null terminator
    char *connectionString = (char *)malloc(requiredSize * sizeof(char));
    if (connectionString == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    snprintf(connectionString, requiredSize, connectionStringFormat, dbHostname, dbName, dbPort, dbUsername, token);
    printf("Built connection string with an EntraID token!\n");

    // printf("Debug connection string: %s\n", connectionString);

    PGconn* connection = PQconnectdb(connectionString);

    if (PQstatus(connection) == CONNECTION_OK) {
        printf("Connected successfully!\n");
    } else {
        printf("Connection failed: %s\n", PQerrorMessage(connection));
    }

    PQfinish(connection);

    free(token);
    free(connectionString);

    return 0;
}