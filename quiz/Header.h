#include<iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::ws;
using std::streamsize;

#include<vector>
using std::vector;

#include<string>
using std::string;

#include<fstream>
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::ios;

#include<limits>
using std::numeric_limits;

#include<algorithm>
using std::random_shuffle;

#include<ctime>

// Display choices
void display_choices() {
	cout << "[1] List Questions" << endl;
	cout << "[2] Complete Quiz" << endl;
	cout << "[3] Chapter Quiz" << endl;
	cout << "[4] Failed Questions Quiz" << endl;
	cout << "[5] Free Word" << endl;
	cout << "[x] Exit" << endl;
}

// Display chapters
void display_chapters(vector<string> chapter_names) {
	for (unsigned int i = 0; i < chapter_names.size(); ++i)
		cout << "[" << i + 1 << "] " << chapter_names[i] << endl;
	cout << "[x] Menu" << endl;
}

// Check if a chapter is valid
bool is_valid_chapter(int number_chapter, int total_chapters) {
	return (((number_chapter <= total_chapters) && (number_chapter>0)) ? true : false);
}

// Quiz launchers
void quizz_launcher(vector<string> questions, vector<string> answers,
	unsigned int first_question, unsigned int last_question, int questions_size) {

	fstream file;
	string line{ "" };

	file.open("failed_questions.txt");
	if (file.is_open()) {
		getline(file, line, '$');
		file.ignore(numeric_limits<streamsize>::max(), '\n');
		file.close();
	}
	else
		cerr << "Error: Unable to open file." << endl;

	file.open("failed_questions.txt", ios::out | ios::trunc);
	if (file.is_open()) {
		file << line;
		file.close();
	}
	else
		cerr << "Error: Unable to open file." << endl;

	vector<int> indexes;
	vector<int> retry_index;
	char retry_answer{ '?' };
	string answer{ "" };

	for (unsigned int i = first_question; i <= last_question; ++i)
		indexes.push_back(i);

	srand((unsigned int)time(NULL));
	random_shuffle(indexes.begin(), indexes.end());
	cout << endl;

	int question_number{ 1 };

	for (vector<int>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
		cout << question_number << "\\" << questions_size << endl;
		cout << questions[*it] << endl;
		getline(cin, answer, '$');
		cout << "\n" << answers[*it] << endl;
		cout << endl;

		cout << "Retry this question later? ";
		cin >> retry_answer;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (retry_answer) {
		case '$':
			retry_index.push_back(*it);
			break;
		default:
			break;
		}

		cout << endl;
		++question_number;
	}

	if (retry_index.size() > 0) {
		cout << "Retry !\n" << endl;
		question_number = 1;
		for (vector<int>::iterator it = retry_index.begin(); it != retry_index.end(); ++it) {
			cout << question_number << "\\" << retry_index.size() << endl;
			cout << questions[*it] << endl;
			getline(cin, answer, '$');
			cout << "\n" << answers[*it] << endl;

			cout << "\nAdd to failed question list? ";
			cin >> retry_answer;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			switch (retry_answer) {
			case '$':
			{
				ofstream file;
				file.open("failed_questions.txt", ios::ate | ios::app);

				if (file.is_open()) {
					file << *it << " ";
					file.close();
				}
				else
					cerr << "Error: Unable to open file." << endl;
			}
			break;
			default:
				break;
			}

			cout << endl;
			++question_number;
		}
	}

	file.open("failed_questions.txt", ios::ate | ios::app);

	if (file.is_open()) {
		file << "$";
		file.close();
	}
	else
		cerr << "Error: Unable to open file." << endl;
}

// Quiz laucher for failed questions
void quizz_launcher(vector<string> questions, vector<string> answers, vector<int> indexes) {
	if (indexes.size() > 0) {
		fstream file;
		file.open("failed_questions.txt", ios::out | ios::trunc);
		if (file.is_open())
			file.close();
		else
			cerr << "Error: Unable to open the file." << endl;

		vector<int> retry_index;
		char retry_answer{ '?' };
		string answer{ "" };

		srand((unsigned int)time(NULL));
		random_shuffle(indexes.begin(), indexes.end());
		cout << endl;

		int question_number{ 1 };

		for (vector<int>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
			cout << question_number << "\\" << indexes.size() << endl;
			cout << questions[*it] << endl;
			getline(cin, answer, '$');
			cout << "\n" << answers[*it] << endl;
			cout << endl;

			cout << "Retry this question later? ";
			cin >> retry_answer;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			switch (retry_answer) {
			case '$':
				retry_index.push_back(*it);
				break;
			default:
				break;
			}

			cout << endl;
			++question_number;
		}

		if (retry_index.size() > 0) {
			cout << "Retry !\n" << endl;
			question_number = 1;
			for (vector<int>::iterator it = retry_index.begin(); it != retry_index.end(); ++it) {
				cout << question_number << "\\" << retry_index.size() << endl;
				cout << questions[*it] << endl;
				getline(cin, answer, '$');
				cout << "\n" << answers[*it] << endl;

				cout << "\nKeep in failed question list? ";
				cin >> retry_answer;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				switch (retry_answer) {
				case '$':
				{
					ofstream file;
					file.open("failed_questions.txt", ios::ate | ios::app);

					if (file.is_open()) {
						file << *it << " ";
						file.close();
					}
					else
						cerr << "Error: Unable to open file." << endl;
				}
				break;
				default:
					break;
				}

				cout << endl;
				++question_number;
			}
		}

		file.open("failed_questions.txt", ios::ate | ios::app);

		if (file.is_open()) {
			file << "$";
			file.close();
		}
		else
			cerr << "Error: Unable to open file." << endl;
	}
	else
		cout << "\nThere is no questions available." << endl;
}