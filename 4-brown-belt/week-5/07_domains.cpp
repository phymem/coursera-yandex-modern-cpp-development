#include <unordered_set>
#include <iostream>
#include <string>

using namespace std;

unordered_set<string> banned_domains;

bool is_domain_banned(const unordered_set<string>& banned, const string& domain)
{
	auto it = banned.find(domain);
	if (it != banned.end())
		return true;

	string::size_type pos = -1;
	while ((pos = domain.find_first_of('.', pos + 1)) != string::npos) {
		if (banned.find(domain.substr(pos + 1)) != banned.end())
			return true;
	}

	return false;
}

int main() {

	int num;
	cin >> num;
	while (num--) {
		string line;
		cin >> line;

		banned_domains.insert(move(line));
	}

	cin >> num;
	while (num--) {
		string line;
		cin >> line;

		cout << (is_domain_banned(banned_domains, line) ? "Bad\n" : "Good\n");
	}

	return 0;
}
