#include <iostream>
#include <vector>
#include "SMR.h"
using namespace std;
int main()
{
	int a;
	SMR::run(3000, SMR_LOCAL, 100*SMR_MB);
	SMR::getGlobalMemoryValue();

	// =================== allocate memory ===================
	vector<char*> heap;
	for (int i = 0; i < 100; i++) {
		char* data;
		int bytes = (1024 * 1024);
		data = (char*)malloc(bytes);
		for (int i = 0; i < bytes; i++) {
			data[i] = (char)rand();
		}
		heap.push_back(data);
		heap.clear();
		printf("allocated memory\nlocal is %lf, global is %d%%\n", (double)SMR::getLocalMemoryValue() / (double)SMR_MB, SMR::getGlobalMemoryValue(true));

	}

	cout << "Local memory: " << (double)SMR::getLocalMemoryValue() / (double)SMR_MB << endl;
	cout << "Global memory: " << SMR::getGlobalMemoryValue(true) << endl;
	cout << "Press any key to stop:" << endl;
	cin >> a;
	SMR::stop();
	cout << "Local memory: " << (double)SMR::getLocalMemoryValue() / (double)SMR_MB << endl;
	cout << "Global memory: " << SMR::getGlobalMemoryValue(true) << endl;
	system("pause");
	return 0;
}