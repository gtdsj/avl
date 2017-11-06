#include <iostream>
#include "avl.h"
using namespace std;

static Comparable<long> TestVals[] = {
	16838,
	5758,
	10113,
	17515,
	31051,
	5627,
	23010,
	7419,
	16212,
	4086,
	2749,
	12767,
	9084,
	12060,
	32225,
	17543,
	25089,
	21183,
	25137,
}; //

int main()
{
	AvlTree<long> tree;
	for (int i = 0; i < sizeof(TestVals)/sizeof(TestVals[0]); ++i){
		tree.insert(&TestVals[i]);
	}
	
	tree.DumpTree();
	tree.remove(5758);
	tree.DumpTree();
	tree.remove(9084);
	tree.DumpTree();
	return 0;
}
