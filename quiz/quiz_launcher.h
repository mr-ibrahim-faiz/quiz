#ifndef QUIZ_LAUNCHER_H
#define QUIZ_LAUNCHER_H

#include "quiz.h"
#include "resume.h"
#include "statistics.h"

#include<iostream>
using std::ios_base;

#include<string>
using std::wstring;

// symbolic names
constexpr char newline { '\n' };
constexpr char exit_character { 'x' };

const string clear_command { "cls" };

// constant expressions
const size_t valid_choice_length { 1 };
const string INVALID_CHOICE = "0";

// converts an UTF8 string to a wstring
wstring to_wstring(const string&);

// retrieves settings information from file
vector<size_t> get_settings();

// retrieves quiz information from file
Quiz get_questions_and_answers();

// retrieves resume information from file
Resume get_resume_information();

// retrieves statistics information from file
Statistics get_statistics_information();

// displays main menu
void display_main_menu();

// displays active mode
void display_active_mode(const Quiz::Mode&);

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

// updates the resume data
Resume update_resume(const Resume&, const Quiz&, const Statistics&);

// updates the resume file
void update_resume_file(const Resume&);

// updates the statistics data
Statistics update_statistics(const Statistics&, const Quiz&);

// updates the statistics file
void update_statistics_file(const Statistics&);

// updates the questions answers file
void update_questions_answers_file();

// gets user's answer
string get_answer();

// reviews a question
void review(const string&, const string&, const size_t&);

// shuffles vector
template<typename T>
void shuffle_vector(vector<T>&);

// gets position 
size_t get_position(const Resume&, const Quiz::Mode&);

// gets indexes 
vector<size_t> get_indexes(const Quiz&, const Resume&, const Quiz::Mode&);

// gets ignored questions
vector<size_t> get_ignored_questions(const Statistics&);

// gets question number
size_t get_question_number(const vector<size_t>&, const vector<size_t>&, const size_t&);

// gets number of questions
size_t get_number_of_questions(vector<size_t>, const vector<size_t>&, const Quiz::Mode&);

// checks if practice mode
bool is_practice(const Quiz::Mode&);

// initializes the quiz environment
void initialize_quiz();

// simple quiz launcher
[[maybe_unused]] Resume quiz_launcher(const Quiz&, const Resume&, const Quiz::Mode&);

#endif
