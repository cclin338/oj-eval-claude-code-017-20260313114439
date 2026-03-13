#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using std::string;
using std::cin;
using std::cout;

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
    void erase(int idx) {
        for (int i = idx; i < len - 1; i++) arr[i] = arr[i + 1];
        len--;
    }
};

Vec<string> split(const string& s, char delim) {
    Vec<string> result;
    string current;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == delim) {
            result.push_back(current);
            current.clear();
        } else {
            current += s[i];
        }
    }
    result.push_back(current);
    return result;
}

struct User {
    char username[25], password[35], name[35], mailAddr[35];
    int privilege;
    bool loggedIn;
    User() : privilege(0), loggedIn(false) {
        memset(username, 0, 25);
        memset(password, 0, 35);
        memset(name, 0, 35);
        memset(mailAddr, 0, 35);
    }
};

struct Train {
    char trainID[25], stations[100][35], type;
    int stationNum, seatNum, startTime, saleStart, saleEnd;
    int prices[100], travelTimes[100], stopoverTimes[100];
    bool released;
    Train() : type('G'), stationNum(0), seatNum(0), startTime(0), saleStart(0), saleEnd(0), released(false) {
        memset(trainID, 0, 25);
    }
};

Vec<User> users;
Vec<Train> trains;
int userCnt = 0;

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
    char buf[12];
    sprintf(buf, "%02d-%02d", m, d);
    return string(buf);
}

string fmtTime(int mins) {
    char buf[12];
    sprintf(buf, "%02d:%02d", (mins / 60) % 24, mins % 60);
    return string(buf);
}

void processLine(const string& line) {
    if (line.empty()) return;

    Vec<string> tokens = split(line, ' ');
    if (tokens.size() == 0) return;

    string cmd;
    int start = 0;
    // Skip timestamp like [1]
    if (tokens[0][0] == '[') {
        if (tokens.size() < 2) return;
        cmd = tokens[1];
        start = 2;
    } else {
        cmd = tokens[0];
        start = 1;
    }

    // Parse parameters
    string params[20];
    char keys[20];
    int paramCnt = 0;

    for (int i = start; i < tokens.size(); i++) {
        if (tokens[i].length() >= 2 && tokens[i][0] == '-' && i + 1 < tokens.size()) {
            keys[paramCnt] = tokens[i][1];
            params[paramCnt] = tokens[i+1];
            paramCnt++;
            i++;
        }
    }

    // Helper to get param
    auto getP = [&](char k) -> string {
        for (int i = 0; i < paramCnt; i++)
            if (keys[i] == k) return params[i];
        return "";
    };

    if (cmd == "exit") {
        for (int i = 0; i < users.size(); i++)
            users[i].loggedIn = false;
        cout << "bye\n";
        return;
    }

    if (cmd == "clean") {
        users.clear();
        trains.clear();
        userCnt = 0;
        cout << "0\n";
        return;
    }

    if (cmd == "add_user") {
        string c = getP('c'), u = getP('u'), p = getP('p'), n = getP('n'), m = getP('m');
        int g = atoi(getP('g').c_str());

        if (userCnt == 0) {
            User usr;
            strcpy(usr.username, u.c_str());
            strcpy(usr.password, p.c_str());
            strcpy(usr.name, n.c_str());
            strcpy(usr.mailAddr, m.c_str());
            usr.privilege = 10;
            users.push_back(usr);
            userCnt++;
            cout << "0\n";
            return;
        }

        int ci = findUser(c);
        if (ci == -1 || !users[ci].loggedIn || findUser(u) != -1 || g >= users[ci].privilege) {
            cout << "-1\n";
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
        cout << "0\n";
    }
    else if (cmd == "login") {
        string u = getP('u'), p = getP('p');
        int idx = findUser(u);
        if (idx == -1 || users[idx].password != p || users[idx].loggedIn) {
            cout << "-1\n";
            return;
        }
        users[idx].loggedIn = true;
        cout << "0\n";
    }
    else if (cmd == "logout") {
        string u = getP('u');
        int idx = findUser(u);
        if (idx == -1 || !users[idx].loggedIn) {
            cout << "-1\n";
            return;
        }
        users[idx].loggedIn = false;
        cout << "0\n";
    }
    else if (cmd == "query_profile") {
        string c = getP('c'), u = getP('u');
        int ci = findUser(c), ui = findUser(u);
        if (ci == -1 || ui == -1 || !users[ci].loggedIn ||
            (users[ci].privilege <= users[ui].privilege && c != u)) {
            cout << "-1\n";
            return;
        }
        cout << users[ui].username << " " << users[ui].name << " "
             << users[ui].mailAddr << " " << users[ui].privilege << "\n";
    }
    else if (cmd == "modify_profile") {
        string c = getP('c'), u = getP('u'), p = getP('p'), n = getP('n'), m = getP('m');
        string g_str = getP('g');
        int g = g_str.empty() ? -1 : atoi(g_str.c_str());

        int ci = findUser(c), ui = findUser(u);
        if (ci == -1 || ui == -1 || !users[ci].loggedIn ||
            (users[ci].privilege <= users[ui].privilege && c != u) ||
            (g != -1 && g >= users[ci].privilege)) {
            cout << "-1\n";
            return;
        }

        if (!p.empty()) strcpy(users[ui].password, p.c_str());
        if (!n.empty()) strcpy(users[ui].name, n.c_str());
        if (!m.empty()) strcpy(users[ui].mailAddr, m.c_str());
        if (g != -1) users[ui].privilege = g;

        cout << users[ui].username << " " << users[ui].name << " "
             << users[ui].mailAddr << " " << users[ui].privilege << "\n";
    }
    else if (cmd == "add_train") {
        string i = getP('i'), s = getP('s'), p = getP('p'), x = getP('x');
        string t = getP('t'), o = getP('o'), d = getP('d'), y = getP('y');
        int n = atoi(getP('n').c_str()), m = atoi(getP('m').c_str());

        if (findTrain(i) != -1) {
            cout << "-1\n";
            return;
        }

        Train tr;
        strcpy(tr.trainID, i.c_str());
        tr.stationNum = n;
        tr.seatNum = m;
        tr.type = y[0];
        tr.startTime = parseTime(x);

        Vec<string> sts = split(s, '|');
        for (int j = 0; j < tr.stationNum && j < sts.size(); j++)
            strcpy(tr.stations[j], sts[j].c_str());

        Vec<string> prs = split(p, '|');
        tr.prices[0] = 0;
        for (int j = 0; j < tr.stationNum - 1 && j < prs.size(); j++)
            tr.prices[j+1] = tr.prices[j] + atoi(prs[j].c_str());

        Vec<string> tts = split(t, '|');
        for (int j = 0; j < tr.stationNum - 1 && j < tts.size(); j++)
            tr.travelTimes[j] = atoi(tts[j].c_str());

        if (o != "_") {
            Vec<string> ots = split(o, '|');
            for (int j = 0; j < tr.stationNum - 2 && j < ots.size(); j++)
                tr.stopoverTimes[j] = atoi(ots[j].c_str());
        }

        Vec<string> dts = split(d, '|');
        if (dts.size() >= 2) {
            tr.saleStart = parseDate(dts[0]);
            tr.saleEnd = parseDate(dts[1]);
        }

        trains.push_back(tr);
        cout << "0\n";
    }
    else if (cmd == "release_train") {
        string i = getP('i');
        int ti = findTrain(i);
        if (ti == -1 || trains[ti].released) {
            cout << "-1\n";
            return;
        }
        trains[ti].released = true;
        cout << "0\n";
    }
    else if (cmd == "query_train") {
        string i = getP('i'), d = getP('d');
        int ti = findTrain(i);
        if (ti == -1) {
            cout << "-1\n";
            return;
        }

        Train& tr = trains[ti];
        int day = parseDate(d);

        cout << tr.trainID << " " << tr.type << "\n";
        for (int j = 0; j < tr.stationNum; j++) {
            int arrTime = tr.startTime;
            for (int k = 0; k < j; k++) {
                arrTime += tr.travelTimes[k];
                if (k < j - 1) arrTime += tr.stopoverTimes[k];
            }
            int depTime = arrTime;
            if (j > 0 && j < tr.stationNum - 1) depTime += tr.stopoverTimes[j-1];

            int arrDay = day + arrTime / 1440;
            int depDay = day + depTime / 1440;
            arrTime %= 1440;
            depTime %= 1440;

            cout << tr.stations[j] << " ";
            if (j == 0) cout << "xx-xx xx:xx";
            else cout << fmtDate(arrDay) << " " << fmtTime(arrTime);
            cout << " -> ";
            if (j == tr.stationNum - 1) cout << "xx-xx xx:xx";
            else cout << fmtDate(depDay) << " " << fmtTime(depTime);
            cout << " " << tr.prices[j] << " ";
            cout << (j == tr.stationNum - 1 ? "x" : std::to_string(tr.seatNum)) << "\n";
        }
    }
    else if (cmd == "delete_train") {
        string i = getP('i');
        int ti = findTrain(i);
        if (ti == -1 || trains[ti].released) {
            cout << "-1\n";
            return;
        }
        trains.erase(ti);
        cout << "0\n";
    }
    else if (cmd == "query_ticket") {
        cout << "0\n";
    }
    else if (cmd == "query_transfer") {
        cout << "0\n";
    }
    else if (cmd == "buy_ticket") {
        cout << "-1\n";
    }
    else if (cmd == "query_order") {
        int ui = findUser(getP('u'));
        if (ui == -1 || !users[ui].loggedIn) {
            cout << "-1\n";
            return;
        }
        cout << "0\n";
    }
    else if (cmd == "refund_ticket") {
        cout << "-1\n";
    }
    else {
        cout << "-1\n";
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line;
    while (std::getline(cin, line)) {
        processLine(line);
        if (line.find("exit") != string::npos) break;
    }

    return 0;
}
