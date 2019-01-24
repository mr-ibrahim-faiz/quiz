#include "quiz.h"

#include<iostream>
using std::cin;
using std::cout;
using std::cerr;
using std::ws;
using std::streamsize;

#include<chrono>
using std::chrono::system_clock;

#include<random>
using std::random_device;

#include<algorithm>
using std::count;
using std::shuffle;
using std::random_shuffle;

#include<fstream>
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::ios_base;

#include<stdexcept>
using std::out_of_range;

#include<limits>
using std::numeric_limits;

#include<algorithm>
using std::find;

// retrieves quiz information from files
void get_questions_and_answers(const string& file_address, vector<string>& questions, vector<string>& answers)
// retrieves questions and answers from the questions_answers files
// the file is created if it doesn't exist
{
	ifstream file;

	file.open(file_address);
	if (file.is_open()) {

		string line { "" };

		while (getline(file, line, '$')) {
			questions.push_back(line);
			file.ignore(1, '\n');
			getline(file, line, '$');
			answers.push_back(line);
			file >> ws;
		}
		file.close();
	}
	else {
		cerr << "Error: Unable to open file.\n";
		cout << "Creating the required file...\n\n";
		ofstream file(file_address);
		if(file.is_open()) file.close();
	}
}

// displays main menu
void display_main_menu(const string& resume_file_address) {
	cout << "[1] List Questions\n";
	cout << "[2] " << ((is_quiz_to_resume(resume_file_address)? "Resume Quiz" : "Complete Quiz")) << '\n';

	// gets retry indexes from resume file
	vector<size_t> indexes = get_retry_indexes(resume_file_address);

	if(are_questions_to_practice(resume_file_address)) cout << "[3] Practice (" << indexes.size() << ")\n";

	cout << "[x] Exit\n";
}

// lists and numbers elements of a vector
template<typename T>
void list_elements(const vector<T>& vec)
// lists and numbers elements of a vector vec
{
	for (size_t i = 0; i < vec.size(); ++i) cout << i + 1 << ":" << vec[i] << '\n';
}

template void list_elements<string>(const vector<string>& vec);

// gets a random distribution of unique unsigned integers
vector<size_t> get_random_int_distribution(const size_t& size)
// returns a shuffled vector of unique integers ranging from 0 to size-1
{
	vector<size_t> indexes;
	for (size_t i = 0; i < size; indexes.push_back(i++));
	
	random_device rd;
	shuffle(indexes.begin(), indexes.end(), rd);

	/*
	unsigned seed = (unsigned) system_clock::now().time_since_epoch().count();
	srand(seed);
	random_shuffle(indexes.begin(), indexes.end());
	*/

	return indexes;
}

// copies a file
void copy_file(const string& src_filename, const string& dst_filename)
// copies the source file src_filename into a destination file dst_filename
{
	ifstream src(src_filename, ios::binary);
	if (src.is_open()) {
		ofstream dst(dst_filename, ios::binary);
		if (dst.is_open()) {
			dst << src.rdbuf();
			dst.close();
		}
		src.close();
	}
}

// creates file if it doesn't exit
void create_file_if(const string& file_address)
// create the file if it doesn't exit
{
	ifstream file;
	file.open(file_address);

	if (file.is_open()) file.close();
	else {
		cerr << "Error: Unable to open file.\n";
		cout << "Creating the required file...\n\n";
		ofstream file(file_address);
		if(file.is_open()) file.close();
	}
}

// writes elements of a vector on file
template<typename T>
void write_elements(const vector<T>& vec, const string& file_name, ios_base::openmode flag, const string& delimiter, const string& period)
// write elements of a vector on file_name
// elements are delimited by the delimiter
// the writing is ended by the period
{
	fstream file;
	file.open(file_name, flag);

	if (file.is_open()) {

		size_t size = vec.size();

		if (size == 0)
			file << period;
		else
			for (size_t i = 0; i < size; ++i) {
				file << vec[i] << ((i != size - 1) ? delimiter : period);
			}

		file.close();
	}
	else
		cerr << "Error: Unable to open file.\n";
}

template void write_elements<size_t>(const vector<size_t>& vec, const string& file_name, ios_base::openmode flag, const string& delimiter, const string& period);

// writes a single element on a file
template<typename T>
void write_single_element(const T& t, const string& file_name, ios_base::openmode flag, const string& period)
// write a single element on file_name
// the writing is ended by the period
{
	fstream file;
	file.open(file_name, flag);

	if (file.is_open()) {
		file << t << period;
		file.close();
	}
	else
		cerr << "Error: Unable to open file.\n";
}

template void write_single_element<size_t>(const size_t& t, const string& file_name, ios_base::openmode flag, const string& period);

// fills a file with a given pattern
template<typename T>
void fill_with_pattern(const T& t, const size_t time, const string& file_name, ios_base::openmode flag, const string& period)
// fills a file with a pattern a given number of times
// the writing is ended by the period
{
	fstream file;
	file.open(file_name, flag);

	if (file.is_open()) {
		for (size_t i = 0; i < time; ++i)
			file << t;
		file << period;
		file.close();
	}
	else
		cerr << "Error: Unable to open file.\n";
}

template void fill_with_pattern<string>(const string& t, const size_t time, const string& file_name, ios_base::openmode flag, const string& period);

// sets up resume file
void set_resume_file(const string& resume_file_address, const vector<size_t>& indexes, const vector<size_t>& retry_indexes = vector<size_t>{}) {
	// fills the first line with whitespaces
	fill_with_pattern(" ", 6, resume_file_address, ios::out | ios::trunc, "$\n");

	// saves the question order in resume file
	write_elements(indexes, resume_file_address, ios::app, " ", " $\n");

	// saves retry indexes
	write_elements(retry_indexes, resume_file_address, ios::app, " ", " $\n");
}

// enables user to review failed question
void review() {
	string review { "" };
	getline(cin, review, '$');

	while (cin.peek() != '\n')
		getline(cin, review, '$');

	getchar(); // deals with the newline
}

// checks if there's a quiz to resume
bool is_quiz_to_resume(const string& resume_file_address)
// checks if there's a quiz to resume (i.e. valid question_number in resume file) and returns true or false
{
	bool can_be_resumed { false };

	ifstream file;
	file.open(resume_file_address);

	if (file.is_open()) {
		size_t question_number { 0 };
		if (file >> question_number)
			can_be_resumed = true;
		else {
			file.clear();
			can_be_resumed = false;
		}
		file.close();
	}
	else
		cerr << "Error: Unable to open file.\n";

	return can_be_resumed;
}

// checks if there are questions to practice with
bool are_questions_to_practice(const string& resume_file_address)
// checks if there are questions to practice with (i.e. at least one question in retry_indexes) and returns true or false
{
	// question number to start with
	size_t first_question_index { 0 };

	// vector containing the order by which the questions will appear
	vector<size_t> indexes;

	// vector of questions to retry
	vector<size_t> retry_indexes;

	// gets resume file information
	get_resume_information(resume_file_address, first_question_index, indexes, retry_indexes);

	// checks the size of retry_indexes
	if (retry_indexes.size() > 0)
		return true;

	return false;
}

// gets resume file information
void get_resume_information(string const& resume_file, size_t& current_question
	, vector<size_t>& indexes, vector<size_t>& retry_indexes)
// retrieves current question, questions order index and retry indexes from resume file
{
	ifstream file;
	file.open(resume_file);

	if (file.is_open()) {
		// retrieves current question
		if (file >> current_question)
			--current_question;
		else
			file.clear();
		file.ignore(numeric_limits<streamsize>::max(), '\n');

		// retrieves questions order index
		size_t index { 0 };
		while (file >> index)
			indexes.push_back(index);

		file.clear();
		file.ignore(numeric_limits<streamsize>::max(), '\n');

		// retrieves retry indexes
		while (file >> index)
			retry_indexes.push_back(index);

		file.clear();
		file.ignore(numeric_limits<streamsize>::max(), '\n');

		file.close();
	}
	else
		cerr << "Error: Unable to open file.\n";
}

// gets retry indexes
vector<size_t> get_retry_indexes(const string& resume_file_address)
// retrieves retry indexes from resume file
{
	ifstream file;
	file.open(resume_file_address);

	vector<size_t> retry_indexes;

	if (file.is_open()) {
		// ignores current question
		file.ignore(numeric_limits<streamsize>::max(), '\n');

		// ignores questions order index
		file.ignore(numeric_limits<streamsize>::max(), '\n');

		// retrieves retry indexes
		size_t index { 0 };
		while (file >> index)
			retry_indexes.push_back(index);

		file.clear();
		file.ignore(numeric_limits<streamsize>::max(), '\n');

		file.close();
	}
	else
		cerr << "Error: Unable to open file.\n";

	return retry_indexes;
}

// shuffles vector
template<typename T>
void shuffle_vector(vector<T>& vec) {
	unsigned seed = (unsigned)system_clock::now().time_since_epoch().count();
	srand(seed);
	random_shuffle(vec.begin(), vec.end());
}

template void shuffle_vector<size_t>(vector<size_t>& vec);

// copies line from a file to another
void copy_lines(const string& src_filename, const string& dst_filename, const size_t& n, ios_base::openmode mode = ios::out)
// copies the first n lines from src_filename into dst_filename
{
	fstream src(src_filename, ios::in | ios::binary);
	if(src.is_open()){
		fstream dst(dst_filename, ios::out | ios::binary);
		if(dst.is_open()){
			for(size_t i = 0; i < n; ++i){
				string sinput;
				getline(src, sinput);
				dst << sinput << '\n';
			}
			dst.close();
		}
		src.close();
	}
}

// quiz launcher
void quiz_launcher(const vector<string>& questions, const vector<string>& answers)
// (1) checks the resume file and determines if there's a quiz to resume
// (2) gives to the user the choice to resume quiz or not
// (3) displays a questions, wait for user's answer,
// (4) displays the right answer
// (5) gives the user the choice to retry later
{
	// initializes resume file address
	const string resume_file_address { "resume_quiz.txt" };

	// initializes save file address
	const string save_file { "save.txt" };

	// exit sequence
	const string exit_sequence { "exit" };

	// maximum number of the same question allowed in retry list
	const size_t maximum_number_of_questions { 10 };

	// creates resume file if it doesn't exist
	create_file_if(resume_file_address);

	// checks if there's a quiz to resume
	bool can_be_resumed = is_quiz_to_resume(resume_file_address);

	// vector containing the order by which the questions will appear
	vector<size_t> indexes;

	// vector of questions to retry
	vector<size_t> retry_indexes;

	// question number to start with
	size_t first_question_index { 0 };

	// gives to the user the choice to resume previous quiz
	if (can_be_resumed) {
		cout << "\033[2mDo you want to resume last quiz ?\033[0m\n";

		string choice { "" };
		while (getline(cin, choice)) {
			if (choice.size() != 1) choice = "0";

			switch (choice[0]) {
			case '$':
				get_resume_information(resume_file_address, first_question_index, indexes, retry_indexes);
				cout << '\n';
				break;

			case '*':
				indexes = get_random_int_distribution(questions.size());
				cout << '\n';
				break;

			default:
				cout << "\n\033[2mPlease enter a valid choice.\033[0m\n";
				continue;
			}

			break;
		}
	}
	else {
		// gets retry indexes from resume file
		retry_indexes = get_retry_indexes(resume_file_address);

		if(retry_indexes.empty())
			indexes = get_random_int_distribution(questions.size());
		else {
			// informs the user that there are questions to practice with
			cout << "\033[2mThere are questions available to practice with. Do you want to proceed?\033[0m\n";

			string choice { "" };
			while (getline(cin, choice)) {
				if (choice.size() != 1) choice = "0";

				switch (choice[0]) {
				case '$':
					indexes = get_random_int_distribution(questions.size());
					cout << '\n';
					break;

				case '*':
					return;

				default:
					cout << "\n\033[2mPlease enter a valid choice.\033[0m\n";
					continue;
				}

				break;
			}
		}
	}

	size_t indexes_size = indexes.size();

	// sets up resume and save files
	set_resume_file(resume_file_address, indexes, retry_indexes);

	for (size_t i = first_question_index; i < indexes_size; ++i) {
		// displays current question number
		cout << i + 1 << "\\" << indexes_size << '\n';

		// adds current question_number to resume file
		write_single_element(i + 1, resume_file_address, ios::in | ios::out, "");

		// displays current question
		cout << "\033[31m" << questions[indexes[i]] << "\033[0m\n\n";

		// gets answer
		string answer { "" };
		getline(cin, answer, '$');

		while(cin.peek() != '\n')
			getline(cin, answer, '$');

		getchar(); // deals with the newline left in cin

		// exit when answer = exit
		if (answer == exit_sequence) return;

		// displays current question's answer and index
		cout << "\n[\033[32m" << indexes[i] << "\033[0m]\n";
		cout << '\n' << answers[indexes[i]] << '\n';

		// asks if the user will retry the question and handles choice
		cout << "\n\033[2mRetry this question later ? \033[0m";

		while (getline(cin, answer)) {
			if (answer.size() != 1)
				answer = "0";

			switch (answer[0]) {
			case '$':
			{
				cout << '\n';
				review();

				// adds the question index in the retry indexes
				size_t number_of_items = count(retry_indexes.begin(), retry_indexes.end(), indexes[i]);
				while(number_of_items != maximum_number_of_questions) {
					retry_indexes.push_back(indexes[i]);
					number_of_items = count(retry_indexes.begin(), retry_indexes.end(), indexes[i]);
				}
			}
				break;

			case '*':
			{
				// removes the question index from the retry indexes if present
				vector<size_t>::iterator it = find(retry_indexes.begin(), retry_indexes.end(), indexes[i]);
				if (it != retry_indexes.end()) retry_indexes.erase(it);
			}
				break;

			default:
				cout << "\n\033[2mPlease enter a valid choice.\033[0m\n";
				continue;
			}

			if (i != indexes_size - 1) cout << '\n';

			break;
		}
		// copies the resume file
		copy_file(resume_file_address, save_file);

		// updates the resume file
		copy_lines(save_file, resume_file_address, 2);
		write_elements(retry_indexes, resume_file_address, ios::app, " ", " $");
		remove(save_file.c_str());
	}

	// fills the first line with whitespaces
	fill_with_pattern(" ", 6, resume_file_address, ios::in | ios::out, "");
}

// simple quiz launcher
void simple_quiz_launcher(const vector<string>& questions, const vector<string>& answers, const vector<size_t>& indexes, const string& resume_file)
// (1) displays a questions, wait for user's answer,
// (2) displays the right answer
// indexes provides the questions and their order of display
{
	// makes a copy of the retry indexes
	vector<size_t> updated_indexes { indexes };

	// exit sequence
	const string exit_sequence { "exit" };

	// save file
	const string save_file { "save.txt" };

	// maximum number of the same question allowed in retry list
	const size_t maximum_number_of_questions { 10 };

	// question number to start with
	size_t first_question_index { 0 };

	size_t indexes_size = indexes.size();

	for (size_t i = first_question_index; i < indexes_size; ++i) {
		// displays current question number
		cout << i + 1 << "\\" << indexes_size << '\n';

		// displays current question
		cout << "\033[31m" << questions[indexes[i]] << "\033[0m\n\n";

		// gets answer
		string answer { "" };
		getline(cin, answer, '$');

		while (cin.peek() != '\n') getline(cin, answer, '$');

		getchar(); // deals with the newline left in cin

		// checks if the user wants to exit
		if(answer == exit_sequence) return;

		// displays current question's answer and index
		cout << "\n[\033[32m" << indexes[i] << "\033[0m]\n";
		cout << '\n' << answers[indexes[i]] << '\n';

		// checks if questions should be removed from the resume file 
		cout << "\033[2m\nRemove from retry list ? \033[0m";
		while(getline(cin, answer)){
			if(answer.length() != 1) answer.clear(); // the answer is invalid

			switch(answer[0]){
			case '$':
				{
					// removes the question index from the retry indexes if present
					vector<size_t>::iterator it = find(updated_indexes.begin(), updated_indexes.end(), indexes[i]);
					if(it != updated_indexes.end()) updated_indexes.erase(it);
				}
				break;

			case '*':
				{
					// adds the question index in the retry indexes
					size_t number_of_items = count(updated_indexes.begin(), updated_indexes.end(), indexes[i]);
					if(number_of_items < maximum_number_of_questions) updated_indexes.push_back(indexes[i]);
				}
				break;

			case '!':
				{
					// does nothing
				}
				break;

			default:
				cout << "\n\033[2mPlease enter a valid choice.\033[0m\n";
				continue;
			}

			break;
		}

		// copies the resume file
		copy_file(resume_file, save_file);

		// updates the resume file
		copy_lines(save_file, resume_file, 2);	
		write_elements(updated_indexes, resume_file, ios::app, " ", " $");
		remove(save_file.c_str());

		if (i != indexes_size - 1) cout << '\n';
	}
}
