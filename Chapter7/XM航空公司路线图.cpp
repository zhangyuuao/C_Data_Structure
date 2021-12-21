#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <cstring>
#include <sstream>
#include <cstdlib>
#define AirportNum 79
#define ERROR -1
using namespace std;

/*用来存储起飞和降落的时间。包括天、时、分钟。*/
typedef struct Date
{
    int day;
    int hour;
    int minite;
} Date;

typedef struct Flight
{
    int Flight_ID;
    string Departure_date;
    string label;
    int Flight_NO;
    int Departure_airport;
    int Arrival_airport;
    string Departure_time;
    string Arrival_time;
    int Airplane_ID;
    int Airplane_model;
    int Air_fares;
    int time;
} Flight;

struct l_graph;

/*弧的结点，因为一个结点会有许多弧，这些弧就是以链表形式存储的。*/
typedef struct Arc1_Node
{
    l_graph *arr;
    Arc1_Node *next;
} Arc1_Node, *Arc1;

/*存储机场信息的图，主要是用邻接表撒。*/
typedef struct l_graph
{
    int Airpot_num; //机场ID
    bool IsRead;    //是否被访问
    Arc1 next;      //与该机场相连的弧，这是一个邻接表。
} l_graph, Adj_graph[AirportNum];

/*航班信息*/
typedef struct FlightInfo
{
    int DepartureTime; //离开时间
    int ArrivalTime;   //到达时间
    int FlightID;      //航班ID顺序
    int AirplaneModel;
    int AirFares; //费用
} FlightInfo;

/*邻接矩阵的元素：链表*/
typedef struct Arc2_Node
{
    FlightInfo e;
    Arc2_Node *next;
} Arc2_Node, *Arc;

/*邻接矩阵。*/
typedef struct AdjM_graph
{
    int *Airpot_num;
    Arc arcMatrix[AirportNum][AirportNum];
} AdjM_graph;

typedef struct NodeforR4
{
    int length;
    int IDtrace[70];
    int StartTime;
    int SumTime;
    int NowTime;
    char trace[70];
} NodeforR4;

typedef struct NodeforR6
{
    int length;
    int IDtrace[70];
    int StartTime;
    int SumTime;
    int NowTime;
    int SumFare;
    char trace[70];
} NodeforR6;

//给定起飞时段或者降落时段或者机型要求，求任意两个机场的多个备选航线
typedef struct Request
{
    int startTime1;
    int startTime2;
    int endTime1;
    int endTime2;
    bool model[6];
} Request;

/*将读取到文件的字符串数据流中的一部分转化为数字。
例如，这个函数用来将每一行数据的Departure Airport 和 Arrival Airport的序号转化为int类型。*/
int ToNum(string e, int i)
{
    int sum = 0;
    for (int j = 0; e[j] >= '0' && e[j] <= '9'; i++, j++)
    {
        sum *= 10;
        sum += e[j] - '0';
    }
    return sum;
}

/*将航班时间转化为分钟.*/
int Totime(Date date)
{
    date.hour += 24 * (date.day - 5);
    date.minite += 60 * date.hour;
    return date.minite;
}

Date calDepDate(Flight Flight[], int i)
{
    Date d;
    int index = Flight[i].Departure_time.find(':') + 1;
    d.day = Flight[i].Departure_time[2] - '0';
    d.hour = ToNum(Flight[i].Departure_time.substr(9), 9);
    d.minite = ToNum(Flight[i].Departure_time.substr(index), index);
    return d;
}

Date calArrDate(Flight Flight[], int i)
{
    Date d;
    int index = Flight[i].Departure_time.find(':') + 1;
    d.day = Flight[i].Arrival_time[2] - '0';
    d.hour = ToNum(Flight[i].Arrival_time.substr(9), 9);
    d.minite = ToNum(Flight[i].Arrival_time.substr(index), index);
    return d;
}

void init(Flight Flight[])
{
    ifstream inFile;
    inFile.open("data.csv");
    if (!inFile)
    {
        cout << "ERROR" << endl;
        exit(1);
    }
    int i = 0;
    string line;
    string field;
    while (getline(inFile, line) && i <= 2346)
    {
        string field;
        Date d;
        istringstream sin(line);
        getline(sin, field, ',');
        Flight[i].Flight_ID = atoi(field.c_str());
        getline(sin, field, ',');
        Flight[i].Departure_date = field.c_str();
        getline(sin, field, ',');
        Flight[i].label = field.c_str();
        getline(sin, field, ',');
        Flight[i].Flight_NO = atoi(field.c_str());
        getline(sin, field, ',');
        Flight[i].Departure_airport = atoi(field.c_str());
        getline(sin, field, ',');
        Flight[i].Arrival_airport = atoi(field.c_str());
        getline(sin, field, ',');
        Flight[i].Departure_time = field.c_str();
        getline(sin, field, ',');
        Flight[i].Arrival_time = field.c_str();
        getline(sin, field, ',');
        Flight[i].Airplane_ID = atoi(field.c_str());
        getline(sin, field, ',');
        Flight[i].Airplane_model = atoi(field.c_str());
        getline(sin, field, ',');
        Flight[i].Air_fares = atoi(field.c_str());
        Date d1, d2;
        d1 = calDepDate(Flight, i);
        d2 = calArrDate(Flight, i);
        Flight[i].time = Totime(d2) - Totime(d1);
        i++;
    }
    for(int j = 1 ; j <= 2346 ; j++){
        cout << Flight[j].Flight_ID << ' ' << Flight[j].Departure_date << ' ' << Flight[j].label << ' ' << Flight[j].Flight_NO << ' ' << Flight[j].Departure_time << ' ' << Flight[j].Arrival_time << ' ' << Flight[j].Airplane_ID << ' ' << Flight[j].Air_fares << ' ' << endl  ;
    }
    cout << "Load data succesfully." << endl;
}

/*将图初始化。g1[i].Airpot_num为原CSV数据集中对应的每一个机场的编号，共79个，已经在前面预定义过。
第一个循环先做初始化，机场之间的连通性设置为NULL。并且设置每一个都没有被访问过。
确定连通性即一趟飞机的离开机场序号和到达机场编号后就开始创建链表来“联通”。*/
void init(Adj_graph &g1, Flight Flight[])
{
    for (int i = 1; i <= AirportNum; i++)
    {
        g1[i].Airpot_num = i;
        g1[i].next = NULL;
        g1[i].IsRead = 0;
    }
    for (int i = 1; i <= 2346; i++)
    {
        int dep = Flight[i].Departure_airport;
        int arr = Flight[i].Arrival_airport;
        Arc1 p = (Arc1_Node *)malloc(sizeof(Arc1_Node));
        p->arr = &g1[arr];
        Arc1 q = g1[dep].next;
        Arc1 k = q;
        if (k)
        {
            if (k->arr->Airpot_num == p->arr->Airpot_num)
                continue;
        }
        /*如果这趟航班所连通的两个机场已经被连通，则跳过继续。
        先遍历与该航班离开的机场相连的所有机场。如果没有被连通，则开始创建。*/
        bool IsNew = 1;
        while (k)
        {
            if (k->arr->Airpot_num == p->arr->Airpot_num)
            {
                IsNew = 0;
                break;
            }
            q = k;
            k = k->next;
        }
        if (IsNew)
        {
            if (q)
            {
                q->next = p;
                p->next = NULL;
            }
            else
            {
                g1[dep].next = p;
                p->next = NULL;
            }
        }
    }
}

void init(AdjM_graph &g2, Flight Flight[])
{
    g2.Airpot_num = (int *)malloc(AirportNum * (sizeof(int)));
    for (int i = 0; i < AirportNum; i++)
    {
        g2.Airpot_num[i] = i + 1;
        for (int j = 0; j < AirportNum; j++)
        {
            g2.arcMatrix[i][j] = NULL;
        }
    }
    for (int i = 1; i <= 2346; i++)
    {
        Arc p = (Arc2_Node *)malloc(sizeof(Arc2_Node));
        p->e.FlightID = Flight[i].Flight_ID;
        int dep = Flight[i].Departure_airport;
        int arr = Flight[i].Arrival_airport;
        Date d;
        d = calDepDate(Flight, i);
        p->e.DepartureTime = Totime(d);
        d = calArrDate(Flight, i);
        p->e.ArrivalTime = Totime(d);
        p->e.AirplaneModel = Flight[i].Airplane_model;
        p->e.AirFares = Flight[i].Air_fares;
        p->next = NULL;
        if (g2.arcMatrix[dep - 1][arr - 1])
        {
            p->next = g2.arcMatrix[dep - 1][arr - 1]->next;
            g2.arcMatrix[dep - 1][arr - 1]->next = p;
        }
        else
        {
            g2.arcMatrix[dep - 1][arr - 1] = p;
        }
    }
}

/*深度优先遍历*/
void dfs(l_graph *g1)
{
    stack<l_graph *> s;
    s.push(g1);
    while (!s.empty())
    {
        l_graph *p = s.top();
        s.pop();
        if (!p->IsRead)
        {
            cout << p->Airpot_num << " ";
            p->IsRead = 1;
        }
        Arc1 q = p->next;
        while (q)
        {
            if (!q->arr->IsRead)
                s.push(q->arr);
            q = q->next;
        }
    }
}

/*广度优先遍历*/
void bfs(l_graph *g1)
{
    queue<l_graph *> s;
    s.push(g1);
    while (!s.empty())
    {
        l_graph *p = s.front();
        s.pop();
        if (!p->IsRead)
        {
            cout << p->Airpot_num << " ";
            p->IsRead = 1;
        }
        Arc1 q = p->next;
        while (q)
        {
            if (!q->arr->IsRead)
                s.push(q->arr);
            q = q->next;
        }
    }
}

void TraverseDFS(Adj_graph &g1)
{
    cout << "深度优先遍历结果" << endl;
    int index;
    cout << "输入起始机场序号:" << endl ;
    cin >> index;
    dfs(&g1[index]);
    for (int i = 1; i <= AirportNum; i++)
    {
        if (g1[i].IsRead == 0)
        {
            dfs(&g1[i]);
        }
    }
}

void TraverseBFS(Adj_graph &g1)
{
    cout << "广度优先遍历结果:" << endl;
    int index;
    cout << "输入起始机场序号:" << endl ;
    cin >> index;
    bfs(&g1[index]);
    for (int i = 1; i <= AirportNum; i++)
    {
        if (g1[i].IsRead == 0)
        {
            bfs(&g1[i]);
        }
    }
}

/*一次中转*/
bool TrasOnes(AdjM_graph &g2, int dep, int arr)
{
    for (int i = 0; i < AirportNum; i++)
    {
        if (g2.arcMatrix[dep][i])
        {
            if (g2.arcMatrix[i][arr])
            {
                cout << "\"" << dep + 1 << "-" << i + 1 << '-' << arr + 1 << "\" ";
                return 1;
            }
        }
    }
    return 0;
}
/*二次中转*/
bool TrasTwice(AdjM_graph &g2, int dep, int arr)
{
    for (int i = 0; i < AirportNum; i++)
    {
        if (g2.arcMatrix[dep][i] && i != arr)
        {
            for (int j = 0; j < AirportNum; j++)
            {
                if (g2.arcMatrix[i][j] && j != dep)
                {
                    if (g2.arcMatrix[j][arr])
                    {
                        cout << "\"" << dep + 1 << "-" << i + 1 << '-' << j + 1 << '-' << arr + 1 << "\" ";
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

void test(){
    printf("%c%c%c%c%c " , 0x5A, 0x68, 0x61, 0x6E, 0x67) ;
    printf("%c%c%c%c%c " , 0x59, 0x75, 0x79, 0x61, 0x6F) ;
}

/*使用邻接矩阵表来完成任意两个机场的可连通性，包括是否可以直飞、1次中转、2次中转等。*/
void request3(AdjM_graph &g2)
{
    cout << "请分别输入出发机场与目的机场的序号:\n";
    int arr = 0, dep = 0;
    cin >> dep >> arr;
    if (dep == arr)
        cout << "出发与目的地相同，发生错误" << endl;
    dep -= 1;
    arr -= 1;
    //转换为矩阵内部序号
    cout << "直飞:" << string((g2.arcMatrix[dep][arr] == NULL) ? "不行诶" : "行") << endl;
    cout << "一次中转:" << string(TrasOnes(g2, dep, arr) ? "行" : "不行诶") << endl;
    cout << "两次中转:" << string(TrasTwice(g2, dep, arr) ? "行" : "不行诶") << endl;
}

/*求任意两个机场之间的最短飞行时间（含转机停留时间）。*/
void request4(AdjM_graph &g2, Flight Flight[])
{
    cout << "请分别输入出发机场与目的机场的序号:" << endl;
    int arr = 0, dep = 0;
    cin >> dep >> arr;
    dep -= 1;
    arr -= 1; //转换为矩阵内部序号
    if (dep == arr)
    {
        cout << "出发与目的地相同，发生错误";
        return;
    }
    int min = 10000;
    char MinTrace[70] = {dep + 1 , arr + 1 , 0};
    int MinIDtrace[70] = {0};
    int trLength = 2;
    stack<NodeforR4 *> s4;
    for (int i = 0; i < AirportNum; i++)
    { //初始结点入栈
        if (g2.arcMatrix[dep][i])
        { //如果这里有航班
            Arc p = g2.arcMatrix[dep][i];
            while (p)
            {
                if (i == arr)
                {
                    if (p->e.ArrivalTime - p->e.DepartureTime < min)
                    {
                        min = p->e.ArrivalTime - p->e.DepartureTime;
                        MinIDtrace[0] = p->e.FlightID;
                    }
                    p = p->next;
                    continue;
                }
                NodeforR4 *NodeforR = (NodeforR4 *)malloc(sizeof(NodeforR4));
                NodeforR->length = 2;
                NodeforR->StartTime = p->e.DepartureTime;
                NodeforR->SumTime = p->e.ArrivalTime - NodeforR->StartTime;
                NodeforR->NowTime = p->e.ArrivalTime;
                NodeforR->trace[0] = dep + 1;
                NodeforR->trace[1] = i + 1;
                NodeforR->trace[69] = 0;
                NodeforR->IDtrace[0] = p->e.FlightID;
                s4.push(NodeforR);
                p = p->next;
            }
        }
    }
    cout << "查询中..." << endl;
    while (!s4.empty())
    {
        NodeforR4 *n = s4.top();
        s4.pop();
        int depn = n->trace[n->length - 1] - 1; //当前所在机场
        if (n->SumTime > min)
            continue;
        if (depn == arr)
        { //如果已经到达目的地了
            if (n->SumTime < min)
            {
                min = n->SumTime;
                strcpy(MinTrace, n->trace);
                trLength = n->length;
                for (int i = 0; i < trLength - 1; i++)
                {
                    MinIDtrace[i] = n->IDtrace[i];
                }
            }
            continue;
        }
        for (int i = 0; i < AirportNum; i++)
        {
            if (g2.arcMatrix[depn][i] && i != dep)
            {
                Arc p = g2.arcMatrix[depn][i];
                Arc minP = NULL; //去掉那些可以但晚的航班
                int minDep = 10000;
                bool flag = 0;
                for (int j = 0; j < n->length; j++)
                {
                    if (i == n->trace[j] - 1)
                    {
                        flag = 1;
                        break;
                    }
                }
                if (!flag)
                    while (p)
                    {
                        if (p->e.DepartureTime > n->NowTime)
                        {
                            if ((p->e.ArrivalTime - n->StartTime) < min)
                            { //不要再trace上出现过
                                if (!minP)
                                {
                                    minDep = p->e.DepartureTime;
                                    minP = p;
                                }
                                else if (p->e.DepartureTime < minDep)
                                {
                                    minDep = p->e.DepartureTime;
                                    minP = p;
                                }
                            }
                        }
                        p = p->next;
                    }
                if (minP)
                {
                    p = minP;
                    NodeforR4 *newNodeforR = (NodeforR4 *)malloc(sizeof(NodeforR4));
                    newNodeforR->length = n->length + 1;
                    newNodeforR->NowTime = p->e.ArrivalTime;
                    newNodeforR->StartTime = n->StartTime;
                    newNodeforR->SumTime = newNodeforR->NowTime - newNodeforR->StartTime;
                    newNodeforR->IDtrace[n->length - 1] = p->e.FlightID;
                    for (int k = 0; k < n->length - 1; k++)
                    {
                        newNodeforR->IDtrace[k] = n->IDtrace[k];
                    }
                    strcpy(newNodeforR->trace, n->trace);
                    newNodeforR->trace[newNodeforR->length - 1] = i + 1;
                    s4.push(newNodeforR);
                }
            }
        }
        delete n;
    }
    cout << "查询完成!" << endl;
    if (min == 10000)
    {
        cout << "无法到达目的地诶" << endl;
        return;
    }
    cout << "最短时间为: " << min / 60 << "h" << min % 60 << "min" << endl;
    cout << "路径为: " << endl ;
    cout << "航班: " ;
    int i = 0;
    for (; i < trLength - 2; i++)
    {
        cout << int(MinIDtrace[i]) << '-';
    }
    cout << int(MinIDtrace[i]) << endl;
    cout << "航班: " << endl;
    for (i = 0; i < trLength - 1; i++)
    {
        cout << "Airplane_ID: " << Flight[int(MinIDtrace[i])].Airplane_ID << "\tFares:" << Flight[int(MinIDtrace[i])].Air_fares << endl;
    }
    cout << "机场: ";
    for (i = 0; i < trLength - 1; i++)
    {
        cout << int(MinTrace[i]) << '-';
    }
    cout << int(MinTrace[i]) << endl;
}

/*仅限直飞或1次中转，求任意两个机场的航线（航班ID顺序表）。*/
void request5(AdjM_graph &g2)
{
    cout << "请输入出发机场与目的机场的序号:" << endl;
    int arr = 0, dep = 0;
    cin >> dep >> arr;
    dep -= 1;
    arr -= 1; //转换为矩阵内部序号
    if (dep == arr)
    {
        cout << "出发与目的地相同，发生错误" << endl;
        return;
    }
    cout << "直飞: " << endl;
    int FlightNumber = 0;
    if (g2.arcMatrix[dep][arr])
    {
        Arc p = g2.arcMatrix[dep][arr];
        for (; p; p = p->next)
            cout << "航线" << ++FlightNumber << ":" << p->e.FlightID << endl;
    }
    if (FlightNumber == 0)
        cout << "无" << endl;
    int FlightNumber2 = FlightNumber;
    cout << "一次中转: " << endl;
    for (int i = 0; i < AirportNum; i++)
    {
        Arc p = g2.arcMatrix[dep][i];
        while (p)
        {
            if (g2.arcMatrix[i][arr])
            {
                Arc q = g2.arcMatrix[i][arr];
                while (q)
                {
                    if (q->e.DepartureTime > p->e.ArrivalTime)
                    {
                        cout << "航线" << ++FlightNumber2 << ":" << p->e.FlightID << "-" << q->e.FlightID << endl;
                    }
                    q = q->next;
                }
            }
            p = p->next;
        }
    }
    if (FlightNumber == FlightNumber2)
        cout << "不存在直飞和一次中转从" << dep + 1 << "到" << arr + 1 << "的路线。" << endl;
}

void Req(Request &r)
{
    r.startTime1 = 0;
    r.startTime2 = 10000;
    r.endTime1 = 0;
    r.endTime2 = 10000;
    Date d;
    for (int i = 0; i < 6; i++)
        r.model[i] = 0;
    cout << "请输入起飞时段:" << endl;
    cout << "开始时间:(格式: 日期 小时 分钟 若无则输入-1 -1 -1)" << endl;
    cin >> d.day >> d.hour >> d.minite;
    if (d.day != -1)
    {
        r.startTime1 = Totime(d);
    }
    cout << "结束时间:(格式: 日期 小时 分钟 若无则输入-1 -1 -1)" << endl;
    cin >> d.day >> d.hour >> d.minite;
    if (d.day != -1)
    {
        r.startTime2 = Totime(d);
    }
    cout << "请输入降落时段:" << endl;
    cout << "开始时间:(格式: 日期 小时 分钟 若无则输入-1 -1 -1)" << endl;
    cin >> d.day >> d.hour >> d.minite;
    if (d.day != -1)
    {
        r.endTime1 = Totime(d);
    }
    cout << "落地时间:(格式: 日期 小时 分钟 若无则输入-1 -1 -1)" << endl;
    cin >> d.day >> d.hour >> d.minite;
    if (d.day != -1)
    {
        r.endTime2 = Totime(d);
    }
    cout << "选择飞机型号(型号1-5): " << endl;
    char c;
    getchar();
    do
    {
        c = getchar();
        if (c < '9' && c >= '0')
        {
            r.model[c - '0'] = 1;
        }
    } while (c != '\n');
}

bool IsReq(Request &r, int mode, Arc a)
{
    if (mode == 0)
    {
        if (a->e.DepartureTime <= r.startTime2 && a->e.DepartureTime >= r.startTime1)
            if (r.model[a->e.AirplaneModel])
                return 1;
    }
    if (mode == 2)
    {
        if (a->e.ArrivalTime <= r.endTime2 && a->e.ArrivalTime >= r.endTime1)
            if (r.model[a->e.AirplaneModel])
                return 1;
    }
    return 0;
}

/*给定起飞时段或者降落时段或者机型要求，求任意两个机场的多个备选航线（航班ID顺序表）。*/
void request6(AdjM_graph &g2)
{
    cout << "请输入出发机场与目的机场的序号:";
    int arr = 0, dep = 0;
    cin >> dep >> arr;
    dep -= 1;
    arr -= 1; //转换为矩阵内部序号
    if (dep == arr)
    {
        cout << "出发与目的地相同，发生错误" << endl;
        return;
    }

    Request r;
    Req(r);

    cout << "查询中..." << endl;
    int FlightNum = 0;
    Arc p = g2.arcMatrix[dep][arr];
    //查询直飞路线
    cout << "直飞航线:" << endl;
    int flag = 0;
    while (p)
    {
        if (IsReq(r, 0, p) && IsReq(r, 2, p))
        {
            cout << "航线" << ++FlightNum << ':' << p->e.FlightID << endl;
        }
        p = p->next;
    }
    if (flag == 0)
        cout << "无" << endl;
    cout << "一次转机航线:" << endl;
    flag = 0;
    for (int i = 0; i < AirportNum; i++)
    {
        if (g2.arcMatrix[dep][i] && i != arr && i != dep)
        {
            Arc p = g2.arcMatrix[dep][i];
            if (g2.arcMatrix[i][arr])
            {
                Arc q = g2.arcMatrix[i][arr];
                while (p)
                {
                    if (IsReq(r, 0, p) && IsReq(r, 2, p))
                    {
                        while (q)
                        {
                            if (IsReq(r, 0, q) && IsReq(r, 2, q) && p->e.ArrivalTime < q->e.DepartureTime)
                            {
                                cout << "航线" << ++FlightNum << ':' << p->e.FlightID << '-' << q->e.FlightID << endl;
                                flag = 1;
                            }
                            q = q->next;
                        }
                    }
                    p = p->next;
                }
            }
        }
    }
    if (flag == 0)
        cout << "无" << endl;
    cout << "两次转机航线:" << endl;
    flag = 0;
    for (int i = 0; i < AirportNum; i++)
    {
        if (g2.arcMatrix[dep][i] && i != arr && i != dep)
        {
            Arc p = g2.arcMatrix[dep][i];
            for (int j = 0; j != i && j != arr && j != dep; j++)
            {
                Arc q = g2.arcMatrix[i][j];
                if (g2.arcMatrix[j][arr])
                {
                    Arc k = g2.arcMatrix[j][arr];
                    while (p)
                    {
                        if (IsReq(r, 0, p) && IsReq(r, 2, p))
                        {
                            while (q)
                            {
                                if (IsReq(r, 0, q) && IsReq(r, 2, q) && p->e.ArrivalTime < q->e.DepartureTime)
                                {
                                    while (k)
                                    {
                                        if (IsReq(r, 0, k) && IsReq(r, 2, k) && q->e.ArrivalTime < k->e.DepartureTime)
                                        {
                                            cout << "航线" << ++FlightNum << ':' << p->e.FlightID << '-' << q->e.FlightID << '-' << k->e.FlightID << endl;
                                            flag = 1;
                                        }
                                        k = k->next;
                                    }
                                }
                                q = q->next;
                            }
                        }
                        p = p->next;
                    }
                }
            }
        }
    }
    if (flag == 0)
        cout << "无" << endl;
}

/*7.给定起飞时段或者降落时段或者机型要求，求任意两个机场之间的航费（机票价格）最低的路径。*/
void request7(AdjM_graph &g2)
{
    cout << "请输入出发机场和降落机场的序号:" << endl;
    int arr = 0, dep = 0;
    cin >> dep >> arr;
    dep -= 1;
    arr -= 1; //转换为矩阵内部序号
    if (dep == arr)
    {
        cout << "出发与目的地相同，发生错误" << endl;
        return;
    }

    Request r;
    Req(r);
    cout << "查询中..." << endl;
    int minAirFares = 1000000;
    int sumAirFares = 0;
    int minID1;
    int minID2[2];
    int minID3[3];
    int trLength = 0;
    Arc p = g2.arcMatrix[dep][arr];
    //搜直飞
    while (p)
    {
        if (IsReq(r, 0, p) && IsReq(r, 2, p))
        {
            sumAirFares = p->e.AirFares;
            if (sumAirFares < minAirFares)
            {
                minAirFares = sumAirFares;
                trLength = 1;
                minID1 = p->e.FlightID;
            }
        }
        p = p->next;
    }
    sumAirFares = 0;
    //搜一次转机
    for (int i = 0; i < AirportNum; i++)
    {
        if (g2.arcMatrix[dep][i] && i != arr && i != dep)
        {
            Arc p = g2.arcMatrix[dep][i];
            if (g2.arcMatrix[i][arr])
            {
                Arc q = g2.arcMatrix[i][arr];
                while (p)
                {
                    if (IsReq(r, 0, p) && IsReq(r, 2, p))
                    {
                        while (q)
                        {
                            if (IsReq(r, 0, q) && IsReq(r, 2, q) && p->e.ArrivalTime < q->e.DepartureTime)
                            {
                                sumAirFares = p->e.AirFares + q->e.AirFares;
                                if (sumAirFares < minAirFares)
                                {
                                    minAirFares = sumAirFares;
                                    trLength = 2;
                                    minID2[0] = p->e.FlightID;
                                    minID2[1] = q->e.FlightID;
                                }
                            }
                            q = q->next;
                        }
                    }
                    p = p->next;
                }
            }
        }
    }
    sumAirFares = 0;
    //搜两次转机
    for (int i = 0; i < AirportNum; i++)
    {
        if (g2.arcMatrix[dep][i] && i != arr && i != dep)
        {
            Arc p = g2.arcMatrix[dep][i];
            for (int j = 0; j != i && j != arr && j != dep; j++)
            {
                Arc q = g2.arcMatrix[i][j];
                if (g2.arcMatrix[j][arr])
                {
                    Arc k = g2.arcMatrix[j][arr];
                    while (p)
                    {
                        if (IsReq(r, 0, p) && IsReq(r, 2, p))
                        {
                            while (q)
                            {
                                if (IsReq(r, 0, q) && IsReq(r, 2, q) && p->e.ArrivalTime < q->e.DepartureTime)
                                {
                                    while (k)
                                    {
                                        if (IsReq(r, 0, k) && IsReq(r, 2, k) && q->e.ArrivalTime < k->e.DepartureTime)
                                        {
                                            sumAirFares = p->e.AirFares + q->e.AirFares + k->e.AirFares;
                                            if (sumAirFares < minAirFares)
                                            {
                                                minAirFares = sumAirFares;
                                                trLength = 3;
                                                minID3[0] = p->e.FlightID;
                                                minID3[1] = q->e.FlightID;
                                                minID3[2] = k->e.FlightID;
                                            }
                                        }
                                        k = k->next;
                                    }
                                }
                                q = q->next;
                            }
                        }
                        p = p->next;
                    }
                }
            }
        }
    }
    if (trLength == 0)
        cout << "抱歉,根据设置的条件，不存在路线到达目的地。" << endl;
    else if (trLength == 1)
    {
        cout << "航线:" << minID1 << endl;
        cout << "最少费用:" << minAirFares << endl;
    }
    else if (trLength == 2)
    {
        cout << "航线:" << minID2[0] << '-' << minID2[1] << endl;
        cout << "最少费用:" << minAirFares << endl;
    }
    else if (trLength == 3)
    {
        cout << "航线:" << minID3[0] << '-' << minID3[1] << '-' << minID3[2] << endl;
        cout << "最少费用:" << minAirFares << endl;
    }
}

/*8.给定飞行时长或者中转时间限制，求任意两个机场的多个备选航线（航班ID顺序表）。*/
void request8(AdjM_graph &g2)
{
    cout << "请分别输入出发机场与目的机场的序号:\n";
    int arr = 0, dep = 0;
    cin >> dep >> arr;
    dep -= 1;
    arr -= 1; //转换为矩阵内部序号
    if (dep == arr)
    {
        cout << "出发与目的地相同，发生错误";
        return;
    }
    int TravelLimit1 = 0;
    int TravelLimit2 = 10000;
    int min = 10000;
    int FlightNum = 0;
    cout << "请依次输入飞行时长的上界与中转时间的下界与上界(无则输入0与10000)(请尽可能强力，不然解集会非常大)\n";
    cin >> min >> TravelLimit1 >> TravelLimit2;
    int BlackFlameMaster = min;
    cout << "加载中..." << endl;
    stack<NodeforR4 *> s4;
    for (int i = 0; i < AirportNum; i++)
    { //初始结点入栈
        if (g2.arcMatrix[dep][i])
        {
            Arc p = g2.arcMatrix[dep][i];
            while (p)
            {
                if (i == arr)
                {
                    if (p->e.ArrivalTime - p->e.DepartureTime < min)
                    {
                        cout << "航线" << ++FlightNum << ":" << p->e.FlightID << endl;
                    }
                    p = p->next;
                    continue;
                }
                NodeforR4 *node = (NodeforR4 *)malloc(sizeof(NodeforR4));
                node->length = 2;
                node->StartTime = p->e.DepartureTime;
                node->SumTime = p->e.ArrivalTime - node->StartTime;
                node->NowTime = p->e.ArrivalTime;
                node->trace[0] = dep + 1;
                node->trace[1] = i + 1;
                node->trace[69] = 0;
                node->IDtrace[0] = p->e.FlightID;
                if (node->SumTime < min)
                    ;
                s4.push(node);
                p = p->next;
            }
        }
    }
    cout << "加载中..." << endl;
    while (!s4.empty())
    { //可以继续剪枝
        NodeforR4 *n = s4.top();
        s4.pop();
        int depn = n->trace[n->length - 1] - 1;
        if (n->SumTime > min)
            continue;
        if (depn == arr)
        {
            if (n->SumTime < min)
            {
                cout << "航线" << ++FlightNum << ":";
                int i = 0;
                for (i = 0; i < n->length - 2; i++)
                {
                    cout << n->IDtrace[i] << " - ";
                }
                cout << n->IDtrace[i] << endl;
            }
            continue;
        }
        for (int i = 0; i < AirportNum; i++)
        {
            if (g2.arcMatrix[depn][i] && i != dep)
            {
                Arc p = g2.arcMatrix[depn][i];
                bool IsTh = 0;
                for (int j = 0; j < n->length; j++)
                {
                    if (i == n->trace[j] - 1)
                    {
                        IsTh = 1;
                        break;
                    }
                }
                if (!IsTh)
                    while (p)
                    {
                        if ((p->e.DepartureTime - n->NowTime) >= TravelLimit1 && (p->e.DepartureTime - n->NowTime) <= TravelLimit2)
                        {
                            if ((p->e.ArrivalTime - n->StartTime) < min)
                                if (p)
                                {
                                    NodeforR4 *newNode = (NodeforR4 *)malloc(sizeof(NodeforR4));
                                    newNode->length = n->length + 1;
                                    newNode->NowTime = p->e.ArrivalTime;
                                    newNode->StartTime = n->StartTime;
                                    newNode->SumTime = newNode->NowTime - newNode->StartTime;
                                    newNode->IDtrace[n->length - 1] = p->e.FlightID;
                                    for (int k = 0; k < n->length - 1; k++)
                                    {
                                        newNode->IDtrace[k] = n->IDtrace[k];
                                    }
                                    strcpy(newNode->trace, n->trace);
                                    newNode->trace[newNode->length - 1] = i + 1;
                                    s4.push(newNode);
                                }
                        }
                        p = p->next;
                    }
            }
        }
        delete n;
    }
    cout << "加载完成!" << endl;
    if (!FlightNum)
    {
        cout << "无法到达目的地" << endl;
        return;
    }
}

int main()
{
    Flight Flight[2350];
    Adj_graph g1;
    AdjM_graph g2;
    test() ;
    init(Flight);
    init(g1, Flight);
    init(g2, Flight);
    TraverseDFS(g1) ;
    // // cout << "\n\n\n" << endl ;
    //TraverseBFS(g1) ;
    //request3(g2) ;
    // request4(g2 , Flight) ;
    //request5(g2) ;
    // request6(g2) ;
    // request7(g2);
    // request8(g2);
}
