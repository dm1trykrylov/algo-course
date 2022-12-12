#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
using namespace std;
long long n, x, y, t, k, m, prx, pry, s, cnt, cnt1, curx, cury;
const int length = 1e5 + 1;
class vec {
public:
	long long x, y;
	vec() {
		x = 0;
		y = 0;
	}
	vec(long long x1, long long y1) {
		x = x1;
		y = y1;
	}
	vec& operator-=(vec b) {
		x -= b.x;
		y -= b.y;
		return *this;
	}
	friend long long operator%(vec b, vec a) {
		vec c;
		
		c.x = b.x * a.y;
		c.y = b.y * a.x;
		return c.x - c.y;
	}
	friend vec operator-(vec a, vec b) {
		return a -= b;
	}
};
vec pts[length];
int main() {
#ifdef _DEBUG
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
#endif
	cin >> n;
	cnt1 = n;
	for (int i = 0; i < cnt1; ++i) {
		cin >> curx >> cury;
		if (i) {
			pts[i - 1].x = curx - prx;
			pts[i - 1].y = cury - pry;
		}
		else {
			k = curx;
			m = cury;
		}
		prx = curx;
		pry = cury;
	}
	pts[n - 1].x = k - curx;
	pts[n - 1].y = m - cury;
	for (int i = 0; i < n - 1; ++i) {
		t = pts[i] % pts[i + 1];
//		cout << t << "\n"[i < n - 2];
		if (t > 0)
			++cnt;
		else if (t == 0)
			--cnt1;
	}
	t = pts[n - 1] % pts[0];
	if (t > 0)
		++cnt;
	else if (t == 0)
		--cnt1;
	if (!cnt || cnt == n || cnt == cnt1)
		cout << "YES";
	else
		cout << "NO";
	return 0;
}