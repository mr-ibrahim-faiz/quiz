#include "quiz_launcher.h"
#include "properties.h"

#include<iostream>
using std::cin;
using std::cout;
using std::cerr;
using std::ws;
using std::streamsize;

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

// symbolic names
constexpr char yes { '$' };
constexpr char no { '*' };
constexpr char alternative_yes { ':' };
constexpr char alternative_no { '!' };
constexpr char list_elements_delimiter { ':' };
constexpr char file_line_delimiter { '$' };

const string exit_sequence { "exit" };

// constant expressions
const size_t maximum_number_of_questions { 3 };
const size_t minimum_number_of_questions { 10 };
constexpr size_t INITIAL_POSITION = 0;

// file names
const string settings_file { "settings.txt" };

// retrieves settings information from file
vector<size_t> get_settings()
// retrieves settings information from the settings file
// the file is created if it doesn't exist
{
	create_settings_file_if();

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
				else 
					properties.erase(sproperty);
			}
			else {
				cerr << "Error: settings file is corrupted !\n";
				cout << "Resetting... ";

				file.clear();
				if(file.is_open()) file.close();

				remove(settings_file.c_str());
				create_settings_file_if();

				result.clear();
				result = get_settings();

				cout << "Done...\n\n";
			}
		}
	}
	else cerr << "Error: Unable to open file.\n";

	return result;
}

// retrieves quiz information from files
Quiz get_questions_and_answers()
// retrieves questions and answers from the questions_answers file
// the file is created if it doesn't exist
{
	Quiz quiz;

	vector<string>& questions = quiz.questions;
	vector<string>& answers = quiz.answers;

	ifstream file;

	file.open(questions_answers_file);
	if (file.is_open()) {

		for (string line; getline(file, line, file_line_delimiter);) {
			if(!line.empty()) questions.push_back(line);
			file.ignore(1, newline);

			line.clear();
			getline(file, line, file_line_delimiter);
			if(!line.empty()) answers.push_back(line);

			file >> ws;
		}
		file.close();
	}
	else create_file_if(questions_answers_file);

	return quiz;
}

// gets resume file information
Resume get_resume_information()
// retrieves current question position, questions order indexes and retry indexes from resume file
{
	Resume resume;
	size_t& position = resume.position;
	vector<size_t>& indexes = resume.indexes;
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

		// retrieves retry indexes
		while (file >> index)
			retry_indexes.push_back(index);

		file.clear();
		file.ignore(numeric_limits<streamsize>::max(), newline);

		file.close();
	}
	else cerr << "Error: Unable to open file.\n";

	return resume;
}

// displays main menu
void display_main_menu() {
	// retrieves resume information from file
	Resume resume = get_resume_information();
	const size_t indexes_size = resume.retry_indexes.size();
	const size_t& position = resume.position;

	cout << "[1] List Questions\n";
	cout << "[2] " << (position != INVALID_POSITION ? "Resume Quiz" : "Complete Quiz") << newline;
	if(indexes_size) cout << "[3] Practice (" << indexes_size << ")\n";
	cout << "[x] Exit\n";
}

// lists and numbers elements of a vector
template<typename T>
void list_elements(const vector<T>& vec)
// lists and numbers elements of a vector vec
{
	for (size_t i = 0; i < vec.size(); ++i) cout << i + 1 << list_elements_delimiter << vec[i] << newline;
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
	file.open(file_address, ios_base::in);

	if (file.is_open()) file.close();
	else {
		file.open(file_address, ios_base::out);
		if(file.is_open()) file.close();
	}
}

// creates settings file if it doesn't exist
void create_settings_file_if(){
	fstream file;
	file.open(settings_file, ios_base::in);

	if (file.is_open()) file.close();
	else {
		file.open(settings_file, ios_base::out);

		if(file.is_open()) {
			file << "question\t0\n";
			file << "answer_index\t0\n";
			file << "prompt\t0\n";

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

// updates the resume file
void update_resume_file(const Resume&  resume) 
// saves the question position, the questions order and retry indexes on the resume file
{
	const size_t& position = resume.position;
	const vector<size_t>& indexes = resume.indexes;
	const vector<size_t>& retry_indexes = resume.retry_indexes;

	// saves the question position
	write_single_element(position, resume_file, ios_base::out, " $\n");

	// saves the questions order
	write_elements(indexes, resume_file, ios_base::app, " ", " $\n");

	// saves retry indexes
	write_elements(retry_indexes, resume_file, ios_base::app, " ", " $");
}

// gets user's answer
string get_answer() 
// gets the user's answer delimited by fine_line_delimiter
{
	string answer;
	for (; getline(cin, answer, file_line_delimiter);) {
		if (cin.peek() == newline) break;
	}
	getchar(); // deals with the newline
	return answer;
}

// reviews a question
void review(const string& question, const string& answer, const size_t& index)
// review a question an arbitrary number of times 
{
	// retrieves settings information from file
	const vector<size_t> settings = get_settings();
	
	for(string choice { yes }; ; getline(cin, choice)){
		if(choice.length() != valid_choice_length) choice = INVALID_CHOICE; // the choice is invalid
		
		const char& user_choice = choice[0];
		switch(user_choice){
		case yes:
		{
			// displays question
			cout << "\033[" << settings[size_t(Property::question)] << "m\n" << question << "\033[0m\n\n";
			
			// gets user's answer
			get_answer();
			
			// displays current question's answer
			cout << "\n[\033[" << settings[size_t(Property::answer_index)] << "m" << index << "\033[0m]\n";
			cout << '\n' << answer << '\n';
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

	case Quiz::Mode::practice:
		indexes = resume.retry_indexes;
		shuffle_vector(indexes);
		break;
	}
	
	return indexes;
}

// quiz launcher
vector<size_t> quiz_launcher(const Quiz& quiz, const Resume& resume, const Quiz::Mode& mode)
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
	vector<size_t> retry_indexes = resume.retry_indexes;

	// sets up resume file
	Resume updated_resume { position, indexes, retry_indexes };
	if (mode == Quiz::Mode::practice) {
		updated_resume.position = get_position(resume, Quiz::Mode::resume);
		updated_resume.indexes = get_indexes(quiz, resume, Quiz::Mode::resume);
	}
	update_resume_file(updated_resume);

	size_t indexes_size = indexes.size();
	size_t factor = retry_indexes.size() / minimum_number_of_questions + 1;

	for (; position < indexes_size; ++position) {
		// calls practice mode if necessary
		if(mode != Quiz::Mode::practice){
			if (retry_indexes.size() >= (minimum_number_of_questions*factor)) {
				cout << "[Practice]\n\n";
				retry_indexes = quiz_launcher(quiz, updated_resume, Quiz::Mode::practice);
				updated_resume.retry_indexes = retry_indexes;
				factor = retry_indexes.size() / minimum_number_of_questions + 1; // updates factor
				cout << "\n[Quiz]\n\n";
			}
		}

		// displays current question position
		size_t question_number = position + 1;
		cout << question_number << "\\" << indexes_size << newline;

		// updates resume file
		if (mode != Quiz::Mode::practice) {
			updated_resume.position = position;
			update_resume_file(updated_resume);
		}

		// current question and answer and question's index
		const string& question = questions[indexes[position]];
		const string& answer = answers[indexes[position]];
		const size_t& index = indexes[position];

		// keeps track of the number of a specific item in retry list
		size_t number_of_items = (size_t) count(retry_indexes.begin(), retry_indexes.end(), index);

		// displays current question
		cout << "\033[" << settings[size_t(Property::question)] << "m" << question << "\033[0m\n\n";

		// gets user's answer and checks if the user wants to exit
		if(get_answer() == exit_sequence) return updated_resume.retry_indexes;

		// displays current question's answer and index
		cout << "\n[\033[" << settings[size_t(Property::answer_index)] << "m" << index << "\033[0m]\n";
		cout << '\n' << answer << '\n';

		// checks if questions should be removed from the resume file 
		if(mode == Quiz::Mode::practice) cout << "\033[" << settings[size_t(Property::prompt)] << "m\n" << ((number_of_items == 1)? "Last appearance !\n" : "") << "Keep in retry list ?\n\033[0m";
		else cout << "\n\033[" << settings[size_t(Property::prompt)] << "m" << ((number_of_items == 0)? "Add to" : "Keep in") << " retry list ?\n\033[0m";

		for (string choice; getline(cin, choice);) {
			if(choice.length() != 1) choice = INVALID_CHOICE; // the choice is invalid

			const char& user_choice = choice[0];

			switch(user_choice){
			case yes:
			{
				// adds the question index in the retry indexes
				if (number_of_items < maximum_number_of_questions) retry_indexes.push_back(index);
			}
			break;

			case no:
			{
				// removes the question index from the retry indexes if present
				vector<size_t>::iterator it = find(retry_indexes.begin(), retry_indexes.end(), index);
				if (it != retry_indexes.end()) retry_indexes.erase(it);
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

			// updates resume file
			updated_resume.retry_indexes = retry_indexes;
			update_resume_file(updated_resume);

			if(user_choice == yes || user_choice == alternative_yes){
				cout << "\n[Review]\n";
				review(question, answer, index); // enables user to review failed question

				if (position != indexes_size - 1){
					switch(mode){
					case Quiz::Mode::normal: case Quiz::Mode::resume:
						if(retry_indexes.size() < (minimum_number_of_questions*factor)) cout << "\n[Quiz]\n";
						break;

					case Quiz::Mode::practice:
						cout << "\n[Practice]\n";
						break;
					}
				}
			}

			break;
		}

		if (position != indexes_size - 1) cout << newline;
	}

	// updates resume file
	if (mode != Quiz::Mode::practice) {
		updated_resume.position = INVALID_POSITION;
		updated_resume.retry_indexes = retry_indexes;
		update_resume_file(updated_resume);
	}

	return updated_resume.retry_indexes;
}
