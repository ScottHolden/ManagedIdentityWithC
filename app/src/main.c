#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <msi_auth_lib.h>

// Returns a copy of an enviroment variable if found
char* getenvdup(const char* name) {
    if (name == NULL || strcmp(name, "") == 0) {
        return NULL;
    }

    char* value = getenv(name);

    if (value == NULL || strcmp(value, "") == 0) {
        fprintf(stderr, "Required env var '%s' not found or empty\n", name);
        return NULL;
    }

    return strdup(value);
}

// Retrieves an Entra ID token from our library scoped to Azure PostgreSQL Flex
char* getTokenPassword() {
    const char* tokenScope = "https://ossrdbms-aad.database.windows.net/.default";

    // Get the token to use as the password
    char* token = NULL;
    char* errors = NULL;

    printf("Requesting token...\n");

    int result = getEntraIDToken(tokenScope, &token, &errors);

    // Check for errors
    if (result != 0 || errors != NULL) {
        fprintf(stderr, "Error getting token: %s\n", errors);
        free(token);
        free(errors);
        return NULL;
    }

    char* tokenCopy = strdup(token);
    free(token);
    free(errors);

    printf("Token recieved\n");
    return token;
}

// Formats required values into a libpq connection string
char* buildConnectionString(const char* dbHostname, const char* dbName, const int dbPort, const char* dbUsername, const char* dbPassword) {
    const char* connectionStringFormat = "host=%s dbname=%s port=%d user=%s password=%s";

    int requiredSize = snprintf(NULL, 0, connectionStringFormat, dbHostname, dbName, dbPort, dbUsername, dbPassword) + 1; // +1 for null terminator

    char *connectionString = (char *)malloc(requiredSize * sizeof(char));
    if (connectionString == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    snprintf(connectionString, requiredSize, connectionStringFormat, dbHostname, dbName, dbPort, dbUsername, dbPassword);

    return connectionString;
}

// Entry point
int main() {
    const char* dbHostname = getenvdup("DB_HOSTNAME");
    const char* dbName = getenvdup("DB_NAME");
    const char* dbUsername = getenvdup("DB_USERNAME");
    const int   dbPort = 5432;
    
    // Null checks
    if (dbHostname == NULL || dbName == NULL || dbUsername == NULL) {
        return 1;
    }

    // Get the password
    const char* dbPassword = getTokenPassword();

    if (dbPassword == NULL) {
        return 1;
    }
    
    // Build a connection string
    const char* connectionString = buildConnectionString(dbHostname, dbName, dbPort, dbUsername, dbPassword);
    
    if (dbPassword == NULL) {
        return 1;
    }
    printf("Built connection string with an EntraID token!\n");

    // printf("Debug connection string: %s\n", connectionString);

    PGconn* connection = PQconnectdb(connectionString);

    if (PQstatus(connection) == CONNECTION_OK) {
        printf("Connected successfully!\n");
    } else {
        printf("Connection failed: %s\n", PQerrorMessage(connection));
    }

    PQfinish(connection);

    return 0;
}