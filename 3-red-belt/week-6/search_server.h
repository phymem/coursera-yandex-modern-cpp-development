#pragma once

#include <unordered_map>
#include <istream>
#include <ostream>
#include <vector>
#include <string>
#include <future>
#include <mutex>

using namespace std;

typedef unsigned int docid_t;
typedef pair<unsigned int, unsigned int> uint_pair_t;

class InvertedIndex {
public:

	InvertedIndex() { index.reserve(10000); }

	void Add(const string& document);

	vector<uint_pair_t> Lookup(const string& word) const {
		if (auto it = index.find(word); it != end(index)) {
			return it->second;
		}
		return {};
	}

private:

	unsigned int num_docs = 0;
	unordered_map<string, vector<uint_pair_t>> index;
};

class SearchServer {
public:

	SearchServer();

	explicit SearchServer(istream& document_input);

	void UpdateDocumentBase(istream& document_input);
	void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:

	void UpdateDocumentBaseSingleThreaded(istream& document_input);
	void AddQueriesStreamSingleThreaded(istream& query_input, ostream& search_results_output);

private:

	mutex index_mutex;
	mutex output_mutex;

	InvertedIndex index;

	bool first_update_completed;
	future<void> update_thread;

	vector<future<void>> query_threads;
};
