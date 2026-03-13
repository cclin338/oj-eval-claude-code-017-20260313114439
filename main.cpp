#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using std::string;

// Custom vector for non-STL implementation
template<typename T>
class Vec {
private:
    T* arr;
    int cap, len;
    void expand() {
        cap = (cap == 0) ? 4 : cap * 2;
        T* tmp = new T[cap];
        for (int i = 0; i < len; i++) tmp[i] = arr[i];
        delete[] arr;
        arr = tmp;
    }
public:
    Vec() : arr(nullptr), cap(0), len(0) {}
    ~Vec() { delete[] arr; }
    void push_back(const T& v) {
        if (len >= cap) expand();
        arr[len++] = v;
    }
    T& operator[](int i) { return arr[i]; }
    const T& operator[](int i) const { return arr[i]; }
    int size() const { return len; }
    void clear() { len = 0; }
    bool empty() const { return len == 0; }
    T* begin() { return arr; }
    T* end() { return arr + len; }
};

// Simple split function
Vec<string> split(const string& s, char delim) {
    Vec<string> result;
    string current;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == delim) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += s[i];
        }
    }
    if (!current.empty()) result.push_back(current);
    return result;
}

struct User {
    char username[25], password[35], name[35], mailAddr[35];
    int privilege;
    bool loggedIn;
    User() : privilege(0), loggedIn(false) {
        username[0] = password[0] = name[0] = mailAddr[0] = '\0';
    }
};

struct Train {
    char trainID[25], stations[100][35], type;
    int stationNum, seatNum, startTime, saleStart, saleEnd;
    int prices[100], travelTimes[100], stopoverTimes[100];
    bool released;
    Train() : stationNum(0), seatNum(0), type('G'), released(false) {
        trainID[0] = '\0';
    }
};

struct Order {
    char username[25], trainID[25];
    int fromSt, toSt, num, price, status, timestamp, departDate;
    Order() : num(0), price(0), status(0), timestamp(0), departDate(0) {
        username[0] = trainID[0] = '\0';
        fromSt = toSt = 0;
    }
};

struct TrainDay {
    char trainID[25];
    int startDate, seats[100];
    TrainDay() : startDate(0) {
        trainID[0] = '\0';
    }
};

Vec<User> users;
Vec<Train> trains;
Vec<Order> orders;
Vec<TrainDay> trainDays;
int userCnt = 0, orderTs = 0;

int parseTime(const string& s) {
    return ((s[0]-'0')*10+(s[1]-'0'))*60 + (s[3]-'0')*10+(s[4]-'0');
}

int parseDate(const string& s) {
    int m = (s[0]-'0')*10+(s[1]-'0'), d = (s[3]-'0')*10+(s[4]-'0');
    return (m==6) ? d : (m==7) ? d+30 : d+61;
}

string fmtDate(int day) {
    int m, d;
    if (day <= 30) { m = 6; d = day; }
    else if (day <= 61) { m = 7; d = day - 30; }
    else { m = 8; d = day - 61; }
    char buf[10];
    sprintf(buf, "%02d-%02d", m, d);
    return string(buf);
}

string fmtTime(int mins) {
    char buf[10];
    sprintf(buf, "%02d:%02d", mins/60, mins%60);
    return string(buf);
}

int findUser(const string& u) {
    for (int i = 0; i < users.size(); i++)
        if (u == users[i].username) return i;
    return -1;
}

int findTrain(const string& id) {
    for (int i = 0; i < trains.size(); i++)
        if (id == trains[i].trainID) return i;
    return -1;
}

int findStation(const Train& t, const string& s) {
    for (int i = 0; i < t.stationNum; i++)
        if (s == t.stations[i]) return i;
    return -1;
}

TrainDay* getTrainDay(const string& id, int date) {
    for (int i = 0; i < trainDays.size(); i++)
        if (string(trainDays[i].trainID) == id && trainDays[i].startDate == date)
            return &trainDays[i];
    return nullptr;
}

void processCmd(const string& line) {
    Vec<string> parts = split(line, ' ');
    if (parts.empty()) return;

    string cmd = parts[0];
    // Remove timestamp prefix like [1]
    if (!cmd.empty() && cmd[0] == '[') {
        if (parts.size() < 2) return;
        cmd = parts[1];
    }

    string c, u, p, n, m, i, d, f, t, s, x, o, y, q;
    int g = -1, nm = -1, sn = -1, sm = -1;

    for (size_t j = 0; j < parts.size(); j++) {
        if (parts[j].length() >= 2 && parts[j][0] == '-' && j+1 < parts.size()) {
            char k = parts[j][1];
            string v = parts[j+1];
            if (k == 'c') c = v;
            else if (k == 'u') u = v;
            else if (k == 'p') p = v;
            else if (k == 'n') { if (!v.empty() && v[0] >= '0' && v[0] <= '9') nm = atoi(v.c_str()); else n = v; }
            else if (k == 'm') { if (!v.empty() && v[0] >= '0' && v[0] <= '9') sm = atoi(v.c_str()); else m = v; }
            else if (k == 'g') g = atoi(v.c_str());
            else if (k == 'i') i = v;
            else if (k == 'd') d = v;
            else if (k == 'f') f = v;
            else if (k == 't') t = v;
            else if (k == 's') s = v;
            else if (k == 'x') x = v;
            else if (k == 'o') o = v;
            else if (k == 'y') y = v;
            else if (k == 'q') q = v;
        }
    }

    if (cmd == "add_user") {
        if (userCnt == 0) {
            User usr;
            strcpy(usr.username, u.c_str());
            strcpy(usr.password, p.c_str());
            strcpy(usr.name, n.c_str());
            strcpy(usr.mailAddr, m.c_str());
            usr.privilege = 10;
            users.push_back(usr);
            userCnt++;
            std::cout << "0\n";
            return;
        }
        int ci = findUser(c);
        if (ci == -1 || !users[ci].loggedIn || findUser(u) != -1 || g >= users[ci].privilege) {
            std::cout << "-1\n";
            return;
        }
        User usr;
        strcpy(usr.username, u.c_str());
        strcpy(usr.password, p.c_str());
        strcpy(usr.name, n.c_str());
        strcpy(usr.mailAddr, m.c_str());
        usr.privilege = g;
        users.push_back(usr);
        userCnt++;
        std::cout << "0\n";
    }
    else if (cmd == "login") {
        int idx = findUser(u);
        if (idx == -1 || users[idx].password != p || users[idx].loggedIn) {
            std::cout << "-1\n";
            return;
        }
        users[idx].loggedIn = true;
        std::cout << "0\n";
    }
    else if (cmd == "logout") {
        int idx = findUser(u);
        if (idx == -1 || !users[idx].loggedIn) {
            std::cout << "-1\n";
            return;
        }
        users[idx].loggedIn = false;
        std::cout << "0\n";
    }
    else if (cmd == "query_profile") {
        int ci = findUser(c), ui = findUser(u);
        if (ci == -1 || ui == -1 || !users[ci].loggedIn ||
            (users[ci].privilege <= users[ui].privilege && c != u)) {
            std::cout << "-1\n";
            return;
        }
        std::cout << users[ui].username << " " << users[ui].name << " "
                  << users[ui].mailAddr << " " << users[ui].privilege << "\n";
    }
    else if (cmd == "modify_profile") {
        int ci = findUser(c), ui = findUser(u);
        if (ci == -1 || ui == -1 || !users[ci].loggedIn ||
            (users[ci].privilege <= users[ui].privilege && c != u) ||
            (g != -1 && g >= users[ci].privilege)) {
            std::cout << "-1\n";
            return;
        }
        if (!p.empty()) strcpy(users[ui].password, p.c_str());
        if (!n.empty()) strcpy(users[ui].name, n.c_str());
        if (!m.empty()) strcpy(users[ui].mailAddr, m.c_str());
        if (g != -1) users[ui].privilege = g;
        std::cout << users[ui].username << " " << users[ui].name << " "
                  << users[ui].mailAddr << " " << users[ui].privilege << "\n";
    }
    else if (cmd == "add_train") {
        if (findTrain(i) != -1) {
            std::cout << "-1\n";
            return;
        }
        Train tr;
        strcpy(tr.trainID, i.c_str());
        tr.stationNum = sn;
        tr.seatNum = sm;
        tr.type = y[0];
        tr.startTime = parseTime(x);

        Vec<string> sts = split(s, '|');
        for (int j = 0; j < tr.stationNum && j < (int)sts.size(); j++)
            strcpy(tr.stations[j], sts[j].c_str());

        Vec<string> prs = split(p, '|');
        tr.prices[0] = 0;
        for (int j = 0; j < tr.stationNum-1 && j < (int)prs.size(); j++)
            tr.prices[j+1] = tr.prices[j] + atoi(prs[j].c_str());

        Vec<string> tts = split(t, '|');
        for (int j = 0; j < tr.stationNum-1 && j < (int)tts.size(); j++)
            tr.travelTimes[j] = atoi(tts[j].c_str());

        if (o != "_") {
            Vec<string> ots = split(o, '|');
            for (int j = 0; j < tr.stationNum-2 && j < (int)ots.size(); j++)
                tr.stopoverTimes[j] = atoi(ots[j].c_str());
        }

        Vec<string> dts = split(d, '|');
        if (dts.size() >= 2) {
            tr.saleStart = parseDate(dts[0]);
            tr.saleEnd = parseDate(dts[1]);
        }

        trains.push_back(tr);
        std::cout << "0\n";
    }
    else if (cmd == "release_train") {
        int ti = findTrain(i);
        if (ti == -1 || trains[ti].released) {
            std::cout << "-1\n";
            return;
        }
        trains[ti].released = true;

        // Create ticket availability for all dates
        for (int dt = trains[ti].saleStart; dt <= trains[ti].saleEnd; dt++) {
            TrainDay td;
            strcpy(td.trainID, trains[ti].trainID);
            td.startDate = dt;
            for (int j = 0; j < trains[ti].stationNum - 1; j++)
                td.seats[j] = trains[ti].seatNum;
            trainDays.push_back(td);
        }
        std::cout << "0\n";
    }
    else if (cmd == "query_train") {
        int ti = findTrain(i);
        if (ti == -1) {
            std::cout << "-1\n";
            return;
        }
        Train& tr = trains[ti];
        int day = parseDate(d);

        std::cout << tr.trainID << " " << tr.type << "\n";
        for (int j = 0; j < tr.stationNum; j++) {
            int arrTime = tr.startTime, depTime = tr.startTime;
            int arrDay = day, depDay = day;

            for (int k = 0; k < j; k++) {
                depTime += tr.travelTimes[k];
                if (k < j-1) depTime += tr.stopoverTimes[k];
            }
            arrTime = depTime;
            if (j > 0) arrTime -= tr.stopoverTimes[j-1];

            while (arrTime >= 1440) { arrTime -= 1440; arrDay++; }
            while (depTime >= 1440) { depTime -= 1440; depDay++; }

            std::cout << tr.stations[j] << " ";
            if (j == 0) std::cout << "xx-xx xx:xx";
            else std::cout << fmtDate(arrDay) << " " << fmtTime(arrTime);
            std::cout << " -> ";
            if (j == tr.stationNum - 1) std::cout << "xx-xx xx:xx";
            else std::cout << fmtDate(depDay) << " " << fmtTime(depTime);
            std::cout << " " << tr.prices[j] << " ";

            if (j == tr.stationNum - 1) {
                std::cout << "x\n";
            } else {
                TrainDay* td = getTrainDay(string(tr.trainID), day);
                if (td) std::cout << td->seats[j] << "\n";
                else std::cout << tr.seatNum << "\n";
            }
        }
    }
    else if (cmd == "delete_train") {
        int ti = findTrain(i);
        if (ti == -1 || trains[ti].released) {
            std::cout << "-1\n";
            return;
        }
        // Remove the train (simple approach)
        for (int j = ti; j < trains.size() - 1; j++)
            trains[j] = trains[j+1];
        trains.clear();
        for (int j = 0; j < trains.size(); j++)
            if (j != ti) trains.push_back(trains[j]);
        std::cout << "0\n";
    }
    else if (cmd == "clean") {
        users.clear();
        trains.clear();
        orders.clear();
        trainDays.clear();
        userCnt = 0;
        orderTs = 0;
        std::cout << "0\n";
    }
    else if (cmd == "exit") {
        // Log out all users
        for (int i = 0; i < users.size(); i++)
            users[i].loggedIn = false;
        std::cout << "bye\n";
    }
    else {
        std::cout << "-1\n";
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        processCmd(line);
        if (line.find("exit") != string::npos) break;
    }

    return 0;
}
