#define _CRT_SECURE_NO_DEPRECATE

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <cstdlib>

using namespace std;

auto normalize_character(char c) -> char;

class frequencies {
public:
	frequencies() : counts(26) {

	}

	frequencies(frequencies&& other): counts(move(other.counts)) {
	}

	frequencies(const frequencies& other) : counts(other.counts) {
	}

	void add(char c) {
		c = normalize_character(c);
		if (c == '.') return;
		counts[c - 'a'] ++;
	}

	void add_word(const string& word) {
		for (auto c : word) {
			add(c);
		}
	}

	auto operator- (const frequencies& other) const -> frequencies {
		frequencies out;
		for (size_t i = 0; i < counts.size(); i++) {
			out.counts[i] = counts[i] - other.counts[i];
		}
		return out;
	}

	auto operator== (const frequencies& other) const -> bool {
		return counts == other.counts;
	}

	auto operator< (const frequencies& other) const -> bool {
		for (size_t i = 0; i < counts.size(); i++) {
			if (counts[i] > other.counts[i]) return false;
			else if (counts[i] < other.counts[i]) return true;
		}
		return false; 
	}

	auto has_at_least_frequencies_for(const frequencies& other) const -> bool {
		for (size_t i = 0; i < counts.size(); i++) {
			if (counts[i] < other.counts[i]) return false;
		}
		return true;	
	}

private:
	vector<int> counts;
};

auto read_words() -> set<string> {
	set<string> words;
	ifstream is("nl_NL.txt");
	while (is.good()) {
		string line;
		getline(is, line);
		string word;
		for (auto c : line) {
			auto valid = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
			if (!valid) break;
			word += c;
		}
		words.insert(word);
	}
	return words;
}

auto normalize_character(char c) -> char {
	if (c >= 'a' && c <= 'z') return c;
	if (c >= 'A' && c <= 'Z') return static_cast<char>(0 + c - 'A' + 'a');
	return '.';
}


int main(int argc, char** argv) {
	cout << "Content-Type: text/plain\n\n";

	const char* query_string = getenv("QUERY_STRING");
	if (query_string == nullptr) query_string = "";
	auto words = read_words();
	
	const string original_word(query_string);
	ofstream out("out.txt");

	frequencies frequencies_original;
	frequencies_original.add_word(original_word );

	map<frequencies, vector<string>> words_by_frequencies;

	for (auto& word : words) {
		frequencies frequencies_of_word;
		frequencies_of_word.add_word(word);
		words_by_frequencies[move(frequencies_of_word)].push_back(word);
	}

	for (auto& word1 : words) {
		frequencies frequencies1;
		frequencies1.add_word(word1);
		if (frequencies_original == frequencies1) {
			cout << word1 << endl;
		}
		else if (frequencies_original.has_at_least_frequencies_for(frequencies1)) {
			const auto frequencies_remaining = frequencies_original - frequencies1;
			const auto& it = words_by_frequencies.find(frequencies_remaining);
			if (it == words_by_frequencies.end()) continue;
			for (auto& word2 : it->second) {
				cout << word1 << " " << word2 << endl;
				out << word1 << " " << word2 << endl;
			}
		}
	}
}
