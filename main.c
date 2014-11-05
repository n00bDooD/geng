#include "container.h"

#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv)
{
	container* cont = create_container(10000000);
	assert(cont != NULL);

	printf("Root and pool allocated\n");

	int ntrees = 1000;
	object* treepar = cont->object_root;
	for(int to = 0; to < ntrees; to++) {
		int nobjs = 500;
		object* pobj = treepar;
		for(int co = 0; co < nobjs; co++) {
			pobj = create_object(pobj);
		}
		treepar = treepar->children[0];
	}

	printf("Object structure created\n");

	delete_object(cont->object_root->children[0]);

	printf("Object structure deleted\n");

	delete_container(cont);

	printf("Root and pool freed\n");

	return 0;
}
