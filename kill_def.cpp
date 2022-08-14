#include <iostream>
#include <vector>
using namespace std;

void find_all(string str, string key, int dx, vector<int> &all) {
	if(str.find(key, dx) == string::npos) return;
	if(str.find(key, dx) != string::npos) {
		all.push_back(str.find(key, dx));
		find_all(str, key, str.find(key, dx)+1, all);
	}
}

void get_info(vector<string> line, vector<int> &x, vector<int> &y, vector<string> &part) {
	x.clear(); y.clear(); part.clear();
	vector<string> dd = vector<string> ();
	dd.push_back("`ifndef");
	dd.push_back("`ifdef"); dd.push_back("`elsif");
	dd.push_back("`else"); dd.push_back("`endif");

	for(int i = 0; i < line.size(); i++) {
		for(int k = 0; k < dd.size(); k++) {
			if(line[i].find(dd[k]) != string::npos) {
				vector<int> all = vector<int> ();
				find_all(line[i], dd[k], 0, all);
				for(int j = 0; j < all.size(); j++) {
					x.push_back(i);
					y.push_back(all[j]);
					part.push_back(dd[k]);
				}
			}
		}
	}
	return;
}

void get_def(vector<string> line, vector<int> x, vector<int> y, vector<string> part, vector<string> &def) {
	for(int i = 0; i < part.size(); i++)
		def.push_back("xxx");
	for(int i = 0; i < part.size(); i++) {
		if(part[i] == "`ifdef" || part[i] == "`elsif" || part[i] == "`ifndef") {
			string tmp;
			int j = y[i];
			while(tmp != " " && tmp != "\t") tmp.assign(line[x[i]], j++, 1);
			while(tmp == " " || tmp == "\t") tmp.assign(line[x[i]], j++, 1);
			int start = j-1;
			while((tmp != " " && tmp != "\t") && j <= line[x[i]].size()) tmp.assign(line[x[i]], j++, 1);
			tmp.assign(line[x[i]], start, j-start-1); 
//			cout << tmp << endl;
			def[i] = tmp;
		}
		if(part[i] == "`else")
			def[i] = "ooo";
	}
}

void kill_others(vector<int> cc, vector<int> &x, vector<string> &part) {
//	for(int i = 0; i < part.size(); i++)
//		cout << "kill_others_before -> " << part[i] << endl;

	vector<int> tmp_int = vector<int> ();
	vector<string> tmp_str = vector<string> ();
	for(int i = 0; i < x.size(); i++) {
		bool ok = 0;
		for(int j = 0; j < cc.size(); j++) {
			if(i == cc[j]) {
				ok = 1;
				break;
			}
		}
		if(!ok) {
			tmp_int.push_back(x[i]);
			tmp_str.push_back(part[i]);
		}
	}
	x.clear();
	part.clear();
	for(int i = 0; i < tmp_int.size(); i++) {
		x.push_back(tmp_int[i]);
		part.push_back(tmp_str[i]);
	}
	
//	for(int i = 0; i < part.size(); i++)
//		cout << "kill_others_after -> " << part[i] << endl;

}

void get_first(vector<int> &x, vector<int> &y, vector<string> &part, vector<string> &def) {
	int cnt = 0;
	vector<int> cc = vector<int> ();
	for(int i = 0; i < part.size(); i++) {
		if(part[i] == "`ifdef" || part[i] == "`ifndef") cnt++;
		if(cnt != 1) cc.push_back(i);
		if(part[i] == "`endif") cnt--;
	}
	kill_others(cc, x, part);
	kill_others(cc, y, def);
}

void kill_part(vector<string> &ff, int start_x, int start_y, int end_x, int end_y) {
	for(int i = 0; i < ff.size(); i++) {
		if(i > start_x && i < end_x)
			for(int j = 0; j < ff[i].size(); j++)
				ff[i][j] = ' ';
		if(i == start_x && i != end_x)
			for(int j = 0; j < ff[i].size(); j++)
				if(j >= start_y)
					ff[i][j] = ' ';
		if(i == end_x && i != start_x)
			for(int j = 0; j < ff[i].size(); j++)
				if(j <= end_y)
					ff[i][j] = ' ';
		if(i == start_x && i == end_x)
			for(int j = 0; j < ff[i].size(); j++)
				if(j >= start_y && j <= end_y)
					ff[i][j] = ' ';
			
	}
}

void get_part(vector<string> &ff, vector<int> x, vector<int> y, vector<string> part, vector<string> def, vector<string> &v_def) {

	bool ok = 0;
	bool ok_else = 0;
	int t_pass;
	string pass;
	
	
	for(int i = 0; i < def.size(); i++) {
//		cout << "def -> " << def[i] << endl; 
		if(part[i] == "`ifndef") {
			bool ok_ifndef = 0;
			for(int k = 0; k < v_def.size(); k++) {
				if(def[i] == v_def[k]) {
					ok_ifndef = 1;
					break;
				}
			}
			if(!ok_ifndef) {
				pass = def[i];
				t_pass = i;
				ok = 1;
			}
		} else {
			for(int j = 0; j < v_def.size(); j++) {
//			cout << "v_def -> " << v_def[j] << endl;
				if(def[i] == v_def[j]) {
					pass = def[i];
					t_pass = i;
					ok = 1;
					break;
				}
			}
		}
		if(ok) break;
	}
	if(!ok) {
		for(int i = 0; i < def.size(); i++) {
			if(def[i] == "ooo") {
				t_pass = i;
				ok_else = 1;
				ok = 1;
				break;
			}
		}
	}
//	cout << "pass -> " << pass << endl;
	if(!ok) return;
	kill_part(ff, x[t_pass+1], y[t_pass+1], x[def.size()-1], y[def.size()-1]+5);
	if(ok_else) kill_part(ff, x[0], y[0], x[t_pass], y[t_pass]+4);
	else kill_part(ff, x[0], y[0], x[t_pass], ff[x[t_pass]].find(pass)+pass.size()-1);
}

void get_all(vector<string> &ff, vector<int> &x, vector<int> &y, vector<string> &part, vector<string> &def, vector<string> &v_def) {
	x.clear(); y.clear(); part.clear(); def.clear();

	get_info(ff, x, y, part);
	get_def(ff, x, y, part, def);
	get_first(x, y, part, def);
	
//	for(int i = 0; i < x.size(); i++)
//		cout << "get_first_after -> " << x[i] << " " << y[i] << " " << def[i] << " " << part[i] << endl;

	vector<int> group_part = vector<int> ();
	int left = 0; int right = 0;
	for(int i = 0; i < part.size(); i++) {
		if(part[i] == "`ifdef" || part[i] == "`ifndef") left++;
		if(part[i] == "`endif") right++;
//		cout << l << " " << r << " " << part[i] << endl;
		group_part.push_back(left);		
	}
	if(left != right) return;
	for(int i = 0; i < left; i++) {
		vector<int> x_g = vector<int> ();	
		vector<int> y_g = vector<int> ();
		vector<string> part_g = vector<string> ();
		vector<string> def_g = vector<string> ();
		for(int j = 0; j < group_part.size(); j++) {
			if(group_part[j] == i+1) {
				x_g.push_back(x[j]);
				y_g.push_back(y[j]);
				part_g.push_back(part[j]);
				def_g.push_back(def[j]);
			}
		}
		get_part(ff, x_g, y_g, part_g, def_g, v_def);
	}
}

bool kill_def (vector<string> &ff, vector<string> &v_def) {
	vector<int> x = vector<int> ();
	vector<int> y = vector<int> ();
	vector<string> part = vector<string> ();
	vector<string> def = vector<string> ();
	get_all(ff, x, y, part, def, v_def);
	int pre = part.size();

//	int cnt = 0;
	int cur = 999;
	while(cur != pre) {
		get_all(ff, x, y, part, def, v_def);
		pre = cur;
		cur = part.size();
	
//		cout << cnt << endl;
//		cnt++;
	}
	
	if(pre == 0) return 1;
	return 0;
}


void want_def(vector<string> ff, vector<string> &v_def) {
	vector<string> t_ff = vector<string> ();
	for(int i = 0; i < ff.size(); i++)
		t_ff.push_back(ff[i]);
	
	bool isdef = 0;
	for(int i = 0; i < ff.size(); i++)
		if(ff[i].find("`endif") != string::npos) {
			isdef = 1;
			break;
		}
	
	if(isdef) {

		vector<int> x = vector<int> ();
		vector<int> y = vector<int> ();
		vector<string> part = vector<string> ();
		vector<string> def = vector<string> ();

//	for(int i = 0; i < ff.size(); i++)
//		cout << "debug -> " << ff[i] << endl;

		get_info(ff, x, y, part);
//	for(int i = 0; i < x.size(); i++)
//		cout << "debug -> " << x[i] << " " << y[i] << " " << part[i] << endl;
//	get_def(ff, x, y, part, def);
		for(int i = 0; i < part.size(); i++)
			def.push_back("xxx");
		get_first(x, y, part, def);

		vector<int> x_g = vector<int> ();	
		vector<int> y_g = vector<int> ();
		vector<string> part_g = vector<string> ();
		for(int i = 0; i < part.size(); i++) {
			if(part[i] == "`ifdef" || part[i] == "`ifndef" || part[i] == "`endif") {
				x_g.push_back(x[i]);
				y_g.push_back(y[i]);
				part_g.push_back(part[i]);
			}
		}
		for(int i = 0; i < part_g.size()-1; i++)
			kill_part(t_ff, x_g[i], y_g[i], x_g[i+1], y_g[i+1]+5);
	}

	for(int i = 0; i < t_ff.size(); i++) {
		if(t_ff[i].find("`define") != string::npos) {
			string tmp;
			int j = t_ff[i].find("`define");
			while(tmp != " " && tmp != "\t") tmp.assign(t_ff[i], j++, 1);
			while(tmp == " " || tmp == "\t") tmp.assign(t_ff[i], j++, 1);
			int start = j-1;
			while((tmp != " " && tmp != "\t") && j <= t_ff[i].size()) tmp.assign(t_ff[i], j++, 1);
			tmp.assign(t_ff[i], start, j-start-1); 
//			cout << tmp << endl;
			
			bool ok = 0;
			for(int k = 0; k < v_def.size(); k++)
				if(v_def[k] == tmp) {
					ok = 1;
					break;
				}
			if(!ok) v_def.push_back(tmp);
		}
	}
}

void def_out(vector<string> &ff, vector<string> &v_def) {	
	
	int cur = 999;
	while(1) {
		want_def(ff, v_def);
		if(cur == v_def.size()) break;
		kill_def(ff, v_def);
		cur = v_def.size();
	}
}




int main(int argc, char *argv[]) {
	cin.tie(0);
	ios::sync_with_stdio(0);

	vector<string> ff = vector<string> ();
	ff.push_back("uhi `ifndef	ABCDE	F		`ifdef	C uygu `endif");
	ff.push_back("	I am happy.");
	ff.push_back("	`elsif BC mlmn");
	ff.push_back("	You are stupid.");
	ff.push_back("`elsif B");
	ff.push_back("	You are stupid.");
	ff.push_back("`elsejk   ");
	ff.push_back("	Do you have any ideal");
	ff.push_back("`endif");
	ff.push_back("ilunu`ifdef	ABCDE	F		`ifdef	C uygu `endif");
	ff.push_back("	I am happy.");
	ff.push_back("	`define C");
	ff.push_back("`endif jio");
	ff.push_back("	`define ABCDE  k");
//	ff.push_back("	`define		jogood	oko");
	for(int i = 0; i < ff.size(); i++)
		cout << ff[i] << endl;

	vector<string> v_def = vector<string> ();
//	v_def.push_back("ABCDE");
//	v_def.push_back("C");
	v_def.push_back("B");


	def_out(ff, v_def);
	
	
	cout << ".........." << endl;
	cout << endl;
	for(int i = 0; i < ff.size(); i++)
		cout << ff[i] << endl;


/*
	kill_def(ff, v_def);
*/



	return 0;
}
