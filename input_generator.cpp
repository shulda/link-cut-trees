#include <bits/stdc++.h>

#define REP(i, n) for(int i = 0; i < n; i++)
#define INF (1<<30)
#define SIZE 1234567



using namespace std;

// Will serve as a custom set because in randomly selecting a vertex,
// we need to be able to locate the k-th element
// It is implemented as a treap with core operations split and join

struct Node{
	int p;
	int value;
	int size;
	Node *left, *right;

	Node(int v){
		p = rand();
		value = v;
		size = 1;
		left = right = NULL;
	}

};

int sz(Node *n){
	return n ? n->size : 0;
}

// Updates size after changing the shape of the tree
void update(Node *n){
	if(!n)return;
	n->size = 1+sz(n->left)+sz(n->right);
}

Node * join(Node *a, Node *b){
	if(!a)return b;
	if(!b)return a;
	Node * ret;
	if(a->p < b->p){
		ret = a;
		a->right = join(a->right, b);
	} else{
		ret = b;
		b->left = join(a, b->left);
	}
	update(ret);
	return ret;
}

pair<Node*, Node*> split(Node *n, int l_size){ //l_size - velikost levé části
	Node *left, *right;
	if(!n)return {NULL, NULL};
	if(sz(n->left) >= l_size){
		right = n;
		auto tmp = split(n->left, l_size);
		left = tmp.first;
		right->left = tmp.second;
	} else{
		left = n;
		auto tmp = split(n->right, l_size - n->size + sz(n->right));
		right = tmp.second;
		left->right = tmp.first;
	}
	update(left);
	update(right);
	return {left, right};
}

// Find `pos`-th element of tree rooted in root

Node * find(Node *root, int pos){ 
	if(sz(root->left) > pos){
		return find(root->left, pos);
	} else if(sz(root->left) == pos){
		return root;
	} else{
		return find(root->right, pos - sz(root->left) - 1);
	}
}

Node * add(Node *root, int val){
	return join(root, new Node(val));
}

Node * rem(Node *root, int pos){
	auto tmp = split(root, pos);
	auto tmp2 = split(tmp.second, 1);
	delete tmp2.first;
	return join(tmp.first, tmp2.second);
}


/* THE GENERATOR STARTS HERE */

int rnd(int a, int b){
	return a + (rand() % (b-a+1));
}


int p[SIZE]; // parents
set<int> e[SIZE]; // edges
int n;

Node * no_parent = NULL;
Node * with_parent = NULL;



// https://en.wikipedia.org/wiki/Reservoir_sampling
int random_neighbour(int a);

int _cnt, _chosen;
void _step(int x){
	if(0 == rnd(0, _cnt))_chosen=x;
	_cnt++;
}
void _dfs(int x, int p=-1){
	_step(x);
	for(auto i : e[x])if(i!=x && i != p){
		_dfs(i, x);
	}
}
int random_neighbour(int a){
	_cnt=0;
	_dfs(a);
	return _chosen;
}



void add_parent(int a, int x, int node_pos = -1){
	p[a] = x;
	e[a].insert(x);
	e[x].insert(a);
	with_parent = add(with_parent, a);
	if(node_pos > -1)no_parent = rem(no_parent, node_pos);
}


void remove_parent(int a, int node_pos){
	int b = p[a];
	p[a] = -1;
	e[a].erase(b);
	e[b].erase(a);
	no_parent = add(no_parent, a);
	with_parent = rem(with_parent, node_pos);
}

// Probability that a vertex will have no parent (exception for vertex 0, it never has a parent)
double forest_coeff = 0.0003; 

// parent[i] = rnd(i - tree_width_coeff, i-1) <- when generating starting forest
int tree_width_coeff = 10; 

// Generate also the `a x y` and `r x y` queries -- as they are generated randomly, the forest becomes pseudo-ballanced very fast
bool alter_structure = true;

int main(int argc, char **argv){

	vector<string> args(argv, argv+argc);
	if(args.size() < 3){
		printf("Usage: input_generator number_of_vertices number_of_queries [forest_coeff=0.0003 [tree_width_coeff=10 [alter_structure=1]]]\n");
		return 8;
	}

	n = stoi(args[1]);
	int m = stoi(args[2]);
	if(args.size() >= 4) forest_coeff = stod(args[3]);
	if(args.size() >= 5) tree_width_coeff = stoi(args[4]);
	if(args.size() >= 6) alter_structure = (bool)stoi(args[5]);

	srand(time(NULL));

	printf("%d %d\n", n, m);

	REP(i, n){
		p[i] = -1;
		// The vertex value ranges are hardcoded, as they do not make much difference
		printf("%d ", rnd(-10000, 10000));
	}
	printf("\n");

	REP(i, n){
		if(i == 0){
			printf("0 ");
			continue;
		}
		if(rand() < (int)(forest_coeff*RAND_MAX)){
			no_parent = add(no_parent, i);
			printf("0 ");
		}else{
			// for coeff == n generates trees with logarithmicall expected depth, for coeff == 1 generates a path
			int x = rnd(max(0, i-tree_width_coeff), i-1); 
			printf("%d ", x+1);
			add_parent(i, x);
		}
	}
	printf("\n");

	int type, a, b, d, tmp;

	REP(i, m){
		type = rnd(alter_structure ? 0 : 2, 5);

		switch(type){
			case 0: // add
				if(sz(no_parent) == 0){
					i--;
					break;
				}
				tmp = rnd(0, sz(no_parent)-1);
				a = find(no_parent, tmp)->value; // random vertex without a parent
				b = rnd(0, a-1);
				printf("a %d %d\n", a+1, b+1);
				add_parent(a, b, tmp);
				break;
			case 1: // remove
				if(sz(with_parent) == 0){
					i--;
					break;
				}
				tmp = rnd(0, sz(with_parent)-1);
				a = find(with_parent, tmp)->value; // random vertex with a parent
				b = p[a];
				printf("r %d %d\n", a+1, b+1);
				remove_parent(a, tmp);
				break;
			case 2: // set cost
				printf("s %d %d\n", rnd(1, n), rnd(-100000, 100000));
				break;
			case 3: // check
				printf("c %d %d\n", rnd(1, n), rnd(1, n));
				break;
			case 4: // path_min
				a = rnd(0, n-1);
				b = random_neighbour(a);
				printf("m %d %d\n", a+1, b+1);
				break;
			case 5: // update
				a = rnd(0, n-1);
				b = random_neighbour(a);
				d = rnd(-1000, 1000);
				printf("u %d %d %d\n", a+1, b+1, d);
				break;
		}
	}

	return 0;

}