#include "object.h"

#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv)
{
	object* root = create_root(10000000);
	assert(root != NULL);

	printf("Root and pool allocated\n");
	
	int ntrees = 1000;
	object* treepar = root;
	for(int to = 0; to < ntrees; to++){
		int nobjs = 500;
		object* pobj = treepar;
		for(int co = 0; co < nobjs; co++){
			pobj = create_object(pobj);
		}
		treepar = treepar->children[0];
	}

	printf("Object structure created\n");

	delete_object(root->children[0]);

	printf("Object structure deleted\n");

	delete_root(root);

	printf("Root and pool freed\n");

	return 0;
}
