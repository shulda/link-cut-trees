#include <bits/stdc++.h>

#define REP(i, n) for(int i = 0; i < n; i++)
#define INF (1<<30)

using namespace std;



// top-down SPLAY podle http://digital.cs.usu.edu/~allan/DS/Notes/Ch22.pdf

int id_cnt = 0; // Each Node has a unique ID (for debugging purpouses)

struct Node{
    Node *left, *right, *parent;
    Node *thin_parent;
    bool rotated;
    int value;
    int size;
    int subtree_min;
    int lazy_add;
    int id;


    Node(int v = 0): left(0), right(0), parent(0), thin_parent(0), rotated(false), value(v), size(1), subtree_min(v), lazy_add(0){
    	id = id_cnt++;
    }
};


bool is_left_child(Node *n){
	assert(n->parent);
	return n->parent->left == n;
}

void set_parent(Node *n, Node *p){
	if(!n)return;
	n->parent = p;
}

// Propagates lazy flags
void touch(Node *n){
	if(!n)return;
	if(n->rotated){
		swap(n->left, n->right);
		if(n->left)n->left->rotated = !n->left->rotated;
		if(n->right)n->right->rotated = !n->right->rotated;
		n->rotated = false;
	}
	if(n->lazy_add != 0){
		int a = n->lazy_add;
		if(n->left){
			n->left->lazy_add+=a;
			n->left->value+=a;
			n->left->subtree_min+=a;
		}
		if(n->right){
			n->right->lazy_add+=a;
			n->right->value+=a;
			n->right->subtree_min+=a;
		}
		n->lazy_add=0;
	}
}

int size(Node *n){
	return n ? n->size : 0;
}

int subtree_min(Node *n){
	return n ? n->subtree_min : INF;
}

// Updates values dependent on children
void soak(Node *n){
	n->size = 1 + size(n->left) + size(n->right);
	n->subtree_min = min(n->value, min(subtree_min(n->left), subtree_min(n->right)));
}

/**
* Splays n to be the root of its tree
* Top-down splaying
*/
Node * splay(Node *n){
	assert(n);

	stack<Node*> s;
	Node *l_root = new Node;
	Node *r_root = new Node;

	// l collects Nodes smaller than the one we're looking for, r the larger ones. To cope with l == NULL,
	// they are initialized to some dummy nodes that we get rid of at the end
	Node *l = l_root, *r = r_root;

	Node *x = n;

	while(x->parent){
		s.push(x);
		x = x->parent;
	}

	Node *thin_parent = x->thin_parent;
	x->thin_parent = NULL;

	// x is now the root

	/**
	* 	WAY DOWN
	*      x
	*     /
	*    y
	*     \
	*      z
	*/
	Node *y, *z;
	while(!s.empty()){
		y = s.top();
		s.pop();
		touch(x);
		touch(y);

		if(s.empty()){ // There was only one vertex remaining
			if(is_left_child(y)){
				r->left = x;
				set_parent(x, r);
				r = x;
			} else{
				l->right = x;
				set_parent(x, l);
				l = x;
			}
			x = y;
			x->parent=NULL;
			break;
		}

		z = s.top();
		s.pop();		

		// Probably not always needed and could be optimized, but I kept this for simplicity (and also better safe than sorry)
		touch(z);

		// boredom
		if(is_left_child(y) && is_left_child(z)){
			Node *tmp = y->right;
			x->left = tmp;
			set_parent(tmp, x);
			y->left = NULL;
			y->right = x;
			set_parent(x, y);
			r->left = y;
			set_parent(y, r);
			soak(x);
			r = y;
			x = z;
		} else if(!is_left_child(y) && !is_left_child(z)){
			Node *tmp = y->left;
			x->right = tmp;
			set_parent(tmp, x);
			y->right = NULL;
			y->left = x;
			set_parent(x, y);
			l->right = y;
			set_parent(y, l);
			soak(x);
			l = y;
			x = z;
		} else if(is_left_child(y) && !is_left_child(z)){
			s.push(z);
			r->left = x;
			set_parent(x, r);
			x->left = NULL;
			r = x;
			x = y;
		} else /*if(!is_left_child(y) && is_left_child(z))*/{
			s.push(z);
			l->right = x;
			set_parent(x, l);
			x->right = NULL;
			l = x;
			x = y;
		}
		x->parent=NULL;
	}


	touch(x);
	/**
	*   x
	*  / \
	* A   B
	*
	*/
	Node *A = x->left, *B = x->right;

	l->right = A;
	set_parent(A, l);
	while(l){
		soak(l);
		l = l->parent;
	}

	r->left = B;
	set_parent(B, r);
	while(r){
		soak(r);
		r = r->parent;
	}

	x->left = l_root->right;
	set_parent(x->left, x);
	x->right = r_root->left;
	set_parent(x->right, x);
	soak(x);


	x->thin_parent = thin_parent;

	// Does it work?
	assert(x == n);

	delete l_root;
	delete r_root;

	return x;
}

/**
* Joins splay trees containing vertices l and r, all vertices in the l-tree will be smaller than all vertices in the r-tree.
* l and r do not have to be roots
*/
Node * join(Node *l, Node *r){
	if(!l)return r;
	if(!r)return l;
	while(l->parent)l = l->parent;
	touch(l);
	while(l->right){
		l = l->right;
		touch(l);
	}
	splay(l);
	splay(r);
	l->right = r;
	set_parent(r, l);
	soak(l);
	return l;
}


// PATH REPREZENTATION, as taught at the 2016 course "Data Structures II" at the Faculty of Mathematics and Physics, Charles University

Node * prev(Node *n){
	splay(n);
	if(!n->left)return 0;
	n=n->left;
	touch(n);
	while(n->right){
		n=n->right;
		touch(n);
	}
	return splay(n);
}
Node * next(Node *n){
	splay(n);
	if(!n->right)return 0;
	n=n->right;
	touch(n);
	while(n->left){
		n=n->left;
		touch(n);
	}
	return splay(n);
}
Node * first(Node *n){
	assert(n);
	while(n->parent)n=n->parent;
	touch(n);
	while(n->left){
		n=n->left;
		touch(n);
	}
	return splay(n);
}
Node * last(Node *n){
	assert(n);
	while(n->parent)n=n->parent;
	touch(n);
	while(n->right){
		n=n->right;
		touch(n);
	}
	return splay(n);
}

/**
* Splits thick path containing n into two parts, the second part starts with n
*/
pair<Node*, Node*> path_cut_before(Node *n){
	splay(n);
	Node *l = n->left;
	set_parent(l, 0);
	n->left=0;
	soak(n);
	return make_pair(l, n);
}

/**
* Splits thick path containing n into two parts, the first part ends with n
*/
pair<Node*, Node*> path_cut(Node *n){
	splay(n);
	Node *r = n->right;
	set_parent(r, 0);
	r->thin_parent = n->thin_parent;
	n->thin_parent = 0;
	n->right=0;
	soak(n);
	return make_pair(n, r);
}

/**
* Joins thick paths containing vertices a and b. Will be called when adding edge a->b (when there was no edge going from a before)
*/
void path_link(Node *a, Node *b){
	splay(a); splay(b);
	Node *thin_parent = b->thin_parent;
	b->thin_parent = NULL;
	Node *tmp = join(a, b);

	tmp->thin_parent = thin_parent;
}

/**
* Reverses the thick path from n to its end (i.e. upwards)
*/
void reverse(Node *n){
	auto tmp = path_cut_before(n);
	tmp.second->rotated=!tmp.second->rotated;
	join(tmp.first, tmp.second);
}

int cost(Node *n){
	return splay(n)->value;
}

/**
* Minimum of thick path from n (including) to its end
*/
int path_min(Node *n){
	splay(n);
	return min(n->value, subtree_min(n->right));
}

void set_cost(Node *n, int cost){
	splay(n);
	n->value = cost;
	soak(n);
}

/** 
* Increases values of all vertices on the thick path from n (including) to its end by delta
*/
void path_update(Node *n, int delta){
	auto tmp = path_cut_before(n);
	tmp.second->value+=delta;
	tmp.second->subtree_min+=delta;
	tmp.second->lazy_add+=delta;
	join(tmp.first, tmp.second);
}



// LINK-CUT TREES

Node *parent(Node *n){
	splay(n);
	Node *x = next(n);
	if(x)return splay(x);
	return n->thin_parent ? splay(n->thin_parent) : 0;
}

Node *root(Node *n){
	n = last(n);
	while(n->thin_parent){
		n = n->thin_parent;
		n = last(n);
	}
	return splay(n);
}

/**
* Removes the edge between n and parent(n)
*/
void cut(Node *n){
	if(next(n)){
		path_cut(n);
	} else{
		splay(n);
		n->thin_parent=0;
	}
}

/**
* Creates the edge u->v, u must be a root (i.e. have no outgoing edge)
*/
void link(Node *u, Node *v){
	splay(u);
	u->thin_parent = v;
}

void expose(Node *n){
	auto tmp = path_cut_before(n);
	if(tmp.first)tmp.first->thin_parent=n;
	splay(n);
	Node *p;
	while(n->thin_parent){
		p=n->thin_parent;
		n->thin_parent=0;
		tmp = path_cut_before(p);
		if(tmp.first)tmp.first->thin_parent=p;
		path_link(n, p);
		n=splay(p);
	}
}

void evert(Node *n){
	expose(n);
	splay(n);
	n->rotated = !n->rotated;
}

/**
* Minimum on path between a and b
*/
int tree_min(Node *a, Node *b){
	evert(a);
	expose(b);
	return path_min(b);
}

/**
* Increases the values of vertices on the path between a and b by d
*/
void update(Node *a, Node *b, int d){
	evert(a);
	expose(b);
	path_update(b, d);
}


/* DEBUGGING FUNCTIONS */

/**
* Prints the splay tree rooted in root
*/
int print_tree(Node * root){
	if(!root){
		printf("()");
		return 0;
	}
	touch(root);
	int mx = 0;
	printf("(key: %d, size: %d, parent:%d, left:\n", root->value, root->size, root->parent ? root->parent->value : -1);
	mx = max(mx, print_tree(root->left));
	printf(",right:\n");
	mx = max(mx, print_tree(root->right));
	printf(")\n");
	return mx+1;
}

int val(Node *n){
	if(n)return n->value;
	return -1;
}

int id(Node *n){
	if(n)return n->id;
	return -1;
}


void print_node(Node *n){
	printf("id: %d, value: %d, rotated:%d, subt_min: %d, lazy_add: %d, left: %d, right: %d, parent: %d, thin_parent: %d\n",
		n->id,
		n->value,
		n->rotated,
		n->subtree_min,
		n->lazy_add,
		id(n->left),
		id(n->right),
		id(n->parent),
		id(n->thin_parent));
}

Node * nodes[1234567];
int n;
char buff[256];

int main(){
	int m;
	scanf("%d %d", &n, &m);

	nodes[0] = NULL;

	int a, b, d;

	REP(i, n){
		scanf("%d", &a);
		nodes[i+1] = new Node(a);
	}

	REP(i, n){
		scanf("%d", &a);
		nodes[i+1]->thin_parent = nodes[a];
	}

	REP(i, m){
		scanf("%s %d %d", buff, &a, &b);
		if(buff[0]=='c'){ // connected
			printf(root(nodes[a])==root(nodes[b]) ? "YES\n" : "NO\n");
		}
		if(buff[0]=='r'){ // cut
			evert(nodes[a]);
			assert(parent(nodes[b]) == nodes[a]);
			cut(nodes[b]);
		}
		if(buff[0]=='a'){ // link
			evert(nodes[a]);
			assert(root(nodes[b]) != root(nodes[a]));
			link(nodes[a], nodes[b]);
		}
		if(buff[0]=='s'){ // set_cost
			set_cost(nodes[a], b);
		}
		if(buff[0]=='m'){ // minimum
			assert(root(nodes[b]) == root(nodes[a]));
			printf("%d\n", tree_min(nodes[a], nodes[b]));
		}
		if(buff[0]=='u'){ // update
			scanf("%d", &d);
			assert(root(nodes[b]) == root(nodes[a]));
			update(nodes[a], nodes[b], d);
		}
	}


	return 0;

}