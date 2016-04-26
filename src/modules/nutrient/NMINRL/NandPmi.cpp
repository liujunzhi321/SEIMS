/*!
 * \file NandPmi.cpp
 * \ingroup NMINRL
 * \author Huiran Gao
 * \date April 2016
 */

#include <cmath>
#include <iostream>

using namespace std;

double maxnum(double x, double y);
int main() {
	double a, b, c;
	cout << "Input two numebers:\n";
	cin >> a >> b;
	c = maxnum(a, b);
	cout << "SPRT(" << c << ") = " << sqrt(c);
	system("pause");
}
double maxnum(double x, double y) {
	if(x > y) {
		return x;
	} else {
		return y;
	}
}