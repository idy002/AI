#include <cstdio>

struct Arr {
	int a[100][100];
	int *operator[]( int i ) {
		return a[i];
	}
	const int *operator[]( int i ) const {
		return a[i];
	}
};

int main() {}
