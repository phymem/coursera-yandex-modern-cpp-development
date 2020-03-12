#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <future>
#include <map>

#include "profile.h"

#define SINGLE_THREADED

template <typename Func>
void ForEachWord(const string& line, Func func) {
	string::size_type alpha_pos = 0;
	string::size_type space_pos = 0;
	while ((alpha_pos = line.find_first_not_of(' ', space_pos)) < line.size()) {
		space_pos = min(line.find_first_of(' ', alpha_pos), line.size());
		func(line.substr(alpha_pos, space_pos - alpha_pos));
	}
}

SearchServer::SearchServer() :
	first_update_completed(false)
{
}

SearchServer::SearchServer(istream& document_input) :
	first_update_completed(false)
{
	UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
	auto kernel = [this, &document_input]() {
		UpdateDocumentBaseSingleThreaded(document_input);
	};

	update_thread = async(kernel);
#ifdef SINGLE_THREADED
	update_thread.wait();
#endif
}

void SearchServer::UpdateDocumentBaseSingleThreaded(istream& document_input) {
	InvertedIndex new_index;

	for (string current_document; getline(document_input, current_document); ) {
		new_index.Add(move(current_document));
	}

	lock_guard guard(index_mutex);
	index = move(new_index);
	first_update_completed = true;
}

void SearchServer::AddQueriesStream(
	istream& query_input, ostream& search_results_output
) {
	if (!first_update_completed) {
		update_thread.wait();
	}

	auto kernel = [this, &query_input, &search_results_output]() {
		AddQueriesStreamSingleThreaded(query_input, search_results_output);
	};

	query_threads.push_back(async(kernel));
#ifdef SINGLE_THREADED
	query_threads.back().wait();
#endif
}

void SearchServer::AddQueriesStreamSingleThreaded(
	istream& query_input, ostream& search_results_output
) {
	vector<unsigned int> docid_count;

	for (string query; getline(query_input, query); ) {

		docid_count.assign(docid_count.size(), 0);

		{
			lock_guard guard(index_mutex);

			ForEachWord(query,
				[&docid_count, this](const string& word) {
					for (const auto& p : index.Lookup(word)) {
						if (docid_count.size() <= p.first) {
							docid_count.resize(p.first + 1);
						}
						docid_count[p.first] += p.second;
					}
				}
			);
		}

		const int NUM_SEARCH_RESULTS = 5;

		vector<uint_pair_t> search_results;
		for (docid_t docid = 0; docid < docid_count.size(); ++docid) {
			if (auto count = docid_count[docid]) {
				search_results.emplace_back(make_pair(docid, count));
			}
		}

		partial_sort(search_results.begin(),
			(search_results.size() > NUM_SEARCH_RESULTS)
				? (search_results.begin() + NUM_SEARCH_RESULTS) : search_results.end(),
			search_results.end(),
			[](const uint_pair_t& lhs, const uint_pair_t& rhs) {
				// hit count
				if (lhs.second != rhs.second) {
					return lhs.second > rhs.second;
				}
				// docid
				return lhs.first < rhs.first;
			}
		);

		{
			lock_guard quard(output_mutex);

			search_results_output << query << ':';
			for (const auto& result : Head(search_results, 5)) {
				search_results_output << " {"
					<< "docid: " << result.first << ", "
					<< "hitcount: " << result.second << '}';
			}
			search_results_output << endl;
		}
	}
}

void InvertedIndex::Add(const string& document) {
	ForEachWord(document,
		[docid = num_docs++, this](string word) {
			auto it = index.find(word);
			if (it == end(index)) {
				auto& vec = index[move(word)];
				vec.reserve(100);
				vec.push_back(uint_pair_t(docid, 1));
				return;
			}
			for (auto& p : it->second) {
				if (p.first == docid) {
					p.second++;
					return;
				}
			}
			it->second.push_back(uint_pair_t(docid, 1));
		}
	);
}
