param postgresName string
param objectId string
param principalName string
@allowed(['User', 'ServicePrincipal'])
param principalType string

resource postgres 'Microsoft.DBforPostgreSQL/flexibleServers@2023-12-01-preview' existing = {
  name: postgresName
  resource adminstrator 'administrators@2023-12-01-preview' = {
    name: objectId
    properties: {
      principalType: principalType
      principalName: principalName
      tenantId: tenant().tenantId
    }
  }
}
