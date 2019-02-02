#pragma once

#include<iostream>
using std::ios;
using std::ios_base;

#include<vector>
using std::vector;

#include<string>
using std::string;

// retrieves settings information from file
vector<size_t> get_settings(const string&);

// retrieves quiz information from files
void get_questions_and_answers(const string&, vector<string>&, vector<string>&);

// displays main menu
void display_main_menu(const string&);

// lists and numbers elements of a vector
template<typename T>
void list_elements(const vector<T>&);

// gets a random distribution of unique unsigned integers
vector<size_t> get_random_int_distribution(const size_t&);

// copies a file
void copy_file(const string&, const string&);

// creates file if it doesn't exit
void create_file_if(const string&);

// creates settings file if it doesn't exist
void create_settings_file_if(const string&);

// writes elements of a vector on file
template<typename T>
void write_elements(const vector<T>&, const string&, ios::fmtflags, const string&, const string&);

// writes a single element on a file
template<typename T>
void write_single_element(const T&, const string&, ios::fmtflags, const string&);

// fills a file with a given pattern
template<typename T>
void fill_with_pattern(const T&, const size_t, const string&, ios::fmtflags, const string&);

// sets up resume and save file
void set_resume_file(const string&, const vector<size_t>&, const vector<size_t>&);

// gets user's answer
string get_answer();

// checks if there's a quiz to resume
bool is_quiz_to_resume(const string&);

// checks if there are questions to practice with
bool are_questions_to_practice(const string&);

// gets resume file information
void get_resume_information(string const&, size_t&, vector<size_t>&, vector<size_t>&);

// gets retry indexes
vector<size_t> get_retry_indexes(const string&);

// shuffles vector
template<typename T>
void shuffle_vector(vector<T>&);

// copies line from a file to another
void copy_lines(const string&, const string&, const size_t&, ios_base::openmode);

// quiz launcher
void quiz_launcher(const vector<string>&, const vector<string>&, const string&);

// simple quiz launcher
void simple_quiz_launcher(const vector<string>&, const vector<string>&, const vector<size_t>&, const string&, const string&);
