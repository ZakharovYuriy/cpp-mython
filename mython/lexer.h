#pragma once

#include <iosfwd>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>


namespace parse {

    using namespace std::literals;

    namespace token_type {

        struct Number {  // Лексема «число»
          int value;   // число
        };

        struct Id {             // Лексема «идентификатор»
            std::string value;  // Имя идентификатора
        };

        struct Char {    // Лексема «символ»
            char value;  // код символа
        };

        struct String {  // Лексема «строковая константа»
            std::string value;
        };

        struct Class {};    // Лексема «class»
        struct Return {};   // Лексема «return»
        struct If {};       // Лексема «if»
        struct Else {};     // Лексема «else»
        struct Def {};      // Лексема «def»
        struct Newline {};  // Лексема «конец строки»
        struct Print {};    // Лексема «print»
        struct Indent {};  // Лексема «увеличение отступа», соответствует двум пробелам
        struct Dedent {};  // Лексема «уменьшение отступа»
        struct Eof {};     // Лексема «конец файла»
        struct And {};     // Лексема «and»
        struct Or {};      // Лексема «or»
        struct Not {};     // Лексема «not»
        struct Eq {};      // Лексема «==»
        struct NotEq {};   // Лексема «!=»
        struct LessOrEq {};     // Лексема «<=»
        struct GreaterOrEq {};  // Лексема «>=»
        struct None {};         // Лексема «None»
        struct True {};         // Лексема «True»
        struct False {};        // Лексема «False»
    }  // namespace token_type

    using TokenBase
        = std::variant<token_type::Number, token_type::Id, token_type::Char, token_type::String,
                       token_type::Class, token_type::Return, token_type::If, token_type::Else,
                       token_type::Def, token_type::Newline, token_type::Print, token_type::Indent,
                       token_type::Dedent, token_type::And, token_type::Or, token_type::Not,
                       token_type::Eq, token_type::NotEq, token_type::LessOrEq, token_type::GreaterOrEq,
                       token_type::None, token_type::True, token_type::False, token_type::Eof>;

    struct Token : TokenBase {
        using TokenBase::TokenBase;

        template <typename T>
        [[nodiscard]] bool Is() const {
            return std::holds_alternative<T>(*this);
        }

        template <typename T>
        [[nodiscard]] const T& As() const {
            return std::get<T>(*this);
        }

        template <typename T>
        [[nodiscard]] const T* TryAs() const {
            return std::get_if<T>(this);
        }
    };

    bool operator==(const Token& lhs, const Token& rhs);
    bool operator!=(const Token& lhs, const Token& rhs);

    std::ostream& operator<<(std::ostream& os, const Token& rhs);

    class LexerError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Lexer {
    public:
        explicit Lexer(std::istream& input);

        // Возвращает ссылку на текущий токен или token_type::Eof, если поток токенов закончился
        [[nodiscard]] const Token& CurrentToken() const;

        // Возвращает следующий токен, либо token_type::Eof, если поток токенов закончился
        Token NextToken();

        // Если текущий токен имеет тип T, метод возвращает ссылку на него.
        // В противном случае метод выбрасывает исключение LexerError
        template <typename T>
        const T& Expect() const {
            using namespace std::literals;
            if (CurrentToken().Is<T>()) {
                return CurrentToken().As<T>();
            }
            throw LexerError("Not implemented"s);
        }

        // Метод проверяет, что текущий токен имеет тип T, а сам токен содержит значение value.
        // В противном случае метод выбрасывает исключение LexerError
        template <typename T, typename U>
        void Expect(const U& value) const {
            using namespace std::literals;
            if(Expect<T>().value != value) {
                throw LexerError("Not implemented"s);
            }
        }

        // Если следующий токен имеет тип T, метод возвращает ссылку на него.
        // В противном случае метод выбрасывает исключение LexerError
        template <typename T>
        const T& ExpectNext() {
            using namespace std::literals;
            if (NextToken().Is<T>()) {
                return CurrentToken().As<T>();
            }
            throw LexerError("Not implemented"s);
        }

        // Метод проверяет, что следующий токен имеет тип T, а сам токен содержит значение value.
        // В противном случае метод выбрасывает исключение LexerError
        template <typename T, typename U>
        void ExpectNext(const U& value) {
            using namespace std::literals;
            if (ExpectNext<T>().value != value) {
                throw LexerError("Not implemented"s);
            }
        }

    private:
        size_t nomber_of_curent_token = 0;

        std::string word = ""s;
        std::string math_word = ""s;
        std::string str_line = ""s;

        size_t offset_buff = 0;
        size_t offset = 0;
        size_t previous_line_offset = 0;
        
        bool is_new_line = true;
        bool is_first_line = true;
        bool exception_symbol = false;
        bool single_quote_open = false;
        bool double_quote_open = false;
        bool coment = false;
        

        const std::set<std::string> keywords_ {
            "class"s, "return"s, "if"s, "else"s, 
            "def"s, "print"s,
            "and"s , "or"s , "not"s , "=="s , "!="s ,
            "<="s, ">="s, "None"s, "True"s, "False"s
        };

        const std::set<char> trigger_symbols_{
            ' ', ',', ':', '.', '(', ')', '\\'
        };

        const std::set<char> math_symbols_{
            '+', '-', '*', '/', '=', '<', '>', '!'
        };

        std::vector<Token> lexer_data;

        bool IsNumber(char ch) const;
        bool IsLegalSymbolForId(char ch) const;
        void NewLineProcessing();
        void QuotesProcessing(const char& symbol, bool& open);
        bool CheckOffset(const char& symbol);
        std::string Unescape(const char symbol);

        void WriteWords(std::string& word, std::string& math_word);
        void WriteWord(std::string& word);
        void WriteToString(const char symbol);
        void WriteMathWord(std::string& math_word);

        void ParsingData(std::istream& input);
        Token ParseToken(std::string_view word);
    };

}  // namespace parse