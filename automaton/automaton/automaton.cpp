// by Fiodar Prystauka (jV_)
//INSTRUCTIONS FOR USING AUTOMATON
// 1. Pick a timetable of a bus on https://arriva-reisinfo.fis.nl/
// 2. Select "Alle haltes tonen" ("Show all stops")
// 3. Download the file of timetable (do not pick "Translate to English" in Chrome, since it will somewhat break the timetable). Its name is Dienstregeltabellen.pdf
// 4. Paste the file in the same folder as automaton.cpp
// 4.1. Before doing the next step(!!!), remember the number of lines and columns of every table present in the file, starting from page 1 (numeration of pages as it is written in the file) 
// 5. Rename the file to Dienstregeltabellen.txt, thus changing the extension from .pdf to .txt. If any confirmation dialogs pop up, confirm your action.
// 6. Open the file. Find the first instance of "Lijn(" (without quotes). Remove EVERY line between the end of the file and the first instance of "Lijn(".
// 7. Find the last instance of "Pagina" (without quotes). Remove EVERY line between the end of the file and the last instance of "Pagina".
// Steps 6 and 7 are needed because null-symbols are present between the lines, and their existence in the file makes using the automaton impossible.
// 8. Save the file.
// 9. Open automaton.cpp. Whenever you see //change, you should make changes according to what you want to see in your output files and step 4.1.
//10. Run the program. In the folder with the file automaton.cpp, you will see numerous output files, like weekday.txt. These are your results.


#pragma warning(disable : 4996)
#pragma warning(disable : 6031)


#include <fstream>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

int main() {
	int a, w, wv, wb, sat,satv, sun;
	cout << "Initialistaion\n";
	_sleep(200);
	cout << "What is the number of stations on the route?\n";
	cin >> a;
	cout << "What is the number of times a bus traverses the route...\n";
	cout << "...on weekdays?\n";
	cin >> w;
	cout << "...on weekday holidays?\n";
	cin>> wv;
	cout << "...on weekday if alternative schedule is applied?\n";
	cin >> wb;
	cout << "...on Saturdays?\n";
	cin >> sat;
	cout << "...on Saturday holidays?\n";
	cin >> satv;
	cout << "on Sunday?\n";
	cin >> sun;
	cout << "What are the numbers of stations you are interested in?\n";
	cout << "Input all the stations. In order to end it, input ^\n";
	vector<int> stat;
	int temp;
	while (cin >> temp)
		stat.push_back(temp);
	cout << "Initialisation complete.\n";

	ifstream input("input_21_r.txt");
	int cnt = 0;
	bool f = 0;
	int i = 0, j = 0;
	int stage = 0;
	int flag = 0;


	ofstream o1("weekday.txt");
	vector<vector<int>> weekday(a, vector<int>(w)); 
	stage = 0, i = 0, j = 0, cnt = 0, flag=0;
	while (cnt < a * w) { 
		string s;
		input >> s;
		if (s.size() > 6)
			if (s[0] == '(' && s[6] == ')') {
				int ans = stoi(s.substr(1, 2)) * 60 + stoi(s.substr(4, 2));
				cnt++;
				if (ans < 180)
					ans += 1440;
				int temp = flag * 27 + stage * 28 + j;
				//cout << i << ' ' << temp << '(' << flag << ' ' << stage << ' ' << j << ')' << '\n';
				weekday[i][temp] = ans;
				j++;
				if (!flag && j == 27 || temp+1==w) { 
					j = 0;
					i++;
				}
				else if (flag && j == 28 || temp+1==w) { 
					j =0;
					i++;
				}
				if (i == a) {
					if (!flag)
						flag=1;
					else
						stage++;
					i = 0;
				}
			}
	}
	
	for (int j = 0; j < w; j++) {
		for (int i = 0; i < stat.size(); i++)
			o1 << weekday[stat[i]][j]<<' ';
		o1 << '\n';
	}
	o1.close();

	ofstream o2("weekdayv.txt");
	vector<vector<int>> weekdayv(a, vector<int>(wv)); 
	stage = 0, i = 0, j = 0, cnt = 0, flag = 0;
	while (cnt < a * wv) {
		string s;
		input >> s;
		if (s.size() > 6)
			if (s[0] == '(' && s[6] == ')') {
				int ans = stoi(s.substr(1, 2)) * 60 + stoi(s.substr(4, 2));
				cnt++;
				if (ans < 180)
					ans += 1440;
				int temp = flag * 27 + stage * 28 + j;
				//cout << i << ' ' << temp << '(' << flag << ' ' << stage << ' ' << j << ')' << '\n';
				weekdayv[i][temp] = ans;
				j++;
				if (!flag && j == 27 || temp + 1 == wv) {
					j = 0;
					i++;
				}
				else if (flag && j == 28 || temp + 1 == wv) { 
					j = 0;
					i++;
				}
				if (i == a) {
					if (!flag)
						flag = 1;
					else
						stage++;
					i = 0;
				}
			}
	}

	for (int j = 0; j < wv; j++) {
		for (int i = 0; i < stat.size(); i++)
			o2 << weekdayv[stat[i]][j] << ' ';
		o2 << '\n';
	}
	o2.close();

	ofstream o3("weekdayb.txt");
	vector<vector<int>> weekdayb(a, vector<int>(wb));
	stage = 0, i = 0, j = 0, cnt = 0, flag = 0;
	while (cnt < a * wb) {
		string s;
		input >> s;
		if (s.size() > 6)
			if (s[0] == '(' && s[6] == ')') {
				int ans = stoi(s.substr(1, 2)) * 60 + stoi(s.substr(4, 2));
				cnt++;
				if (ans < 180)
					ans += 1440;
				int temp = flag * 27 + stage * 28 + j;
				//cout << i << ' ' << temp << '(' << flag << ' ' << stage << ' ' << j << ')' << '\n';
				weekdayb[i][temp] = ans;
				j++;
				if (!flag && j == 27 || temp + 1 == wb) {
					j = 0;
					i++;
				}
				else if (flag && j == 28 || temp + 1 == wb) { 
					j = 0;
					i++;
				}
				if (i == a) {
					if (!flag)
						flag = 1;
					else
						stage++;
					i = 0;
				}
			}
	}

	for (int j = 0; j < wb; j++) {
		for (int i = 0; i < stat.size(); i++)
			o3 << weekdayb[stat[i]][j] << ' ';
		o3 << '\n';
	}
	o3.close();

	ofstream o4("saturday.txt");
	vector<vector<int>> saturday(a, vector<int>(sat));
	stage = 0, i = 0, j = 0, cnt = 0, flag = 0;
	while (cnt < a * sat) {
		string s;
		input >> s;
		if (s.size() > 6)
			if (s[0] == '(' && s[6] == ')') {
				int ans = stoi(s.substr(1, 2)) * 60 + stoi(s.substr(4, 2));
				cnt++;
				if (ans < 180)
					ans += 1440;
				int temp = flag * 27 + stage * 28 + j; /// note
				//cout << i << ' ' << temp << '(' << flag << ' ' << stage << ' ' << j << ')' << '\n';
				saturday[i][temp] = ans;
				j++;
				if (!flag && j == 27 || temp + 1 == sat) { /// note
					j = 0;
					i++;
				}
				else if (flag && j == 28) { 
					j = 0;
					i++;
				}
				if (i == a) {
					if (!flag)
						flag = 1;
					else
						stage++;
					i = 0;
				}
			}
	}
	for (int j = 0; j < sat; j++) {
		for (int i = 0; i < stat.size(); i++)
			o4 << saturday[stat[i]][j] << ' ';
		o4 << '\n';
	}
	o4.close();

	ofstream o5("saturdayv.txt");
	vector<vector<int>> saturdayv(a, vector<int>(sat));
	stage = 0, i = 0, j = 0, cnt = 0, flag = 0;
	while (cnt < a * satv) {
		string s;
		input >> s;
		if (s.size() > 6)
			if (s[0] == '(' && s[6] == ')') {
				int ans = stoi(s.substr(1, 2)) * 60 + stoi(s.substr(4, 2));
				cnt++;
				if (ans < 180)
					ans += 1440;
				int temp = flag * 27 + stage * 28 + j;
				//cout << i << ' ' << temp << '(' << flag << ' ' << stage << ' ' << j << ')' << '\n';
				saturdayv[i][temp] = ans;
				j++;
				if (!flag && j == 27 || temp + 1 == satv) {
					j = 0;
					i++;
				}
				else if (flag && j == 28) {
					j = 0;
					i++;
				}
				if (i == a) {
					if (!flag)
						flag = 1;
					else
						stage++;
					i = 0;
				}
			}
	}
	for (int j = 0; j < satv; j++) {
		for (int i = 0; i < stat.size(); i++)
			o5 << saturdayv[stat[i]][j] << ' ';
		o5 << '\n';
	}
	o5.close();
	ofstream o6("sunday.txt");
	vector<vector<int>> sunday(a, vector<int>(sun));
	stage = 0, i = 0, j = 0, cnt = 0, flag = 0;
	while (cnt < a * sun) {
		string s;
		input >> s;
		if (s.size() > 6)
			if (s[0] == '(' && s[6] == ')') {
				int ans = stoi(s.substr(1, 2)) * 60 + stoi(s.substr(4, 2));
				cnt++;
				if (ans < 180)
					ans += 1440;
				int temp = flag * 27 + stage * 28 + j;
				//cout << i << ' ' << temp << '(' << flag << ' ' << stage << ' ' << j << ')' << '\n';
				sunday[i][temp] = ans;
				j++;
				if (!flag && j == 27 || temp + 1 == sun) {
					j = 0;
					i++;
				}
				else if (flag && j == 28) {
					j = 0;
					i++;
				}
				if (i == a) {
					if (!flag)
						flag = 1;
					else
						stage++;
					i = 0;
				}
			}
	}
	for (int j = 0; j < sun; j++) {
		for (int i = 0; i < stat.size(); i++)
			o6 << sunday[stat[i]][j] << ' ';
		o6 << '\n';
	}
	o6.close();
}