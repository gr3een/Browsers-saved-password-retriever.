#include"helpFile.h"
#include<iostream>
using namespace std;

void vaultInit()
{
	printf("Entered vault init \n");
	if (intialize)
	{
		return;
	}
	
	if (!(hVaultDll = LoadLibraryA("vaultcli.dll")))
	{
		return;
	}
	
	pVaultEnumerateItems = (VaultEnumerateItems)GetProcAddress(hVaultDll, "VaultEnumerateItems");
	pVaultEnumerateVaults = (VaultEnumerateVaults)GetProcAddress(hVaultDll, "VaultEnumerateVaults");
	pVaultFree = (VaultFree)GetProcAddress(hVaultDll, "VaultFree");
	pVaultOpenVault = (VaultOpenVault)GetProcAddress(hVaultDll, "VaultOpenVault");
	pVaultCloseVault = (VaultCloseVault)GetProcAddress(hVaultDll, "VaultCloseVault");
	pVaultGetItem = (PVAULTGETITEM)GetProcAddress(hVaultDll, "VaultGetItem");

	if (!pVaultEnumerateItems || !pVaultEnumerateVaults || !pVaultFree || !pVaultOpenVault || !pVaultCloseVault || !pVaultGetItem) {
		FreeLibrary(hVaultDll);
		return;
	}
	printf("Exit vault init\n");
}

void vaultFinal()
{
	printf("Entered vault Final \n");
	if (!intialize)
	{
		return;
	}
	if (hVaultDll)
	{
		FreeLibrary(hVaultDll);
	}

	intialize =FALSE;
	printf("Exit vault final \n");
}



void printPassword()

{
	printf("Entered print password\n");
	DWORD vaultsCounter, itemsCounter;
	LPGUID vaults;
	HVAULT hVault;
	PVOID items;
	PVAULT_ITEM vaultItems, pVaultItems;

	if (pVaultEnumerateVaults(NULL, &vaultsCounter, &vaults) != ERROR_SUCCESS)
	{
		return;
	}

	for (DWORD i = 0; i < vaultsCounter; i++)
	{
		if (pVaultOpenVault(&vaults[i], 0, &hVault) == ERROR_SUCCESS)
		{
			if (pVaultEnumerateItems(hVault, VAULT_ENUMERATE_ALL_ITEMS, &itemsCounter, &items) == ERROR_SUCCESS)
			{
				vaultItems = (PVAULT_ITEM)items;

				for (DWORD j = 0; j < itemsCounter; j++)
				{
					wcout << "URL: " << vaultItems[j].Resource->data.String << endl;
					wcout << "USERNAME: " << vaultItems[j].Identity->data.String << endl;

					pVaultItems = NULL;

					if (pVaultGetItem(hVault, &vaultItems[j].SchemaId, vaultItems[j].Resource, vaultItems[j].Identity, vaultItems[j].PackageSid, NULL, 0, &pVaultItems) == 0)
					{
						if (pVaultItems->Authenticator != NULL && pVaultItems->Authenticator->data.String != NULL)
						{
							wcout << "Password: " << pVaultItems->Authenticator->data.String << endl;
						}
						pVaultFree(pVaultItems);
					}
					printf("\n");
					
				}
				pVaultFree(items);
				printf("\n");
				
			}
			pVaultCloseVault(&hVault);
		}
		
	}
	if (vaults)
	{
		pVaultFree(vaults);
		vaults = NULL;
	}
	printf("Exit print password\n");
}
