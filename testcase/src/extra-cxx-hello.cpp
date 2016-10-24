#include "trap.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;
int a[] = {5, 2, 7, 4, 1, 3, 8, 6};
int alen = sizeof(a) / sizeof(a[0]);
int main()
{
    cout << "Hello World from C++!" << endl;

    map<int, int> mymap;
    for (int i = 0; i < alen; i++)
        mymap.insert(make_pair(a[i], i));
    for (map<int, int>::iterator it = mymap.begin(); it != mymap.end(); it++) {
        cout << "mymap: (" << it->first << ", " << it->second << ")" << endl;
    }

    vector<double> myvct;
    myvct.push_back(0.15); myvct.push_back(0.30); myvct.push_back(1.59);
    myvct.push_back(1e200); myvct.push_back(1e-100); myvct.push_back(-3.14);
    sort(myvct.begin(), myvct.end());
    for (vector<double>::iterator it = myvct.begin(); it != myvct.end(); it++) {
        cout << "myvct: " << *it << endl;
    }

    string mystr;
    for (int r = 0; r < 3; r++) {
        for (int i = 'a'; i <= 'g'; i++) {
            mystr += i;
        }
    }
    cout << "mystr: " << mystr << endl;

    cout << setprecision(15) << fixed;
    cout << "sqrt(10)  = " << sqrt(10) << endl;
    cout << "log10(2)  = " << log(2) / log(10) << endl;
    cout << "fabs(-5)  = " << fabs(-5) << endl;
    cout << "sin(PI/4) = " << sin(M_PI / 4) << endl;
    cout << "sqrt(2)/2 = " << sqrt(2) / 2 << endl;

    HIT_GOOD_TRAP;
    return 0;
}
