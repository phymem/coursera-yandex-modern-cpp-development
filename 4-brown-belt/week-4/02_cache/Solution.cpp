#include "Common.h"

#include <algorithm>
#include <mutex>
#include <list>

using namespace std;

class LruCache : public ICache {
public:

	LruCache(shared_ptr<IBooksUnpacker> books_unpacker, const Settings& settings) :
		books_unpacker_(books_unpacker),
		max_memory_(settings.max_memory),
		free_memory_(settings.max_memory) {}

	BookPtr GetBook(const string& book_name) override {
		lock_guard<mutex> guard(mutex_);

		auto it = find_if(cache_.begin(), cache_.end(),
			[&book_name](const BookPtr& book) { return book->GetName() == book_name; });

		if (it != cache_.end()) {
			cache_.push_front(*it);
			cache_.erase(it);
		}
		else {
			BookPtr book = books_unpacker_->UnpackBook(book_name);
			size_t book_memory = book->GetContent().length();

			if (max_memory_ < book_memory)
				return book;

			while (free_memory_ < book_memory) {
				free_memory_ += cache_.back()->GetContent().length();
				cache_.pop_back();
			}

			free_memory_ -= book_memory;
			cache_.push_front(book);
		}

		return cache_.front();
	}

private:

	shared_ptr<IBooksUnpacker> books_unpacker_;
	size_t max_memory_;
	size_t free_memory_;
	list<BookPtr> cache_;
	mutex mutex_;
};


unique_ptr<ICache> MakeCache(
	shared_ptr<IBooksUnpacker> books_unpacker,
	const ICache::Settings& settings
) {
	return make_unique<LruCache>(books_unpacker, settings);
}
