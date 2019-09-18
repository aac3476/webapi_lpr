#include "pch.h"
#include <iostream>
#include <vlpr.h>

#pragma warning(disable:4996)
using namespace std;
int main() {
	vlpr lpr = vlpr();
	char * result =lpr.sendPic("/identify", "152.32.134.49", "D:\\1.jpg", "1.jpg", 8801, "", "");
	cout << result;
}