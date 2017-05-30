#include"Header.h"

int main() {
	char choice {'?'};
	vector<string> questions;
	vector<string> answers;
	vector<int> chapters;
	vector<string> chapter_names;

	ifstream file("questions_answers.txt");
	if (file.is_open()) {
		int input {0};
		while (file >> input)
			chapters.push_back(input);

		file.clear();
		file.ignore(numeric_limits<streamsize>::max(), '$');
		file >> ws;

		string line {""};
		while (getline(file,line,'\n')) {
			if (line != "$")
				chapter_names.push_back(line);
			else
				break;
		}

		file >> ws;

		while (getline(file, line,'$')) {
			file >> ws;
			questions.push_back(line);
			getline(file, line, '$');
			answers.push_back(line);
			file >> ws;
		}
		file.close();
	}
	else
		cerr << "Error: Unable to open file." << endl;

	display_choices();
	while (cin >> choice) {
		cin.ignore(numeric_limits<streamsize>::max(),'\n');

		switch (choice) {
		case '1':
		{
			// List all questions
			cout << endl;
			if (questions.size() > 0)
				for (unsigned int i = 0; i < questions.size(); ++i)
					cout << i + 1 << ":" << questions[i] << endl;
			else
				cout << "The list is empty." << endl;

		}
		break;

		case '2':
			if (questions.size() > 0 && questions.size() == answers.size())
				quizz_launcher(questions, answers, 0, questions.size() - 1, questions.size());
			else
				cout << "\nThere's not a single question to display." << endl;
			break;

		case '3':
		{
			cout << endl;

			if (chapter_names.size() > 0) {
				display_chapters(chapter_names);
				unsigned int chapter_choice{ 0 };
				cin >> chapter_choice;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');

				if (is_valid_chapter(chapter_choice, chapters.size() / 2)) {
					unsigned int first_question = chapters[(chapter_choice - 1) * 2], last_question = chapters[(chapter_choice - 1) * 2 + 1];
					int questions_size = last_question - first_question + 1;
					quizz_launcher(questions, answers, first_question, last_question, questions_size);
				}
				else
					cout << "\nThis chapter doesn't exist." << endl;
			}
			else
				cout << "There is no chapters available." << endl;
		}
			break;

		case '4':
		{
			// Failed questions quiz

			int index {0};
			vector<int> failed_indexes;
			
			ifstream file("failed_questions.txt");
			if (file.is_open()) {
				while (file >> index)
					failed_indexes.push_back(index);
				file.clear();
				file.ignore(numeric_limits<streamsize>::max(), '\n');
				file.close();
			}
			else
				cerr << "Error: Unable to open file." << endl;

			//// List failed questions
			//cout << endl;
			//for (unsigned int i = 0; i < failed_indexes.size(); ++i)
			//	cout << i + 1 << ":" << questions[failed_indexes[i]] << endl;

			quizz_launcher(questions, answers, failed_indexes);
		}

		break;

		case '5':
		{
			cout << endl;
			string answer {""};
			getline(cin, answer, '$');
		}
			break;

		case 'x':
			cout << "\nGoodbye !" << endl;
			return 0;
			break;

		default:
			break;
		}

		cout << endl;
		display_choices();
	}

	return 0;
}