#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <queue>

using namespace std;

namespace fs = std::filesystem;

// In Dijkstra, we need to initialise the parameter of the station, whether time or price, as infinity. 2e9 is arbitrarily chosen to be larger than 
// any other value received while computing, so it is referred as infinity.
const int inf = 2e9; 

queue<int> qBfs; // queue used in BFS

int n, m; // n - number of stations, m - number of routes (nearly unused)

// Some notes:
// NS = Nederlandse Spoorwegen = Dutch Railways. If a route name contains "NS", then it's a train route.
// NS Spinter is usually a small route designed for short travels, while NS Intercity is designed for long travels. As the model is extremely small,
//		it barely makes a difference. Sprinter trains are slower because they stop at small stations in small cities.
// In my realisation of the program, all the bus routes are coded with exactly four symbols, and no other route is coded with for symbols.
// "ft" always means "on foot" or "walk"

//function determining which timetable to use
string timetableDefine(string route, string day, int &presentTime) {
		int d;
		if (day == "Mon")
			d = 0;
		if (day == "Tue")
			d = 1;
		if (day == "Wed")
			d = 2;
		if (day == "Thu")
			d = 3;
		if (day == "Fri")
			d = 4;
		if (day == "Sat")
			d = 5;
		if (day == "Sun")
			d = 6;
		if (presentTime < 180) {
			d--;
			presentTime += 1440;
		}
		if (d < 0)
			d += 7;
		if (route.size() == 4) {
			if (d < 5)
				return "weekday.txt";
			if (d == 5)
				return "saturday.txt";
			return "sunday.txt";
		}
		else {
			if (d == 0)
				return "mon.txt";
			if (d == 1)
				return "tue.txt";
			if (d == 2)
				return "wed.txt";
			if (d == 3)
				return "thu.txt";
			if (d == 4)
				return "fri.txt";
			if (d == 5)
				return "sat.txt";
			return "sun.txt";
		}		
}


class route {
private:
	int index=0;
	string id;
	vector<int> stations;
	double boardPrice;
	vector<double> prices;
public:
	route() {

	}
	route(string a, int x);
	void setPrices(int i);
	void set(int place, int v) {
		stations[place] = v;
	}
	string getName() {
		return id;
	}
	int getRouteSize() {
		return stations.size();
	}
	int getRouteStation(int place) {
		return stations[place];
	}
	void setBoardPrice(double price) {
		boardPrice = price;
	}
	void setEdgePrice(double price, int i) {
		prices[i] = price;
	}
	double getBoardPrice() {
		return boardPrice;
	}
	double getEdgePrice(int i) {
		return prices[i];
	}
	~route() {

	}
	
};

vector<route> routeDB; //database of routes
map<string, int> routeDict; // dictionary of name->routeID

route::route(string a, int x) {
	id = a;
	index = routeDB.size();
	stations.resize(x);
	routeDict[a] = index;
}

void route::setPrices(int i) { //initialisation of prices
	prices.resize(stations.size()-1);
	string path = "prices\\";
	path.append(routeDB[i].getName());
	path.append(".txt");
	ifstream input(path);
	double a;
	input>>a;
	setBoardPrice(a);
	for (int i = 0; i < prices.size(); i++) {
		input >> a;
		setEdgePrice(a, i);
	}
}

int routeFind(string k) { //identify the index of the route. they are supposed to be in the database
	return routeDict[k];
}

class station {
private:
	string fullname;
	int id;
	int entered;
	int used;
	string infoPath;
	int transfers;
	int arrTime;
	int depTime;
	int parentStop;
	string routeTrav;
	double arrPrice;
public:
	vector<string> path;
	station() {
		depTime = inf;
		arrTime = inf;
		arrPrice = inf;
		parentStop = -1;
		routeTrav = -1;
		transfers = inf;
		used = 0;
		entered = 0;
		//used.clear();
	}
	station(int index, string name);
	void reset() {
		arrTime = inf;
		depTime = inf;
		arrPrice = inf;
		parentStop = -1;
		routeTrav = "";
		transfers = inf;
		entered = 0;
		used = 0;
		path.clear();
	}
	void setInitial(int ti) {
		depTime = ti;
		arrTime = ti;
		arrPrice = 0;
		parentStop = -1;
		routeTrav = "";
		transfers = 0;
		entered = 0;
		used = 0;
		path.clear();
	}
	string getName() {
		return fullname;
	}
	int getTrans() {
		return transfers;
	}
	int getArrTime() {
		return arrTime;
	}
	int getDepTime() {
		return depTime;
	}
	void setDepTime(int x) {
		depTime = x;
	}
	int getParentStop() {
		return parentStop;
	}
	string getRoute() {
		return routeTrav;
	}
	double getPrice() {
		return arrPrice;
	}
	void setPath(int prev, int cur, string route);
	void setUpdate(string r, int arrTi, int parent, int trans, double price, int depTi) {
		//cout << "Update! " << r << ' ' << depTi << "->" << arrTi << ' ' << parent << "-> ? "<<price<<'\n';
		routeTrav = r;
		depTime = depTi;
		arrTime = arrTi;
		parentStop = parent;
		transfers = trans;
		arrPrice = price;
	}
	void setEntered() {
		entered = 1;
	}
	void setUsed() {
		used = 1;
	}
	int getEntered() {
		return entered;
	}
	int getUsed() {
		return used;
	}
	void Relaxation(string mode, string day, int ti); // update is quite similar for all three modes
	~station() {

	}
	
};

vector<station> stationDB; // station database
map<string, int> stationDict; //dictionary of name->stationID

station::station(int index, string name) {
	id = index;
	fullname = name;
	stationDict[name] = index;
	depTime = inf;
	arrTime = inf;
	arrPrice = inf;
	parentStop = -1;
	routeTrav = "";
	transfers = inf;
	entered = 0;
	used = 0;
	//used.clear();
}

void station::setPath(int prev, int cur, string route) { // changes the path to a certain station
	stationDB[cur].path.clear();
	stationDB[cur].path = stationDB[prev].path;
	stationDB[cur].path.push_back(route);
}

int stationFind(string k) { //identify the id of the station, or report the station is not present in the database
	if (stationDict.count(k))
		return stationDict[k];
	return -1;
}

void station::Relaxation(string mode, string day, int ti) {
	string name = station::getName();
	int sId = stationFind(name);
	string path1 = "stations\\"; //building up path to open and read info about a station
	path1.append(name);
	path1.append(".txt");
	ifstream input1(path1);
	string r;
	int s;
	bool f = 0;
	while (input1.good()) {
		if (f)
			input1.ignore();
		getline(input1, r); // r is route, s is the index of the current station in the route (numerating from 0)
		input1 >> s;
		f = 1;
		string path2 = "timetables\\"; //building up path to open and read a timetable
		path2.append(r);
		path2.append("\\");
		path2.append(timetableDefine(r, day, ti));
		ifstream input2(path2);
		vector<int> traverse;
		int sz = routeDB[routeFind(r)].getRouteSize();
		while (input2.good()) { // update inside routes
			vector<int> temp(sz);
			for (int i = 0; i < sz; i++)
				input2 >> temp[i];
			if (temp[s] >= ti) {
				double price = 0;
				vector<string> pathTemp = stationDB[sId].path;
				bool check = 0;
				// NS=Nederlandse Spoorwegen = Dutch Railways. If a route name contains "NS", then it's a train route
				// In my realisation of the program, all the bus routes are coded with exactly four symbols, and no other route is coded with for symbols
				for (int i = pathTemp.size()-1; i >=0; i--) {
					if (pathTemp[i].size() == 4 && r.find("NS") != string::npos)
						check = 1;
					if (pathTemp[i].find("NS") != string::npos && r.size() == 4)
						check = 1;
					if (pathTemp[i].size() == 4 && r.size() == 4)
						break;
					if (pathTemp[i].find("NS") != string::npos && r.find("NS") != string::npos)
						break;
				}
				if (pathTemp.empty())
					check = 1;
				if (check && r.size() == 4)
					price += 1.08;
				if (check && r.find("NS") != string::npos)
					price += 2.40;
				for (int i = s + 1; i < sz; i++) {
					price+= routeDB[routeFind(r)].getEdgePrice(i - 1);
					//int sId = stationFind(name);
					int toId = routeDB[routeFind(r)].getRouteStation(i);
					if (!stationDB[toId].getUsed()) {
						if (mode == "trans") { //"ttp"=transfers, time, price (priority)
							if (stationDB[toId].getTrans() > stationDB[sId].getTrans() + 1) { //comparing number of transfers
								stationDB[toId].setPath(sId, toId, r);
								stationDB[toId].setUpdate(r, temp[i], sId, stationDB[sId].getTrans() + 1, price+stationDB[sId].getPrice(), temp[s]);
								if (!stationDB[toId].getEntered()) {
									qBfs.push(toId);
									stationDB[toId].setEntered();
								}

							}
							else if (stationDB[toId].getTrans() == stationDB[sId].getTrans() + 1 && stationDB[toId].getArrTime() > temp[i]) { //transfers equal, comparing time
								stationDB[toId].setPath(sId, toId, r);
								stationDB[toId].setUpdate(r, temp[i], sId, stationDB[sId].getTrans() + 1, price+stationDB[sId].getPrice(), temp[s]);
								if (!stationDB[toId].getEntered()) {
									qBfs.push(toId);
									stationDB[toId].setEntered();
								}
							}
						}
						else if (mode == "time") { 
							if (stationDB[toId].getArrTime() > temp[i]) { // comparing time
								stationDB[toId].setPath(sId, toId, r);
								stationDB[toId].setEntered();
								stationDB[toId].setUpdate(r, temp[i], sId, stationDB[sId].getTrans() + 1, price + stationDB[sId].getPrice(), temp[s]);
							}
						}
						else if (mode == "price") { 
							if (stationDB[toId].getPrice() > price + stationDB[sId].getPrice()) { //comparing price
								stationDB[toId].setPath(sId, toId, r);
								stationDB[toId].setEntered();
								stationDB[toId].setUpdate(r, temp[i], sId, stationDB[sId].getTrans() + 1, price + stationDB[sId].getPrice(), temp[s]);
							}
						}
					}
					
				}
				break;
			}
			
		}
	}
	input1.close();
	input1.open("walkedges.txt"); //update using walking edges (there are very few of them)
	f = 0;
	while (input1.good()) {
		if (f)
			input1.ignore();
		f = 1;
		string s;
		getline(input1, s);
		string f;
		getline(input1, f);
		int ti;
		input1 >> ti;
		if (s == station::getName()) {
			int sId = stationFind(s);
			int toId = stationFind(f);
			if (!stationDB[toId].getUsed()) {
				if (mode == "trans") { //"ttp"=transfers, time, price (priority)
					if (stationDB[toId].getTrans() > stationDB[sId].getTrans()) { //comparing number of transfers
						stationDB[toId].setPath(sId, toId, r);
						// "ft" always stands for "on foot" or "walking"
						stationDB[toId].setUpdate("ft", stationDB[sId].getArrTime() + ti, sId, stationDB[sId].getTrans(), stationDB[sId].getPrice(), stationDB[sId].getArrTime());
						if (!stationDB[toId].getEntered()) {
							qBfs.push(toId);
							stationDB[toId].setEntered();
						}

					}
					else if (stationDB[toId].getTrans() == stationDB[sId].getTrans() && stationDB[toId].getArrTime() > stationDB[sId].getArrTime() + ti) { //transfers equal, comparing time
						stationDB[toId].setPath(sId, toId, r);
						stationDB[toId].setUpdate("ft", stationDB[sId].getArrTime() + ti, sId, stationDB[sId].getTrans(), stationDB[sId].getPrice(), stationDB[sId].getArrTime());
						if (!stationDB[toId].getEntered()) {
							qBfs.push(toId);
							stationDB[toId].setEntered();
						}
					}
				}
				else if (mode == "time") { 
					if (stationDB[toId].getArrTime() > stationDB[sId].getArrTime()+ti) { // comparing time
						stationDB[toId].setPath(sId, toId, r);
						stationDB[toId].setEntered();
						stationDB[toId].setUpdate("ft", stationDB[sId].getArrTime() + ti, sId, stationDB[sId].getTrans(), stationDB[sId].getPrice(), stationDB[sId].getArrTime());
					}
				}
				else if (mode == "price") { 
					if (stationDB[toId].getPrice() > stationDB[sId].getPrice()) { //comparing price
						stationDB[toId].setPath(sId, toId, r);
						stationDB[toId].setEntered();
						stationDB[toId].setUpdate("ft", stationDB[sId].getArrTime() + ti, sId, stationDB[sId].getTrans(), stationDB[sId].getPrice(), stationDB[sId].getArrTime());
					}
				}
			}
		}
	}
}

//initialises routes by reading data from files
void initialisationRoutes(const fs::path& pathToScan, string prev) {
	int i = 0;
	for (const auto& entry : fs::directory_iterator(pathToScan)) {
		const auto filenameStr = entry.path().filename().string();
		if (filenameStr == "timetables") {
			initialisationRoutes(entry, filenameStr);
		}
		if (prev == "timetables" && entry.is_directory()) {
			ifstream input;
			string nameOpen = prev; //building filename to open & read
			nameOpen.append("\\"); 
			nameOpen.append(filenameStr); 
			if (filenameStr.size() == 4)
				nameOpen.append("\\weekday.txt");
			else
				nameOpen.append("\\mon.txt");
			input.open(nameOpen); 
			string temp;
			getline(input, temp);
			int v = count(temp.begin(), temp.end(), ' '); //number of spaces equals the number of numbers in the first string, therefore number of stations 
			route create(filenameStr, v);
			routeDB.push_back(create);
			input.close();
		}
		i++;
	}
}

//initialises stations by reading data from files
void initialisationStations(const fs::path& pathToScan, string prev) {
	int i = 0;
	for (const auto& entry : fs::directory_iterator(pathToScan)) {
		const auto filenameStr = entry.path().filename().string();
		if (filenameStr == "stations") {
			initialisationStations(entry, filenameStr);
		}
		if (prev != "") {
			string name = filenameStr; //erasing extension (.txt, 4 symbols) of file to give a full name for the station
			for (int i = 0; i < 4; i++)
				name.pop_back();
			station stat(i, name);
			stationDB.push_back(stat); 
			ifstream input;
			string nameOpen=prev;// building filename to open and read
			nameOpen.append("\\");
			nameOpen.append(filenameStr);
			input.open(nameOpen); // file has information type (route id, index of the stop in the route (numerating from 0))
			bool f = 0;
			string r;
			int v;
			while (input.good()) {
				if (f)
					input.ignore();
				f = 1;
				getline(input, r);
				input >> v;
				routeDB[routeFind(r)].set(v, i);
			}
			input.close();
		}
		i++;
	}
}

//calling individual initialisation function for every route
void initialisationPrices() {
	for (int i = 0; i < routeDB.size(); i++)
		routeDB[i].setPrices(i);
}

void bfs(string s, string f, string day, int ti) { // Computes the route with minimal number of transfers by using BFS 
	int ids = stationFind(s);
	int idf = stationFind(f);
	if (ids == -1) {
		cout << "Starting station not found\n";
		return;
	}
	if (idf == -1) {
		cout << "Destination station not found\n";
		return;
	}
	qBfs.push(ids);
	stationDB[ids].setInitial(ti);
	while (!qBfs.empty()) {
		int v = qBfs.front();
		//cout << "\t\t\tDEBUG: vertice " << v << '\n';
		qBfs.pop();
		stationDB[v].Relaxation("trans", day, stationDB[v].getArrTime()); // "ttp"=transfers, time, price
		stationDB[v].setUsed();
	}
	vector<int> path;
	for (int i = idf; i != ids; i = stationDB[i].getParentStop()) {
		if (i == -1) {
			path.clear();
			break;
		}
		path.push_back(i);
	}
	reverse(path.begin(), path.end());
	if (path.empty()) {
		cout << "It is impossible to reach the mentioned station using the routes included in the database. Posssible reasons:\n";
		cout << "- You set out too late in the evening. The buses and trains will have stopped operating before you get to your destination.\n";
		cout << "Try departing in the morning instead.\n";
		cout << "- You set out at the weekends, when some of the buses do not operate. In the model, buses 7 & 183 are the only buses that drive you \n";
		cout << "to Burggravenlaan, and they are both inoperable at the weekends. \n";
	}
	else {
		cout << "Result:\n";
		for (int i = 0; i < path.size(); i++) {
			string k = stationDB[path[i]].getRoute();
			if (k.size() == 4)
				cout << "Bus " << stoi(k) % 1000 << ' ';
			else if (k == "ft")
				cout << "Walk" << ' ';
			else if (k.find("NS Intercity") != string::npos)
				cout << "NS Intercity ";
			else
				cout << "NS Sprinter ";
			cout << stationDB[stationDB[path[i]].getParentStop()].getName() << " -> "; //parent station
			cout << stationDB[path[i]].getName() << ' '; //current station
			int start = stationDB[path[i]].getDepTime(); //departure time 
			cout << (start / 60)%24 << ':' << start % 60 << " -> ";
			int finish = stationDB[path[i]].getArrTime(); // arrival time
			cout << (finish / 60)%24 << ':' << finish % 60 << '\n'; //arrival time
		}
		cout << "Journey price: " << stationDB[idf].getPrice() << '\n';
	}
}

void dijkstra(string s, string f, string mode, string day, int ti) { // Computing quickest or cheapest route using Dijkstra algorithm
	int ids = stationFind(s);
	int idf = stationFind(f);
	if (ids == -1) {
		cout << "Starting station not found\n";
		return;
	}
	if (idf == -1) {
		cout << "Destination station not found\n";
		return;
	}
	stationDB[ids].setInitial(ti);
	for (int cnt = 0; cnt < n; cnt++) {
		if (mode == "time") {
			int mn = inf;
			int index = -1;
			for (int i = 0; i < n; i++) {
				if (stationDB[i].getArrTime() < mn && !stationDB[i].getUsed()) {
					mn = stationDB[i].getArrTime();
					index = i;
				}
			}
			if (mn == inf)
				break;
			stationDB[index].Relaxation("time", day, mn);
			stationDB[index].setUsed();
		}
		if (mode == "price") {
			double mn = inf;
			int index = -1;
			for (int i = 0; i < n; i++) {
				if (stationDB[i].getPrice() < mn && !stationDB[i].getUsed()) {
					mn = stationDB[i].getPrice();
					index = i;
				}
			}
			if (abs(mn - inf) < 1)
				break;
			stationDB[index].Relaxation("price", day, stationDB[index].getArrTime());
			stationDB[index].setUsed();
		}
	}
	vector<int> path;
	for (int i = idf; i != ids; i = stationDB[i].getParentStop()) {
		if (i == -1) {
			path.clear();
			break;
		}
		path.push_back(i);
	}
	reverse(path.begin(), path.end());
	if (path.empty()) {
		cout << "It is impossible to reach the mentioned station using the routes included in the database. Posssible reasons:\n";
		cout << "- You set out too late in the evening. The buses and trains will have stopped operating before you get to your destination].\n";
		cout << "Try departing in the morning instead.\n";
		cout << "- You set out at the weekends, when some of the buses do not operate. In the model, buses 7 & 183 are the only buses that drive you \n";
		cout << "to Burggravenlaan, and they are both inoperable at the weekends. \n";
	}
	else {
		cout << "Result:\n";
		for (int i = 0; i < path.size(); i++) {
			string k = stationDB[path[i]].getRoute();
			if (k.size() == 4)
				cout << "Bus " << stoi(k) % 1000 << ' ';
			else if (k == "ft")
				cout << "Walk" << ' ';
			else if (k.find("NS Intercity") != string::npos)
				cout << "NS Intercity ";
			else
				cout << "NS Sprinter ";
			cout << stationDB[stationDB[path[i]].getParentStop()].getName() << " -> "; //parent station
			cout << stationDB[path[i]].getName() << ' '; //current station
			int start = stationDB[path[i]].getDepTime(); //departure time 
			cout << (start / 60)%24 << ':' << start % 60 << " -> ";
			int finish = stationDB[path[i]].getArrTime(); // arrival time
			cout << (finish / 60)%24 << ':' << finish % 60 << '\n'; //arrival time
		}
		cout << "Journey price: " << stationDB[idf].getPrice() << '\n';
	}
	
}


int main(int argc, char *argv[])
{
	const fs::path pathToShow{ argc >= 2 ? argv[1] : fs::current_path() };
	initialisationRoutes(pathToShow, "");
	initialisationStations(pathToShow, "");
	initialisationPrices();
	n = stationDB.size();
	m = routeDB.size();
	int queries = 0;
	while (true) {
		cout << "Starting station?\n";
		if(queries)
			cin.ignore();
		string s;
		getline(cin, s);
		cout << '\n';
		cout << "Ending station?\n";
		string f;
		getline(cin, f);
		cout << '\n';
		cout << "Departure time (hh:mm)?\n";
		string t;
		cin >> t;
		int ti = stoi(t.substr(0, t.find(':', 0))) * 60 + stoi(t.substr(t.find(':')+1, 2)); // converting time in minutes since midnight
		cout << '\n';
		cout << "What day are you departing on? Use standard abbreviatures (Mon for Monday, Tue for Tuesday, etc.)\nNote: input is case sensitive\n";
		string day;
		cin >> day;
		cout << '\n';
		cout << "Choose mode: type\n";
		cout << "trans \t for the minimum number of transfers\n";
		cout << "time \t for the quickest arrival\n";
		cout << "price \t for the cheapest trip\n";
		string mode;
		cin >> mode;
		cout << '\n';
		if (mode == "trans") 
			bfs(s, f, day, ti);
		else if(mode=="time" || mode=="price")
			dijkstra(s, f, mode, day, ti);
		else 
			cout << "Unexpected mode. Execution impossible\n";
		queries++;
		for (int i = 0; i < n; i++)
			stationDB[i].reset();
		cout << "\n\n";
	}
}

