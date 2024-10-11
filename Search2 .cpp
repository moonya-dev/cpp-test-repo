#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// Константа, указывающая максимальное количество возвращаемых документов
const int MAX_RESULT_DOCUMENT_COUNT = 5;

// Объявляем структуру DocumentContent для хранения информации о документе
struct DocumentContent {
    int id; // Уникальный идентификатор документа
    vector<string> words; // Вектор слов, содержащихся в документе
};

// Функция для считывания строки с ввода
string ReadLine() {
    string s;
    getline(cin, s); // Считываем строку до символа новой строки
    return s; // Возвращаем считанную строку
}

// Функция для считывания числа из ввода и последующего считывания строки
int ReadLineWithNumber() {
    int result = 0;
    cin >> result; // Считываем число
    ReadLine(); // Считываем оставшуюся строку (это необходимо, чтобы убрать символ новой строки)
    return result; // Возвращаем считанное число
}

// Функция для разбиения строки на слова
vector<string> SplitIntoWords(const string& text) {
    vector<string> words; // Вектор для хранения слов
    string word; // Переменная для хранения текущего слова
    for (const char c : text) { // Проходим по каждому символу строки
        if (c == ' ') { // Если встречаем пробел
            if (!word.empty()) { // Если текущее слово не пустое
                words.push_back(word); // Добавляем его в вектор слов
                word.clear(); // Очищаем текущее слово
            }
        } else { // Если символ не пробел
            word += c; // Добавляем символ к текущему слову
        }
    }
    if (!word.empty()) { // Если в конце осталась часть слова
        words.push_back(word); // Добавляем его в вектор слов
    }

    return words; // Возвращаем вектор слов
}

// Функция для парсинга стоп-слов из строки
set<string> ParseStopWords(const string& text) {
    set<string> stop_words; // Множество для хранения уникальных стоп-слов
    for (const string& word : SplitIntoWords(text)) { // Разбиваем текст на слова
        stop_words.insert(word); // Добавляем каждое слово в множество (игнорируя дубликаты)
    }
    return stop_words; // Возвращаем множество стоп-слов
}

// Функция для разбиения строки на слова без стоп-слов
vector<string> SplitIntoWordsNoStop(const string& text, const set<string>& stop_words) {
    vector<string> words; // Вектор для хранения слов
    for (const string& word : SplitIntoWords(text)) { // Разбиваем текст на слова
        if (stop_words.count(word) == 0) { // Проверяем, является ли слово стоп-словом
            words.push_back(word); // Если нет, добавляем его в вектор
        }
    }
    return words; // Возвращаем вектор слов
}

// Функция для добавления документа в вектор документов
void AddDocument(vector<DocumentContent>& documents, const set<string>& stop_words,
                 int document_id, const string& document) {
    const vector<string> words = SplitIntoWordsNoStop(document, stop_words); // Разбиваем документ на слова без стоп-слов
    documents.push_back({document_id, words}); // Добавляем новый документ в вектор
}

// Функция для парсинга запроса и получения уникальных слов
set<string> ParseQuery(const string& text, const set<string>& stop_words) {
    set<string> query_words; // Множество для хранения уникальных слов запроса
    for (const string& word : SplitIntoWordsNoStop(text, stop_words)) { // Разбиваем запрос на слова
        query_words.insert(word); // Добавляем каждое слово в множество
    }
    return query_words; // Возвращаем множество слов запроса
}

// Функция для сопоставления документа с запросом и определения релевантности
int MatchDocument(const DocumentContent& content, const set<string>& query_words) {
    if (query_words.empty()) { // Если нет слов в запросе
        return 0; // Возвращаем 0 релевантности
    }
    set<string> matched_words; // Множество для хранения совпавших слов
    for (const string& word : content.words) { // Проходим по словам документа
        if (matched_words.count(word) != 0) { // Если слово уже учтено
            continue; // Пропускаем его
        }
        if (query_words.count(word) != 0) { // Если слово есть в запросе
            matched_words.insert(word); // Добавляем его в множество совпадений
        }
    }
    return static_cast<int>(matched_words.size()); // Возвращаем количество совпавших слов
}

// Функция для нахождения всех документов, подходящих под запрос
vector<pair<int, int>> FindAllDocuments(const vector<DocumentContent>& documents,
                                        const set<string>& query_words) {
    vector<pair<int, int>> matched_documents; // Вектор для хранения совпадающих документов
    for (const auto& document : documents) { // Проходим по каждому документу
        const int relevance = MatchDocument(document, query_words); // Получаем релевантность документа
        if (relevance > 0) { // Если релевантность положительная
            matched_documents.push_back({relevance, document.id}); // Добавляем документ и его релевантность в вектор
        }
    }
    return matched_documents; // Возвращаем вектор совпадающих документов
}

// Функция для нахождения топовых документов по релевантности
vector<pair<int, int>> FindTopDocuments(const vector<DocumentContent>& documents,
                                        const set<string>& stop_words, const string& raw_query) {
    const set<string> query_words = ParseQuery(raw_query, stop_words); // Парсим запрос
    auto matched_documents = FindAllDocuments(documents, query_words); // Находим все совпадающие документы

    // Сортируем документы по релевантности
    sort(matched_documents.begin(), matched_documents.end());
    reverse(matched_documents.begin(), matched_documents.end()); // Переворачиваем, чтобы получить убывающий порядок
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) { // Если документов больше, чем нужно
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT); // Обрезаем до нужного количества
    }
    for (auto& matched_document : matched_documents) { // Меняем местами релевантность и id
        swap(matched_document.first, matched_document.second);
    }
    return matched_documents; // Возвращаем топовые документы
}

int main() {
    const string stop_words_joined = ReadLine(); // Считываем стоп-слова
    const set<string> stop_words = ParseStopWords(stop_words_joined); // Парсим стоп-слова

    // Считываем документы
    vector<DocumentContent> documents; // Вектор для хранения документов
    const int document_count = ReadLineWithNumber(); // Считываем количество документов
    for (int document_id = 0; document_id < document_count; ++document_id) {
        AddDocument(documents, stop_words, document_id, ReadLine()); // Добавляем каждый документ
    }

    const string query = ReadLine(); // Считываем запрос
    for (auto [document_id, relevance] : FindTopDocuments(documents, stop_words, query)) {
        // Выводим результаты: id документа и его релевантность
        cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s
             << endl;
    }
}
