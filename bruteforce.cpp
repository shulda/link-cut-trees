#include <bits/stdc++.h>

#define REP(i, n) for(int i = 0; i < n; i++)
#define INF (1<<30)
#define SIZE 1234567


using namespace std;

set<int> e[SIZE];

int w[SIZE];
int n;


void add(int a, int b){
	e[a].insert(b);
	e[b].insert(a);
}
void remove(int a, int b){
	e[a].erase(b);
	e[b].erase(a);
}
void set_w(int a, int v){
	w[a] = v;
}
/**
* minimum on path a->b, returns pair<connected, minimum>
*/
pair<bool, int> minimum(int a, int b, int p = -1){
	if(a == b)return make_pair(true, w[b]);
	for(auto i : e[a])if(i != p){
		auto tmp = minimum(i, b, a);
		if(tmp.first){
			return make_pair(true, min(w[a], tmp.second));
		}
	}
	return make_pair(false, INF);
}

/**
* increase values of all vertices on path a->b by d.
*/
bool upd(int a, int b, int d, int p = -1){
	if(a == b){
		w[b]+=d;
		return true;
	}
	for(auto i : e[a])if(i != p){
		if(upd(i, b, d, a)){
			w[a]+=d;
			return true;
		}
	}
	return false;
}


int main(){
	int m;
	scanf("%d %d", &n, &m);

	char buff[26];

	int a, b, d;

	REP(i, n){
		scanf("%d", &a);
		w[i] = a;
	}

	REP(i, n){
		scanf("%d", &a);
		if(a > 0){
			add(i, a-1);
		}
	}

	REP(i, m){

		scanf("%s %d %d", buff, &a, &b);
		a--;b--;
		if(buff[0] == 'a'){
			add(a, b);
		}
		if(buff[0] == 'r'){
			remove(a, b);
		}
		if(buff[0] == 'c'){
			printf(minimum(a, b).first ? "YES\n" : "NO\n");
		}
		if(buff[0] == 's'){
			set_w(a, b+1);
		}
		if(buff[0] == 'm'){
			printf("%d\n", minimum(a, b).second);
		}
		if(buff[0] == 'u'){
			scanf("%d", &d);
			upd(a, b, d);
		}
	}

	return 0;

}