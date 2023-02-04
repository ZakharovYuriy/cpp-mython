#include "lexer.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <unordered_map>
#include <string_view>
#include <string>

using namespace std;

namespace parse {

bool operator==(const Token &lhs, const Token &rhs) {
	using namespace token_type;

	if (lhs.index() != rhs.index()) {
		return false;
	}
	if (lhs.Is<Char>()) {
		return lhs.As<Char>().value == rhs.As<Char>().value;
	}
	if (lhs.Is<Number>()) {
		return lhs.As<Number>().value == rhs.As<Number>().value;
	}
	if (lhs.Is<String>()) {
		return lhs.As<String>().value == rhs.As<String>().value;
	}
	if (lhs.Is<Id>()) {
		return lhs.As<Id>().value == rhs.As<Id>().value;
	}
	return true;
}

bool operator!=(const Token &lhs, const Token &rhs) {
	return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream &os, const Token &rhs) {
	using namespace token_type;

#define VALUED_OUTPUT(type) \
        if (auto p = rhs.TryAs<type>()) return os << #type << '{' << p->value << '}';

	VALUED_OUTPUT(Number);
	VALUED_OUTPUT(Id);
	VALUED_OUTPUT(String);
	VALUED_OUTPUT(Char);

#undef VALUED_OUTPUT

#define UNVALUED_OUTPUT(type) \
        if (rhs.Is<type>()) return os << #type;

	UNVALUED_OUTPUT(Class);
	UNVALUED_OUTPUT(Return);
	UNVALUED_OUTPUT(If);
	UNVALUED_OUTPUT(Else);
	UNVALUED_OUTPUT(Def);
	UNVALUED_OUTPUT(Newline);
	UNVALUED_OUTPUT(Print);
	UNVALUED_OUTPUT(Indent);
	UNVALUED_OUTPUT(Dedent);
	UNVALUED_OUTPUT(And);
	UNVALUED_OUTPUT(Or);
	UNVALUED_OUTPUT(Not);
	UNVALUED_OUTPUT(Eq);
	UNVALUED_OUTPUT(NotEq);
	UNVALUED_OUTPUT(LessOrEq);
	UNVALUED_OUTPUT(GreaterOrEq);
	UNVALUED_OUTPUT(None);
	UNVALUED_OUTPUT(True);
	UNVALUED_OUTPUT(False);
	UNVALUED_OUTPUT(Eof);

#undef UNVALUED_OUTPUT

	return os << "Unknown token :("sv;
}

Lexer::Lexer(std::istream &input) {
	ParsingData(input);
}

const Token& Lexer::CurrentToken() const {
	if (lexer_data.empty()) {
		throw std::logic_error("Not implemented"s);
	}
	return lexer_data[nomber_of_curent_token];
}

Token Lexer::NextToken() {
	if (nomber_of_curent_token < (static_cast<size_t>(lexer_data.size()) - 1)) {
		return lexer_data[++nomber_of_curent_token];
	}
	return lexer_data[nomber_of_curent_token];
}

bool Lexer::IsNumber(char ch) const {
	if (ch >= '0' && ch <= '9') {
		return true;
	}
	return false;
}

bool Lexer::IsLegalSymbolForId(char ch) const {
	if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_') {
		return true;
	}
	return false;
}

void Lexer::WriteWord(string &word) {
	if (!word.empty()) {
		if (IsNumber(word[0])) {
			for (const char &ch : word) {
				if (!IsNumber(ch)) {
					throw;
				}
			}
			lexer_data.push_back(token_type::Number { stoi(word) });
		} else {
			lexer_data.push_back(ParseToken(word));
		}
		word = ""s;
		is_first_line = false;
	}
}

void Lexer::WriteMathWord(string &math_word) {
	if (!math_word.empty()) {
		if (math_word.size() == 1) {
			lexer_data.push_back(token_type::Char { math_word[0] });
		} else {
			lexer_data.push_back(ParseToken(math_word));
		}
		math_word = ""s;
		is_first_line = false;
	}
}

void Lexer::WriteToString(const char symbol) {
	if (exception_symbol) {
		str_line += Unescape(symbol);
		exception_symbol = false;
	} else {
		str_line += symbol;
	}
	is_first_line = false;
}

void Lexer::WriteWords(std::string &word, std::string &math_word) {
	WriteWord(word);
	WriteMathWord(math_word);
}

void Lexer::NewLineProcessing() {
	WriteWords(word, math_word);
	if (!is_first_line && !is_new_line) {
		lexer_data.push_back(token_type::Newline());
		previous_line_offset = offset;
		offset = 0;
	}
	is_new_line = true;
	coment = false;
}

string Lexer::Unescape(const char symbol) {
	string result_symbol = "";

	switch (symbol) {
	case 'n':
		result_symbol += '\n';
		break;
	case 't':
		result_symbol += '\t';
		break;
	case '\'':
		result_symbol += '\'';
		break;
	case '"':
		result_symbol += '"';
		break;
	default:
		result_symbol += "\\";
		result_symbol += symbol;
	}
	return result_symbol;
}

void Lexer::QuotesProcessing(const char &symbol, bool &open) {
	if (!open) {
		WriteWords(word, math_word);
		open = true;
	} else {
		if (exception_symbol) {
			str_line += Unescape(symbol);
			exception_symbol = false;
		} else {
			open = false;
			lexer_data.push_back(token_type::String { str_line });
			str_line = "";
		}
	}
}

bool Lexer::CheckOffset(const char &symbol) {
	if (symbol == ' ') {
		++offset_buff;
		if (offset_buff == 2) {
			offset_buff = 0;
			++offset;
			if (previous_line_offset < offset) {
				lexer_data.push_back(token_type::Indent());
			}
		}
		return true;
	} else {
		if (symbol != '\n' && symbol != '#') {
			if ((previous_line_offset > offset)) {
				for (size_t i = previous_line_offset; i > offset; --i) {
					lexer_data.push_back(token_type::Dedent());
				}
			}
			is_new_line = false;
		} else {
			offset = previous_line_offset;
		}
	}
	return false;
}

void Lexer::ParsingData(std::istream &input) {
	using namespace std::literals;

	string line="";
	char symbol;
	size_t line_counter=0;
	while (getline(input, line)) {
		++line_counter;
		for (size_t i = 0; i < line.size(); ++i) {
			symbol = line[i];

			if(symbol == 0){
				continue;
			}

			if (coment) {
				continue;
			}

			if (is_new_line) {
				if (CheckOffset(symbol)) {
					continue;
				}
			}

			if (symbol == '\\') {
				exception_symbol = true;
				continue;
			}

			if (symbol == '\'' && !double_quote_open) {
				QuotesProcessing(symbol, single_quote_open);
				continue;
			}

			if (single_quote_open) {
				WriteToString(symbol);
				continue;
			}

			if (symbol == '"' && !single_quote_open) {
				QuotesProcessing(symbol, double_quote_open);
				continue;
			}
			if (double_quote_open) {
				WriteToString(symbol);
				continue;
			}

			if (symbol == '#') {
				coment = true;
				continue;
			}

			if (math_symbols_.count(symbol)) {
				WriteWord(word);
				math_word += symbol;
				if (math_word.size() > 1) {
					if (keywords_.count(math_word)) {
						WriteMathWord(math_word);
					} else {
						string ch = "";
						ch += math_word[0];
						WriteMathWord(ch);
						ch = "";
						ch += math_word[1];
						WriteMathWord(ch);
						math_word = "";
					}
				}
				continue;
			}

			if (trigger_symbols_.count(symbol)) {
				WriteWords(word, math_word);
				if (symbol != ' ') {
					lexer_data.push_back(token_type::Char { symbol });
				}
				continue;
			}

			if (!math_word.empty()) {
				WriteWords(word, math_word);
			}

			if (IsLegalSymbolForId(symbol) || IsNumber(symbol)) {
				word += symbol;
			} else {
				string err="";
				if(symbol < 0){
					err += "Most likely the wrong language is chosen\n";
				}
					err +="Invalid line nomber:" + to_string(line_counter) +
							"\nFirst error symbol nomber:" + to_string(i) +
							"\nInvalid line is:" + line;
				throw std::invalid_argument( err );
			}

		}
		NewLineProcessing();
	}
	WriteWords(word, math_word);
	if (!is_new_line) {
		lexer_data.push_back(token_type::Newline());
	}
	CheckOffset('e');
	lexer_data.push_back(token_type::Eof());
}

Token Lexer::ParseToken(std::string_view word) {
	if (word == "class"s) {
		return token_type::Class();
	}
	if (word == "return"s) {
		return token_type::Return();
	}
	if (word == "if"s) {
		return token_type::If();
	}
	if (word == "else"s) {
		return token_type::Else();
	}
	if (word == "def"s) {
		return token_type::Def();
	}
	if (word == "\n"s) {
		return token_type::Newline();
	}
	if (word == "print"s) {
		return token_type::Print();
	}
	if (word == "and"s) {
		return token_type::And();
	}
	if (word == "or"s) {
		return token_type::Or();
	}
	if (word == "not"s) {
		return token_type::Not();
	}
	if (word == "=="s) {
		return token_type::Eq();
	}
	if (word == "!="s) {
		return token_type::NotEq();
	}
	if (word == "<="s) {
		return token_type::LessOrEq();
	}
	if (word == "!="s) {
		return token_type::NotEq();
	}
	if (word == ">="s) {
		return token_type::GreaterOrEq();
	}
	if (word == "None"s) {
		return token_type::None();
	}
	if (word == "True"s) {
		return token_type::True();
	}
	if (word == "False"s) {
		return token_type::False();
	}
	return token_type::Id { static_cast<string>(word) };
}

}  // namespace parse
