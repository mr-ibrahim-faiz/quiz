#ifndef RESUME_H
#define RESUME_H

#include<vector>
using std::vector;

// user-defined type Resume
struct Resume {
	// default constructor
	Resume() noexcept
		: position{}
		, indexes{}
		, retry_indexes{}{}
	
	// constructor taking position, indexes and retry_indexes as arguments
	explicit Resume(const size_t& position, const vector<size_t>& indexes, const vector<size_t>& retry_indexes) noexcept
		: position { position }
		, indexes { indexes }
		, retry_indexes { retry_indexes }{}

	// resume information
	size_t position; // position of the current question (equals question_index + 1)
	vector<size_t> indexes; // order of the questions
	vector<size_t> retry_indexes; // questions to retry
};

#endif
