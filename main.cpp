#include <chrono>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <unordered_set>

std::string generateUniqueId();
std::string getCurrentTime();
void addTask(std::string &filePath);
void showTask(std::string &filePath);
void deleteTask(std::string &filePath);

int main() {

  std::string welocmeQuery = R"(
(1)Add Task, (2)View Task, (3)Delete Task, (4)Quit
>> )";

  std::string filePath = "TODO.txt";

  int choice{};
  while (choice != 4) {
    std::cout << welocmeQuery;
    if (!(std::cin >> choice)) {
      std::cout << "Invalid input. Please enter a valid number.\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      choice = 0;
    }

    switch (choice) {
    case 1: {
      addTask(filePath);
      break;
    }
    case 2: {
      showTask(filePath);
      break;
    }
    case 3: {
      deleteTask(filePath);
      break;
    }

    case 0: {
      break;
    }
    }
  }
}

std::string generateUniqueId(size_t length) {
  static const char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dist(0, sizeof(chars) - 2);
  static std::unordered_set<std::string> usedId;

  std::string result;
  do {
    result.clear();
    for (size_t i = 0; i < length; ++i) {
      result += chars[dist(gen)];
    }

  } while (usedId.find(result) != usedId.end());

  usedId.insert(result);
  return result;
}

std::string getCurrentTime() {
  auto now = std::chrono::system_clock::now();
  std::string formatted_time = std::format("{:%d/%m/%Y %H:%M}", now);

  return formatted_time;
}

void addTask(std::string &filePath) {
  std::ofstream addInput(filePath, std::ios::app);
  if (!addInput) {
    std::cerr << "Error Opening the file\n";
    return;
  } else
    std::cin.ignore();

  std::string task;
  std::cout << "Task : ";
  std::getline(std::cin, task);
  addInput << generateUniqueId(6) << task << getCurrentTime() << "\n";
  addInput.close();
}

void showTask(std::string &filePath) {
  std::ifstream showInput(filePath);
  if (!showInput) {
    std::cerr << "No Task Found\n";
    return;
  }

  const size_t MAX_TEXT_LEN = 40;
  std::string line;
  std::cout << "==========================================================="
               "=========\n";
  std::cout << "  ID   |                   TASK                   |       "
               "TIME      |\n";
  std::cout << "==========================================================="
               "=========\n";

  while (std::getline(showInput, line)) {
    std::string text = line.substr(6, line.length() - 22);
    if (text.length() > MAX_TEXT_LEN) {
      text = text.substr(0, MAX_TEXT_LEN - 3) + "...";
    } else {
      text = text.append(MAX_TEXT_LEN - text.length(), ' ');
    }

    std::cout << line.substr(0, 6) << " | " << text << " | "
              << line.substr(line.length() - 16) << "|\n";
  }
  std::cout << "==========================================================="
               "=========\n";
  showInput.close();
}

void deleteTask(std::string &filePath) {
  std::ifstream showInput(filePath);
  if (!showInput) {
    std::cerr << "Error Accessing file!\n";
    return;
  }

  std::ofstream outFile("temp.txt");
  if (!outFile) {
    std::cerr << "Error creating temporary file\n";
    return;
  }

  bool taskDeleted = false;

  std::cin.ignore();
  std::string id;
  std::string line;
  std::cout << "Enter Task Id to Delete : ";
  std::getline(std::cin, id);

  while (std::getline(showInput, line)) {
    if (line.substr(0, 6) == id) {
      taskDeleted = true;
      continue;
    } else {

      outFile << line << '\n';
    }
  }

  showInput.close();
  outFile.close();

  if (std::remove(filePath.c_str()) != 0) {
    std::cerr << "Error Deleting File\n";
    return;
  }

  if (std::rename("temp.txt", filePath.c_str()) != 0) {
    std::cerr << "Error Renaming File\n";
  }

  if (taskDeleted)
    std::cout << "Task Deleted Successfully\n";
  else
    std::cout << "No Task Found with Matching ID\n";
}
