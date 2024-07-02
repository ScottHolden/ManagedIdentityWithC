#ifndef MSI_AUTH_LIB_H
#define MSI_AUTH_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

int getEntraIDToken(const char * tokenScope, char** token, char** errors);

#ifdef __cplusplus
}
#endif

#endif