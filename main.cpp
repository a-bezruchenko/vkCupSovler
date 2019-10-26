#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <list>
#include <tuple>
#include <string>
#include <array>
#include <stack>

using namespace std;

struct coord
{
	coord(int x, int y) { coord::x = x; coord::y = y;}
	coord() {};
	int x, y;
	bool operator==(coord& o) {return x==o.x && y==o.y;}
	int operator-(coord& o) {return abs(x-o.x) + abs(y-o.y);}
};

struct action
{
	int x,y,a;
	action(int x, int y, int a) { action::x = x; action::y = y;action::a=a;}
	action(int x, int y) { action::x = x; action::y = y;a=0;}
	action(coord o) { x=o.x; y=o.y; a=0;}
	action(coord o, int a) { x=o.x; y=o.y; action::a=a;}
	action() {};
	bool operator==(action& o) {return x==o.x && y==o.y && a==o.a;}
	coord toCoord() {return coord(x,y);}
};

struct request
{
	// начало и конец
	coord a, b;
	// номер
	int n;
};

struct state
{
	state() {};
	state(coord xy, int t, int p) {c=xy;time=t;pass=p;};
	coord c;
	int time;
	int pass;
};

int main()
{
	int w, h, k;
	scanf("%d %d", &w, &h);
	scanf("%d", &k);
	//cin >> w >> h;
	//cin >> k;
	// текущие координаты такси
	vector<coord> taxi(k);
	// количество пассажиров в каждом такси
	vector<int> taxiPass(k);
	// приказы на перемещение, отданные такси
	vector<list<action>> taxiOrders(k);
	// состояние такси
	vector<list<state>> taxiStates(k);

	for (int i = 0; i < k; i++)
	{
		scanf("%d %d", &taxi[i].x , &taxi[i].y);
		//cin >> taxi[i].x >> taxi[i].y;
		taxiPass[i] = 0;
	}
	int currentTime = 0;

	// назначаем каждому такси место, к которому он будет двигаться в отсутствие заказов
	// TODO: назначать такси в зависимости от их начального расположения
	// TODO: выгодней разбить на квадраты
	vector<coord>taxiStation(k);

	if (w>=h)
	{
		for (int i = 0; i < k; i++)
		{
			taxiStation[i].x = (w/k)*i + (w/k)/2;
			taxiStation[i].y = h/2;
		}
	}
	else
	{
		for (int i = 0; i < k; i++)
		{
			taxiStation[i].x = w/2;
			taxiStation[i].y = (h/k)*i + (h/k)/2;
		}
	}

	// послать инструкции
	printf("%d ", k);
	//cout << k << ' ';
	fflush(stdout);
	for (int i = 0; i < k; i++)
	{
		taxiStates[i].push_back(state(taxiStation[i], taxiStation[i]-taxi[i], 0));
		taxiOrders[i].push_back(action(taxiStation[i]));
		printf("\n%d %d ", i+1, 1);
		//cout << i+1 << ' ' << 1 << ' ';
		fflush(stdout);
		printf("%d %d %d ", taxiStation[i].x, taxiStation[i].y, 0);
		//cout << taxiStation[i].x << ' ' << taxiStation[i].y << ' ' << 0 << ' ';
		fflush(stdout);
	}

	list<request> reqList;
	bool cont = true;
	int tj;
	int lastReq = 1;
	while (true)
	{
		request newReq;
		scanf("%d %d %d %d %d", &tj, &newReq.a.x, &newReq.a.y, &newReq.b.x, &newReq.b.y);
		//cin >> tj;
		//cin >> newReq.a.x >> newReq.a.y >> newReq.b.x >> newReq.b.y;
		newReq.n = lastReq;
		lastReq++;
		if (tj==-1)
			break;

		// обновляем текущие координаты для такси
		for (int i = 0; i < k; i++)
		{
			if (taxiOrders[i].empty())
				continue;
			
			coord cur = taxi[i];
			int movementLeft = tj - currentTime;
			while (!taxiOrders[i].empty() && movementLeft)
			{
				coord t = taxiOrders[i].front().toCoord();
				if (abs(t.x - cur.x) >= movementLeft)
				{
					if (cur.x < t.x)
						cur.x += movementLeft;
					else
						cur.x -= movementLeft;

					movementLeft = 0;
				}
				else
				{
					movementLeft -= t.x - cur.x;
					cur.x = t.x;
					if (t==taxiStates[i].front().c)
						taxiStates[i].pop_front();
					if (abs(t.y - cur.y) >= movementLeft)
					{
						if (cur.y < t.y)
							cur.y += movementLeft;
						else
							cur.y -= movementLeft;

						movementLeft = 0;
					}
					else
					{
						movementLeft -= t.y - cur.y;
						cur.y = t.y;
						if (t==taxiStates[i].front().c)
							taxiStates[i].pop_front();
					}
				}
				
				if (t==cur)
				{
					if (taxiOrders[i].front().a>0)
						taxiPass[i]++;
					if (taxiOrders[i].front().a<0)
						taxiPass[i]--;
					taxiOrders[i].pop_front();
				}
			}
		}
		currentTime = tj;
		// конец обновления текущих координат

		// здесь идёт обработка нового заказа
		int minTaxi = 0;
		int minDistance = taxiStates[0].front().c - newReq.a + taxiStates[0].front().time-currentTime; 
		for (int i = 0; i < k; i++)
		{
			int tempDist = taxiStates[i].front().c - newReq.a + taxiStates[i].front().time-currentTime;
			if (taxiPass[i]==0 && tempDist < minDistance)
			{
				minDistance = tempDist;
				minTaxi = i;
			}
		}


		if (!taxiOrders[minTaxi].empty())
			taxiOrders[minTaxi].pop_back();

		if (!taxiStates[minTaxi].empty())
			taxiStates[minTaxi].pop_back();

		taxiOrders[minTaxi].push_back(action(newReq.a, newReq.n));
		taxiOrders[minTaxi].push_back(action(newReq.b, -newReq.n));
		taxiOrders[minTaxi].push_back(action(taxiStation[minTaxi]));

		taxiStates[minTaxi].push_back(state(coord(newReq.a.x, taxi[minTaxi].y), currentTime+(newReq.a.x-taxi[minTaxi].x), 1));
		taxiStates[minTaxi].push_back(state(newReq.a, currentTime+(newReq.a-taxi[minTaxi]), 1));
		taxiStates[minTaxi].push_back(state(coord(newReq.b.x, newReq.a.y), currentTime+(newReq.a-taxi[minTaxi])+(newReq.a.x-newReq.b.x), 1));
		taxiStates[minTaxi].push_back(state(newReq.b, currentTime+(newReq.a-taxi[minTaxi])+(newReq.b-newReq.a), 0));
		taxiStates[minTaxi].push_back(state(coord(taxiStation[minTaxi].x, newReq.b.y), currentTime+(newReq.a-taxi[minTaxi])+(newReq.b-newReq.a)+(taxiStation[minTaxi].x-newReq.b.x), 1));
		taxiStates[minTaxi].push_back(state(taxiStation[minTaxi], currentTime+(newReq.a-taxi[minTaxi])+(newReq.b-newReq.a)+(taxiStation[minTaxi]-newReq.b), 0));
		
		// здесь кончается обработка нового заказа

		// здесь начинается выдача приказов
		printf("1");
		//cout << 1 << endl;
		fflush(stdout);
		printf("\n%d %d ", minTaxi+1, taxiOrders.size());
		//cout << minTaxi+1 << ' ' << taxiOrders.size() << ' ';
		fflush(stdout);
		for (auto i = taxiOrders[minTaxi].begin(); i != taxiOrders[minTaxi].end(); i++)
		{
			printf("%d %d %d ", i->x, i->y, i->a);
			//cout << i->x << ' ' << i->y << ' ' << i->a << ' ';
			fflush(stdout);
		}

		
		// здесь кончается выдача приказов
	}
	printf("0");
	//cout << 0;
	fflush(stdout);
	return 0;
}
