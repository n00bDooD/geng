#include "object.h"

#include <stdio.h>

int main(int argc, char** argv)
{
	object* root = create_root(1000000);

	printf("Root and pool allocated\n");
	
	int nobjs = 5000;
	object* pobj = root;
	for(int co = 0; co < nobjs; co++){
		pobj = create_object(pobj);
	}

	printf("Object structure created\n");

	delete_object(root->children[0]);

	printf("Object structure deleted\n");

	delete_root(root);

	printf("Root and pool freed\n");

	return 0;
}
