//#define DBG

#ifdef DBG
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
using namespace std;
#endif

class SortedStrings {
public:
	void AddString(const string& s) {
		m_strings.push_back(s);
		sort(m_strings.begin(), m_strings.end());
	}
	vector<string> GetSortedStrings() {
		return m_strings;
	}
private:
	vector<string> m_strings;
};

#ifdef DBG
void PrintSortedStrings(SortedStrings& strings) {
	for (const string& s : strings.GetSortedStrings()) {
		cout << s << " ";
	}
	cout << endl;
}

int main() {

	SortedStrings strings;

	strings.AddString("first");
	strings.AddString("third");
	strings.AddString("second");
	PrintSortedStrings(strings);

	strings.AddString("second");
	PrintSortedStrings(strings);

	return 0;
}
#endif // DBG
