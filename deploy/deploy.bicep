@description('Location for all resources.')
param location string = resourceGroup().location

@description('A prefix to add to the start of all resource names. Note: A "unique" suffix will also be added')
param prefix string = 'cmsipg'

@description('The container image to run')
param containerAppImage string = 'ghcr.io/scottholden/managedidentitywithc:release'

@description('Tags to apply to all deployed resources')
param tags object = {}

param pgSkuTier string = 'Burstable'
param pgSkuName string = 'Standard_B1ms'
param pgVersion string = '16'
param pgStorageSizeGB int = 128
param pgStorageTier string = 'P10'

var databaseName = 'postgres'
var containerAppUsername = 'containerapp'

var uniqueNameFormat = '${prefix}-{0}-${uniqueString(resourceGroup().id, prefix)}'

resource postgres 'Microsoft.DBforPostgreSQL/flexibleServers@2023-12-01-preview' = {
  name: format(uniqueNameFormat, 'pg')
  location: location
  tags: tags
  sku: {
    name: pgSkuName
    tier: pgSkuTier
  }
  properties: {
    createMode: 'Default'
    version: pgVersion
    network: {
      publicNetworkAccess: 'Enabled'
    }
    storage: {
      storageSizeGB: pgStorageSizeGB
      autoGrow: 'Disabled'
      tier: pgStorageTier
    }
    backup: {
      geoRedundantBackup: 'Disabled'
      backupRetentionDays: 7
    }
    highAvailability: {
      mode: 'Disabled'
    }
    authConfig: {
      activeDirectoryAuth: 'Enabled'
      passwordAuth: 'Disabled'
      tenantId: tenant().tenantId
    }
  }
  resource allowAzureFirewallRule 'firewallRules@2023-12-01-preview' = {
    name: 'AllowAllAzureServicesAndResourcesWithinAzureIps'
    properties: {
      endIpAddress: '0.0.0.0'
      startIpAddress: '0.0.0.0'
    }
  }
}

module containerAppAdministrator 'pgAdmin.bicep' = {
  name: '${deployment().name}-pgAdmin'
  params: {
    objectId: containerApp.identity.principalId
    postgresName: postgres.name
    principalName: containerAppUsername
    principalType: 'ServicePrincipal'
  }
}

resource logAnalytics 'Microsoft.OperationalInsights/workspaces@2022-10-01' = {
  name: format(uniqueNameFormat, 'logs')
  location: location
  tags: tags
  properties: {
    retentionInDays: 30
    sku: {
      name: 'PerGB2018'
    }
  }
}

resource containerAppEnv 'Microsoft.App/managedEnvironments@2024-03-01' = {
  name: format(uniqueNameFormat, 'containerapp')
  location: location
  tags: tags
  properties: {
    appLogsConfiguration: {
      destination: 'log-analytics'
      logAnalyticsConfiguration: {
        customerId: logAnalytics.properties.customerId
        sharedKey: logAnalytics.listKeys().primarySharedKey
      }
    }
  }
}

resource containerApp 'Microsoft.App/containerApps@2024-03-01' = {
  name: prefix
  location: location
  tags: tags
  identity: {
    type: 'SystemAssigned'
  }
  properties: {
    managedEnvironmentId: containerAppEnv.id
    template: {
      containers: [
        {
          name: 'sampleapp'
          image: containerAppImage
          resources: {
            cpu: json('.25')
            memory: '.5Gi'
          }
          env: [
            {
              name: 'DB_HOSTNAME'
              value: postgres.properties.fullyQualifiedDomainName
            }
            {
              name: 'DB_NAME'
              value: databaseName
            }
            {
              name: 'DB_USERNAME'
              value: containerAppUsername
            }
          ]
        }
      ]
    }
  }
}
