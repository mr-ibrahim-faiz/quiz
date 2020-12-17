#pragma warning(disable: 4996)

#include "quiz_launcher.h"
#include "properties.h"

#include<iostream>
using std::cin;
using std::cout;
using std::cerr;
using std::ws;
using std::streamsize;
using std::pair;

#include<random>
using std::random_device;

#include<algorithm>
using std::count;
using std::shuffle;

#include<fstream>
using std::fstream;
using std::ifstream;

#include<algorithm>
using std::find;

#include<codecvt>
using std::codecvt_utf8_utf16;

#include<locale>
using std::wstring_convert;

#include <io.h>
#include <fcntl.h>

#include<string>
using std::to_string;

#include<stdexcept>
using std::runtime_error;

// symbolic names
constexpr char yes { '$' };
constexpr char no { '*' };
constexpr char alternative_yes { ':' };
constexpr char alternative_no { '!' };
constexpr char list_elements_delimiter { ':' };
constexpr char file_line_delimiter { '$' };

const string exit_sequence { "exit" };
const string empty_line { "$" };

// constant expressions
constexpr int success_threshold { 10 };
constexpr size_t maximum_number_of_questions { 3 };
constexpr size_t minimum_number_of_questions { 10 };
constexpr size_t INITIAL_POSITION = 0;

// converts an UTF8 string to a wstring
wstring to_wstring(const string& source){
	wstring_convert<codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
	return convert.from_bytes(source.data());
}

// file names
const string questions_answers_file { "questions_answers.txt" };
const string resume_file { "resume_quiz.txt" };
const string statistics_file { "statistics.txt" };
const string settings_file { "settings.txt" };
const string temporary_file { "questions_answers_temp.txt" };

// retrieves settings information from file
vector<size_t> get_settings()
// retrieves settings information from the settings file
{
	ifstream file;
	file.open(settings_file);

	map<const string, Property> properties = mapping_string_property();

	const size_t number_of_properties = properties.size();
	vector<size_t> result(number_of_properties);

	if(file.is_open()) {
		string sproperty;

		while(file >> sproperty) {
			size_t value{};
			file >> value;

			if(file) {
				Property eproperty = properties[sproperty];
				if(properties.size() == number_of_properties) result[size_t(eproperty)] = value;
				else properties.erase(sproperty);
			}
			else cerr << "Error: settings file is corrupted !\n";
		}
	}
	else cerr << "Error: Unable to open settings file.\n";

	return result;
}

// retrieves quiz information from file
Quiz get_questions_and_answers()
// retrieves questions and answers from the questions_answers file
{
	Quiz quiz;

	vector<string>& questions = quiz.questions;
	vector<string>& answers = quiz.answers;
	vector<size_t>& removed = quiz.removed;

	ifstream file;

	file.open(questions_answers_file);
	if (file.is_open()) {
		for (pair<string,size_t> line_and_index; getline(file, line_and_index.first, file_line_delimiter); ++line_and_index.second) {
			string& line = line_and_index.first;
			size_t& index = line_and_index.second;

			if (!line.empty()) questions.push_back(line);
			else removed.insert(removed.begin(), index); // an empty line question is considered "removed" by the program
			file >> ws;

			line.clear();
			getline(file, line, file_line_delimiter);
			if(!line.empty()) answers.push_back(line); // an empty line answer is considered "removed" by the program
			file >> ws;
		}
		file.close();
	}
	else cerr << "Error: Unable to open questions and answers file.\n";

	return quiz;
}

// retrieves resume information from file
Resume get_resume_information()
// retrieves current question position, questions order indexes and retry indexes from resume file
{
	Resume resume;
	size_t& position = resume.position;
	vector<size_t>& indexes = resume.indexes;

	size_t& retry_position = resume.retry_position;
	vector<size_t>& retry_indexes = resume.retry_indexes;

	ifstream file;
	file.open(resume_file);

	if (file.is_open()) {
		// retrieves current question position
		if (file >> position){}
		else position = INVALID_POSITION;

		file.clear();
		file.ignore(numeric_limits<streamsize>::max(), newline);

		// retrieves questions order index
		size_t index{ 0 };
		while (file >> index)
			indexes.push_back(index);

		file.clear();
		file.ignore(numeric_limits<streamsize>::max(), newline);

		// retrieves current retry question position
		if (file >> retry_position) {}
		else retry_position = INVALID_POSITION;

		file.clear();
		file.ignore(numeric_limits<streamsize>::max(), newline);

		// retrieves retry indexes
		while (file >> index)
			retry_indexes.push_back(index);

		file.clear();
		file.ignore(numeric_limits<streamsize>::max(), newline);

		file.close();
	}
	else cerr << "Error: Unable to open resume file.\n";

	return resume;
}

// retrieves statistics information from file
Statistics get_statistics_information()
// retrieves statistics information from the statistics file, including the number of times a question was well answered.
{
	Statistics statistics;
	vector<size_t>& successes = statistics.successes;
	vector<size_t>& failures = statistics.failures;

	ifstream file;
	file.open(statistics_file);

	if (file.is_open()){
		// retrieves number of times questions were well answered
		size_t number { 0 };

		while(file >> number){
			successes.push_back(number);
			file.ignore(1);
			file >> number;
			failures.push_back(number);
		}

		file.close();
	}
	else cerr << "Error: Unable to open statistics file.\n";

	// checks the statistics data
	if(successes.size() != failures.size()) throw runtime_error("(statistics) size mismatch.");

	return statistics;
}

// displays main menu
void display_main_menu() {
	// retrieves resume information from file
	Resume resume = get_resume_information();
	const size_t indexes_size = resume.retry_indexes.size();
	const size_t& position = resume.position;
	const size_t& retry_position = resume.retry_position;

	cout << "[1] List Questions\n";
	cout << "[2] " << (position != INVALID_POSITION ? "Resume " : "") << "Quiz" << newline;
	if(indexes_size) cout << "[3] " << ((retry_position != INVALID_POSITION)? "Resume " : "") << "Practice (" << indexes_size << ")\n";
	cout << "[u] Updater\n";
	cout << "[x] Exit\n";
}

// displays active mode
void display_active_mode(const Quiz::Mode& mode) {
	if (is_practice(mode)) cout << "[Practice]\n\n";
	else cout << "[Quiz]\n\n";
}

// lists and numbers elements of a vector
template<typename T>
void list_elements(const vector<T>& vec)
// lists and numbers elements of a vector vec
{
	int translation_mode = _setmode(_fileno(stdout), _O_U16TEXT);
	for (size_t i = 0; i < vec.size(); ++i){
		wprintf(L"%d%lc%ls%lc", int(i + 1), int(list_elements_delimiter), to_wstring(vec[i]).data(), int(newline));
	}
	translation_mode = _setmode(_fileno(stdout), translation_mode);
}

template void list_elements<string>(const vector<string>& vec);

// gets a random distribution of unique unsigned integers
vector<size_t> get_random_int_distribution(const size_t& size)
// returns a shuffled vector of unique integers ranging from 0 to size-1
{
	vector<size_t> indexes;
	for (size_t i = 0; i < size; indexes.push_back(i++));

	shuffle_vector(indexes);

	return indexes;
}

// creates file if it doesn't exit
void create_file_if(const string& file_address)
// create the file if it doesn't exit
{
	fstream file;
	file.open(file_address, ios_base::in | ios_base::binary);

	if (file.is_open()) file.close();
	else {
		file.open(file_address, ios_base::out | ios_base::binary);
		if(file.is_open()) file.close();
	}
}

// creates settings file if it doesn't exist
void create_settings_file_if(){
	fstream file;
	file.open(settings_file, ios_base::in | ios_base::binary);

	if (file.is_open()) file.close();
	else {
		file.open(settings_file, ios_base::out | ios_base::binary);

		if(file.is_open()) {
			file << "question\t33\n";
			file << "answer_index\t33\n";
			file << "prompt\t90\n";

			file.close();
		}
		else cerr << "Error: Unable to open file.\n";
	}
}

// writes elements of a vector on a file
template<typename T>
void write_elements(const vector<T>& vec, const string& file_name, ios_base::openmode mode, const string& delimiter, const string& period)
// write elements of a vector on file_name
// elements are delimited by the delimiter
// the writing is ended by the period
{
	fstream file;
	file.open(file_name, mode);

	if (file.is_open()) {
		const size_t size = vec.size();

		if (size == 0) file << period;
		else
			for (size_t i = 0; i < size; ++i) {
				file << vec[i] << ((i != size - 1) ? delimiter : period);
			}

		file.close();
	}
	else cerr << "Error: Unable to open file.\n";
}

// writes a single element on a file
template<typename T>
void write_single_element(const T& t, const string& file_name, ios_base::openmode mode, const string& period)
// writes a single element on file_name
// the writing is ended by the period
{
	fstream file;
	file.open(file_name, mode);

	if (file.is_open()) {
		file << t << period;
		file.close();
	}
	else cerr << "Error: Unable to open file.\n";
}

// updates the resume data
Resume update_resume(const Resume& resume, const Quiz& quiz, const Statistics& statistics)
{
	Resume updated_resume;
	updated_resume.position = resume.position;
	updated_resume.retry_position = resume.retry_position;

	vector<size_t> indexes = resume.indexes;
	vector<size_t> retry_indexes = resume.retry_indexes;

	const vector<size_t>& removed = quiz.removed;

	// gets ignored questions
	const vector<size_t> ignored_questions = get_ignored_questions(statistics);

	// removes empty questions and answers
	for(size_t i { 0 }; i < removed.size(); ++i) {
		const size_t& removed_index = removed[i];
		size_t& updated_position = updated_resume.position;
		size_t& updated_retry_position = updated_resume.retry_position;

		vector<size_t> updated_indexes;
		for (size_t j { 0 }; j < indexes.size(); ++j) {
			const size_t& index = indexes[j];

			if (index != removed_index) {
				updated_indexes.push_back((index > removed_index) ? index - 1 : index);
			}
			else {
				if(updated_position != INVALID_POSITION && updated_position > j) --updated_position;
			}
		}
		indexes = updated_indexes;

		vector<size_t> updated_retry_indexes;
		for (size_t j { 0 }; j < retry_indexes.size(); ++j) {
			const size_t& index = retry_indexes[j];

			if (index != removed_index) {
				updated_retry_indexes.push_back((index > removed_index) ? index - 1 : index);
			}
			else {
				if(updated_retry_position != INVALID_POSITION && updated_retry_position > j) --updated_retry_position;
			}
		}
		retry_indexes = updated_retry_indexes;
	}

	// removes questions that exceeded the success threshold
	for(const size_t& ignored_question: ignored_questions){
		vector<size_t> updated_retry_indexes;
		for(const size_t& retry_index: retry_indexes){
			if(ignored_question != retry_index) updated_retry_indexes.push_back(retry_index);
		}
		retry_indexes = updated_retry_indexes;
	}

	// adds newly added questions
	const size_t questions_size = quiz.questions.size();
	const size_t indexes_size = indexes.size();

	if (!indexes.empty() && (questions_size > indexes_size)) {
		const size_t new_questions = questions_size - indexes_size;
		for (size_t i { 0 }; i < new_questions; ++i) {
			indexes.push_back(indexes_size + i);
			retry_indexes.push_back(indexes_size + i);
		}
	}

	updated_resume.indexes = indexes;
	updated_resume.retry_indexes = retry_indexes;

	return updated_resume;
}

// updates the resume file
void update_resume_file(const Resume&  resume) 
// saves the question position, the questions order and retry indexes on the resume file
{
	const size_t& position = resume.position;
	const vector<size_t>& indexes = resume.indexes;

	const size_t& retry_position = resume.retry_position;
	const vector<size_t>& retry_indexes = resume.retry_indexes;

	// saves the question position
	write_single_element(position, resume_file, ios_base::out | ios_base::binary, " $\n");

	// saves the questions order
	write_elements(indexes, resume_file, ios_base::app | ios_base::binary, " ", " $\n");

	// saves the retry question position
	write_single_element(retry_position, resume_file, ios_base::app | ios_base::binary, " $\n");

	// saves retry indexes
	write_elements(retry_indexes, resume_file, ios_base::app | ios_base::binary, " ", " $");
}

// updates the statistics data
Statistics update_statistics(const Statistics& statistics, const Quiz& quiz){
	Statistics updated_statistics;
	vector<size_t> successes = statistics.successes;
	vector<size_t> failures = statistics.failures;

	const vector<size_t>& removed = quiz.removed;

	// removes empty questions and answers
	for (size_t i { 0 }; i < removed.size(); ++i) {
		const size_t& removed_index = removed[i];
		successes.erase(successes.begin() + (int) removed_index);
		failures.erase(failures.begin() + (int) removed_index);
	}

	// adds newly added questions
	const size_t questions_size = quiz.questions.size();
	const size_t successes_size = successes.size();

	if (questions_size > successes_size) {
		const size_t new_questions = questions_size - successes_size;
		for (size_t i { 0 }; i < new_questions; ++i) {
			successes.push_back(0);
			failures.push_back(0);
		}
	}

	updated_statistics.successes = successes;
	updated_statistics.failures = failures;

	// checks the statistics data
	if(successes.size() != failures.size()) throw runtime_error("(statistics) size mismatch.");

	return updated_statistics;
}

// updates the statistics file
void update_statistics_file(const Statistics& statistics){
	const vector<size_t>& successes = statistics.successes;
	const vector<size_t>& failures = statistics.failures;

	// checks the statistics data
	if(successes.size() != failures.size()) throw runtime_error("(statistics) size mismatch.");

	fstream file;
	file.open(statistics_file, ios_base::out | ios_base::binary);

	if(file.is_open()){
		for(size_t i { 0 }; i < successes.size(); ++i)
			file << successes[i] << list_elements_delimiter << failures[i] << newline;
		file.close();
	}
	else cerr << "Error: Unable to open statistics file.\n";
}

// updates the questions answers file
void update_questions_answers_file()
// removes empty questions and answers from file
{
	fstream source(questions_answers_file, ios_base::in | ios_base::binary);

	if(source.is_open()) {
		fstream destination(temporary_file, ios_base::out | ios_base::binary);

		if (destination.is_open()) {
			for (string line; getline(source, line);) {
				if (line != empty_line) {
					destination << line << newline;
				}
				else source >> ws;
			}
			destination.close();
		}
		else cerr << "Error: Unable to open destination file.\n";
		source.close();
	}
	else cerr << "Error: Unable to open source file.\n";

	remove(questions_answers_file.c_str());
	rename(temporary_file.c_str(), questions_answers_file.c_str());
}

// gets user's answer
string get_answer() 
// gets the user's answer delimited by fine_line_delimiter
{
	string answer;
	for (; getline(cin, answer, file_line_delimiter);) {
		if (cin.peek() == newline) break;
	}
	[[maybe_unused]] int ch = getchar(); // deals with the newline
	return answer;
}

// reviews a question
void review(const string& question, const string& answer, const size_t& index)
// review a question an arbitrary number of times 
{
	// retrieves settings information from file
	const vector<size_t> settings = get_settings();
	
	for(string choice { alternative_yes }; ; getline(cin, choice)){
		if(choice.length() != valid_choice_length) choice = INVALID_CHOICE; // the choice is invalid
		
		const char& user_choice = choice[0];
		switch(user_choice){
		case yes:
		{
			// clears screen
			[[maybe_unused]] int result = system(clear_command.c_str());

			// displays mode
			cout << "[Review]\n";
		}

		case alternative_yes:
		{
			// displays question
			int translation_mode = _setmode(_fileno(stdout), _O_U16TEXT);
			string squestion = "\033[" + to_string(settings[size_t(Property::question)]) + "m\n" + question + "\033[0m\n\n";
			wprintf(L"%ls", to_wstring(squestion).data());
			translation_mode = _setmode(_fileno(stdout), translation_mode);
			
			// gets user's answer
			[[maybe_unused]] string user_answer = get_answer();
			
			// displays current question's answer
			cout << "\n[\033[" << settings[size_t(Property::answer_index)] << "m" << index << "\033[0m]\n";
			translation_mode = _setmode(_fileno(stdout), translation_mode);
			wprintf(L"\n%ls\n", to_wstring(answer).data());
			translation_mode = _setmode(_fileno(stdout), translation_mode);
			cout << "\033[" << settings[size_t(Property::prompt)] << "m\nTry again ?\n\033[0m";
        }
        	break;

        case no:
        	return;

        default:
			cout << "\n\033[" << settings[size_t(Property::prompt)] << "mPlease enter a valid choice.\033[0m\n";
        	break;
        }
    }
}

// shuffles vector
template<typename T>
void shuffle_vector(vector<T>& vec) {
	random_device rd;
	shuffle(vec.begin(), vec.end(), rd);
}

// gets position 
size_t get_position(const Resume& resume, const Quiz::Mode& mode) {
	switch (mode) {
	case Quiz::Mode::resume:
		return resume.position;

	case Quiz::Mode::practice_resume:
		return resume.retry_position;

	default:
		return INITIAL_POSITION;
	}
}

// gets indexes 
vector<size_t> get_indexes(const Quiz& quiz, const Resume& resume, const Quiz::Mode& mode) {
	vector<size_t> indexes;

	switch (mode) {
	case Quiz::Mode::normal:
		indexes = get_random_int_distribution(quiz.questions.size());
		break;

	case Quiz::Mode::resume:
		indexes = resume.indexes;
		break;

	case Quiz::Mode::practice_normal: case Quiz::Mode::practice_resume:
		indexes = resume.retry_indexes;
		if(mode == Quiz::Mode::practice_normal) shuffle_vector(indexes);
		break;
	}
	
	return indexes;
}

// gets ignored questions
vector<size_t> get_ignored_questions(const Statistics& statistics){
	vector<size_t> ignored_questions;
	const vector<size_t>& successes = statistics.successes;
	const vector<size_t>& failures = statistics.failures;
	for(size_t i = 0; i < successes.size(); ++i){
		const int success = (int) successes[i];
		const int failure = (int) failures[i];
		if(success - failure > success_threshold) ignored_questions.push_back(i);
	}
	return ignored_questions;
}

// gets question number
size_t get_question_number(const vector<size_t>& questions, const vector<size_t>& ignored_questions, const size_t& position){
	size_t question_number { position + 1 };
	for(size_t i { 0 }; i < questions.size(); ++i){
		if(i == position) break;
		const size_t& question = questions[i];
		for(const size_t& ignored_question : ignored_questions)
			if(ignored_question == question) --question_number;
	}
	return question_number;
}

// gets number of questions
size_t get_number_of_questions(vector<size_t> questions, const vector<size_t>& ignored_questions, const Quiz::Mode& mode){
	size_t number_of_questions { 0 };

	switch (mode) {
	case Quiz::Mode::normal: case Quiz::Mode::resume:
		number_of_questions = questions.size() - ignored_questions.size();
		break;

	case Quiz::Mode::practice_normal: case Quiz::Mode::practice_resume:
	{
		for(const size_t& ignored_question: ignored_questions){
			vector<size_t> updated_questions;
			for(const size_t& question: questions){
				if(ignored_question != question) updated_questions.push_back(question);
			}
			questions = updated_questions;
		}
		number_of_questions = questions.size();
	}
		break;
	}

	return number_of_questions;
}

// checks if practice mode
bool is_practice(const Quiz::Mode& mode) {
	if (mode == Quiz::Mode::practice_normal || mode == Quiz::Mode::practice_resume) return true;
	else return false;
}

// initializes the quiz environment
void initialize_quiz()
// creates the required files including questions and answers file, resume file, and statistics file
{
	create_settings_file_if();
	create_file_if(questions_answers_file);
	create_file_if(resume_file);
	create_file_if(statistics_file);
}

// quiz launcher
[[maybe_unused]] Resume quiz_launcher(const Quiz& quiz, const Resume& resume, const Quiz::Mode& mode)
// (1) displays a question, wait for the user's answer,
// (2) displays the right answer
// (3) checks if the user wants to retry the question later
// (4) returns the indexes of questions we want to try later
{
	// retrieves settings information from file
	const vector<size_t> settings = get_settings();

	// retrieves the quiz information
	const vector<string>& questions = quiz.questions;
	const vector<string>& answers = quiz.answers;

	size_t position = get_position(resume, mode); // question to start with
	const vector<size_t> indexes = get_indexes(quiz, resume, mode);

	size_t retry_position = resume.retry_position;
	vector<size_t> retry_indexes = (mode == Quiz::Mode::practice_normal)? indexes : resume.retry_indexes;

	// retrieves the statistics information
	Statistics statistics = get_statistics_information();
	statistics = update_statistics(statistics, quiz);
	update_statistics_file(statistics);

	vector<size_t>& successes = statistics.successes;
	vector<size_t>& failures = statistics.failures;

	// sets up resume file
	Resume updated_resume { position, indexes, retry_position, retry_indexes };
	if (is_practice(mode)) {
		updated_resume.position = resume.position;
		updated_resume.indexes = resume.indexes;
	}
	updated_resume = update_resume(updated_resume, quiz, statistics);
	update_resume_file(updated_resume);

	size_t removed_questions { 0 };
	size_t indexes_size = indexes.size();
	size_t factor = retry_indexes.size() / minimum_number_of_questions + 1;

	for (; position < indexes_size; ++position) {
		// calls practice mode if necessary
		if(!is_practice(mode)){
			if (retry_indexes.size() >= (minimum_number_of_questions*factor)) {
				updated_resume = quiz_launcher(quiz, updated_resume, (retry_position == INVALID_POSITION)? Quiz::Mode::practice_normal : Quiz::Mode::practice_resume);
				factor = retry_indexes.size() / minimum_number_of_questions + 1; // updates factor
			}
		}

		// current question and answer and question's index
		const string& question = questions[indexes[position]];
		const string& answer = answers[indexes[position]];
		const size_t& index = indexes[position];

		// questions that exceeds the success threshold are ignored
		const int success = (int) successes[index];
		const int failure = (int) failures[index];
		if(success - failure > success_threshold) continue;

		// clears screen
		[[maybe_unused]] int result = system(clear_command.c_str());

		// displays active mode
		display_active_mode(mode);

		// displays current question position
		const vector<size_t> ignored_questions = get_ignored_questions(statistics);

		const size_t question_number = get_question_number(indexes, ignored_questions, position);
		const size_t number_of_questions = get_number_of_questions(indexes, ignored_questions, mode);
		cout << question_number << "\\" << number_of_questions << newline;

		// updates statistics file
		statistics = update_statistics(statistics, quiz);
		update_statistics_file(statistics);

		// updates resume file
		if (!is_practice(mode)) { updated_resume.position = position; }
		else { updated_resume.retry_position = position; }
		updated_resume = update_resume(updated_resume, quiz, statistics);
		update_resume_file(updated_resume);

		// keeps track of the number of a specific item in retry list
		size_t number_of_items = (size_t) count(retry_indexes.begin(), retry_indexes.end(), index);

		// displays current question
		// SET FOR DEPRECATION: the question is marked with an asterisk if it was well answered a certain nubmer of times
		int translation_mode = _setmode(_fileno(stdout), _O_U16TEXT);
		string squestion = "\033[" + to_string(settings[size_t(Property::question)]) + "m" + question + ((success - failure > success_threshold)? "*" : "") + "\033[0m\n\n";
		wprintf(L"%ls", to_wstring(squestion).data());
		translation_mode = _setmode(_fileno(stdout), translation_mode);

		// gets user's answer and checks if the user wants to exit
		if (get_answer() == exit_sequence) {
			if (is_practice(mode)) {
				// updates statistics file
				statistics = update_statistics(statistics, quiz);
				update_statistics_file(statistics);

				// updates resume file
				updated_resume.retry_position -= removed_questions;
				updated_resume = update_resume(updated_resume, quiz, statistics);
				update_resume_file(updated_resume);
			}

			// clears screen
			[[maybe_unused]] int result_clear = system(clear_command.c_str());

			return updated_resume;
		}

		// displays current question's answer and index
		cout << "\n[\033[" << settings[size_t(Property::answer_index)] << "m" << index << "\033[0m]\n";
		translation_mode = _setmode(_fileno(stdout), translation_mode);
		wprintf(L"\n%ls\n", to_wstring(answer).data());
		translation_mode = _setmode(_fileno(stdout), translation_mode);

		// checks if questions should be removed from the resume file 
		if(is_practice(mode)) cout << "\033[" << settings[size_t(Property::prompt)] << "m\n" << ((number_of_items == 1)? "Last appearance !\n" : "") << "Keep in retry list ?\n\033[0m";
		else cout << "\n\033[" << settings[size_t(Property::prompt)] << "m" << ((number_of_items == 0)? "Add to" : "Keep in") << " retry list ?\n\033[0m";

		for (string choice; getline(cin, choice);) {
			if(choice.length() != 1) choice = INVALID_CHOICE; // the choice is invalid

			const char& user_choice = choice[0];

			switch(user_choice){
			case yes:
			{
				// adds the question index in the retry indexes
				if (number_of_items < maximum_number_of_questions) retry_indexes.push_back(index);
				failures[index] = failures[index] + 1;
			}
			break;

			case no:
			{
				// removes the question index from the retry indexes if present
				vector<size_t>::iterator it = find(retry_indexes.begin(), retry_indexes.end(), index);
				if (it != retry_indexes.end()) {
					retry_indexes.erase(it);
					if (is_practice(mode)) { ++removed_questions; }
				}
				successes[index] = successes[index] + 1;
			}
			break;

			case alternative_yes: case alternative_no:
			{
				// does nothing
			}
			break;

			default:
				cout << "\n\033[" << settings[size_t(Property::prompt)] << "mPlease enter a valid choice.\033[0m\n";
				continue;
			}

			// updates statistics file
			statistics = update_statistics(statistics, quiz);
			update_statistics_file(statistics);

			// updates resume file
			updated_resume.retry_indexes = retry_indexes;
			updated_resume = update_resume(updated_resume, quiz, statistics);
			update_resume_file(updated_resume);

			if(user_choice == yes || user_choice == alternative_yes){
				review(question, answer, index); // enables user to review failed question
			}

			break;
		}

		if (position != indexes_size - 1) cout << newline;
	}

	// updates statistics file
	statistics = update_statistics(statistics, quiz);
	update_statistics_file(statistics);

	// updates resume file
	if (!is_practice(mode)) { updated_resume.position = INVALID_POSITION; }
	else { updated_resume.retry_position = INVALID_POSITION; }
	updated_resume.retry_indexes = retry_indexes;
	updated_resume = update_resume(updated_resume, quiz, statistics);
	update_resume_file(updated_resume);

	// clears screen
	[[maybe_unused]] int result = system(clear_command.c_str());

	return updated_resume;
}
