#include"MD5.h"
#include<iostream>
using namespace std;

int main() {
    string s;
    //cin >> s;
    getline(cin,s);
    MD5 md5(s);
    string result = md5.get_encode();
    cout << result << endl;
    return 0;
}