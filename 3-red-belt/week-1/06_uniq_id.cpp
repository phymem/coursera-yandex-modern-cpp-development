#include <string>
#include <vector>
using namespace std;

#define UNIQ_ID_CONCATE(line) id##line
#define UNIQ_ID_EXPAND_CONCATE(line) UNIQ_ID_CONCATE(line)

#define UNIQ_ID UNIQ_ID_EXPAND_CONCATE(__LINE__)

int main() {
	int UNIQ_ID = 0;
	string UNIQ_ID = "hello";
	vector<string> UNIQ_ID = {"hello", "world"};
	vector<int> UNIQ_ID = {1, 2, 3, 4};
}
