#pragma once

#include<iostream>
using std::ios_base;

#include<vector>
using std::vector;

#include<string>
using std::string;

#include<limits>
using std::numeric_limits;

// symbolic names
constexpr char newline { '\n' };
constexpr char yes { '$' };
constexpr char no { '*' };
constexpr char alternative_no { '!' };
constexpr char file_line_delimiter { '$' };
constexpr char exit_character { 'x' };

const string exit_sequence { "exit" };

// maximum number of the same question allowed in retry list
const size_t maximum_number_of_questions { 3 };
const string INVALID_CHOICE = "0";
constexpr size_t INITIAL_POSITION = 0;
constexpr size_t INVALID_POSITION = numeric_limits<size_t>::max();

// file names
const string questions_answers_file { "questions_answers.txt" };
const string resume_file { "resume_quiz.txt" };
const string settings_file { "settings.txt" };
const string save_file { "save.txt" };

// user-defined type Quiz
struct Quiz {
	// default constructor
	Quiz() noexcept
		: questions{}
		, answers{}{}

	// questions and answers
	vector<string> questions;
	vector<string> answers;

	// Modes
	enum class Mode {
		practice, normal, resume
	};
};

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

// retrieves settings information from file
vector<size_t> get_settings();

// retrieves quiz information from files
Quiz get_questions_and_answers();

// gets resume file information
Resume get_resume_information();

// displays main menu
void display_main_menu();

// lists and numbers elements of a vector
template<typename T>
void list_elements(const vector<T>&);

// gets a random distribution of unique unsigned integers
vector<size_t> get_random_int_distribution(const size_t&);

// creates file if it doesn't exit
void create_file_if(const string&);

// creates settings file if it doesn't exist
void create_settings_file_if();

// writes elements of a vector on a file
template<typename T>
void write_elements(const vector<T>&, const string&, ios_base::openmode, const string&, const string&);

// writes a single element on a file
template<typename T>
void write_single_element(const T&, const string&, ios_base::openmode, const string&);

// sets up resume file
void update_resume_file(const Resume&);

// gets user's answer
string get_answer();

// shuffles vector
template<typename T>
void shuffle_vector(vector<T>&);

// gets position 
size_t get_position(const Resume&, const Quiz::Mode&);

// gets indexes 
vector<size_t> get_indexes(const Quiz&, const Resume&, const Quiz::Mode&);

// simple quiz launcher
void quiz_launcher(const Quiz&, const Resume&, const Quiz::Mode&);
