#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include <utility>
#include <regex>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cctype>
#include <cmath>

#include "./soft_string_compare.hh"


void ss_compare::print_substringmatrix(std::string s1, std::string s2, SubstringMatrix sm, std::ostream& printer) {

    // print format specifiers
    const int width = 2;

    std::size_t sm_x = 1 + s1.length();
    std::size_t sm_y = 1 + s2.length();
    for (std::size_t i = 0; i < sm_x; i++) {
        for (std::size_t j = 0; j < sm_y; j++) {
            std::string ps;
            if ((i == 0) && (j == 0)) {
                ps.assign(std::string(" ", width));

            } else if ((i > 0) && (j == 0)) {
                ps = s1[i - 1];

            } else if ((i == 0) && (j > 0)) {
                ps = s2[j - 1];

            } else {
                if (sm[i][j] > 0) {
                    ps = std::to_string(sm[i][j]);

                } else {
                    ps.assign(std::string(" ", width));
                }
            }

            // print format specifiers
            printer << std::setfill(' ') << std::setw(width) << std::setprecision(3) << std::fixed;
            printer << ps << ", ";
        }
        printer << "\n";
    }
}


void ss_compare::print_substringmatrix(std::string s1, std::string s2, ComparisonMatrix cm, std::ostream& printer) {

    // initialize SubstringMatrix
    std::size_t sm_x = 2 + s1.length();
    std::size_t sm_y = 2 + s2.length();
    SubstringMatrix sm(sm_x, std::vector<std::size_t>(sm_y));

    // copy ComparisonMatrix to SubstringMatrix
    for (std::size_t i = 0; i < 2 + sm_x; i++) {
        for (std::size_t j = 0; j < 2 + sm_y; j++) {
            if (cm[i][j]) {
                sm[i][j] = 1;
            } else {
                sm[i][j] = 0;
            }
        }
    }

    // print newly created SubstringMatrix
    print_substringmatrix(s1, s2, sm, printer);
}


void ss_compare::print_substringtuples(std::string s1, std::string s2, std::vector<SubstringTuple> substrings, std::ostream& printer) {

    // sort SubstringTuples by rating
    std::sort(substrings.begin(), substrings.end(), [](const auto a, const auto b) { return (std::get<2>(a) < std::get<2>(b)); });

    // print first string
    printer << "1: " << s1 << "\n";

    for (auto substr = substrings.rbegin(); substr != substrings.rend(); substr++) {
        auto st = *substr;
        auto s1_b1     = std::string(std::get<0>(st), '_');
        auto s1_substr = s1.substr(std::get<0>(st), std::get<2>(st));
        auto s1_b2     = std::string(s1.length() - std::get<0>(st) - std::get<2>(st), '_');

        // print substring with _ to mask first string
        printer << "++ " << s1_b1 << s1_substr << s1_b2 << "\n";
    }

    for (auto substr = substrings.begin(); substr != substrings.end(); substr++) {
        auto st = *substr;
        auto s2_b1     = std::string(std::get<1>(st), '_');
        auto s2_substr = s2.substr(std::get<1>(st), std::get<2>(st));
        auto s2_b2     = std::string(s2.length() - std::get<1>(st) - std::get<2>(st), '_');

        // print substring with _ to mask second string
        printer << "-- " << s2_b1 << s2_substr << s2_b2 << "\n";
    }

    // print second string
    printer << "2: " << s2 << "\n";
}


ss_compare::ComparisonMatrix ss_compare::calculate_comparisonmatrix(std::string s1, std::string s2) {
    
    // initialize ComparisonMatrix
    std::size_t cm_x = s1.length();
    std::size_t cm_y = s2.length();
    std::vector<std::vector<bool>> cm{cm_x, std::vector<bool>(cm_y)};

    // compare strings one byte at a time
    // this will handle UTF-8, but rather crudely
    for(std::size_t i = 0; i < cm_x; i++) {
        for (std::size_t j = 0; j < cm_y; j++) {
            cm[i][j] = (s1[i] == s2[j]);
        }
    }

    return cm;
}


ss_compare::SubstringMatrix ss_compare::calculate_substringmatrix(std::string s1, std::string s2, ComparisonMatrix cm) {

    // initialize SubstringMatrix
    std::size_t sm_x = 2 + s1.length();
    std::size_t sm_y = 2 + s2.length();
    SubstringMatrix sm(sm_x, std::vector<std::size_t>(sm_y));

    // initialize border to 0
    for(std::size_t i = 0; i < sm_x; i++) {
        sm[i][0] = sm[i][sm_y - 1] = 0;
    }

    for(std::size_t j = 0; j < sm_y; j++) {
        sm[0][j] = sm[sm_x - 1][j] = 0;
    }

    // compare strings one byte at a time
    // this will handle UTF-8, but rather crudely
    for(std::size_t i = 0; i < sm_x - 2; i++) {
        for (std::size_t j = 0; j < sm_y - 2; j++) {
            if (cm[i][j]) {
                sm[i + 1][j + 1] = sm[i][j] + 1;

            } else {
                sm[i + 1][j + 1] = 0;
            }
        }
    }

    return sm;
}


std::vector<ss_compare::SubstringTuple> ss_compare::calculate_substringtuples(std::string s1, std::string s2, SubstringMatrix sm, std::size_t minimum_length) {

    // initialize SubstringTuple vector
    std::size_t cs_x = s1.length();
    std::size_t cs_y = s2.length();
    std::vector<SubstringTuple> cs;

    for(std::size_t i = 2; i < cs_x + 2; i++) {
        for (std::size_t j = 2; j < cs_y + 2; j++) {
            if ((sm[i][j] == 0) && (sm[i - 1][j - 1] > 0)) {
                // a substring has ended
                // add SubstringTuple to vector
                auto substring_length = sm[i - 1][j - 1];
                if (substring_length > minimum_length) {
                    SubstringTuple st{i - 1 - substring_length, j - 1 - substring_length, substring_length};
                    cs.push_back(st);
                }
            }
        }
    }

    return cs;
}


std::vector<std::string> ss_compare::words_in_string(std::string s, std::regex r) {
    std::vector<std::string> words;
    
    auto words_begin = std::sregex_iterator(s.begin(), s.end(), r);
    auto words_end = std::sregex_iterator();
    for (auto i = words_begin; i != words_end; i++) {
        std::smatch match = *i;
        std::string match_str = match.str();
        words.push_back(match_str);
    }

    return words;
}


std::map<std::string, std::size_t> ss_compare::calculate_word_frequencies(std::vector<std::string> document){
    std::map<std::string, std::size_t> word_frequencies;
    
    for (auto line = document.begin(); line != document.end(); line++) {
        auto s = *line;

        auto words = words_in_string(s);
        for (std::size_t i = 0; i < words.size(); i++) {
            auto word = words[i];

            auto find_word = word_frequencies.find(word);
            if (find_word == word_frequencies.end()) {
                // add new word
                word_frequencies.insert({word, 1});

            } else {
                // increment word frequency
                find_word->second += 1;
            }
        }
    }

    return word_frequencies;
}


double ss_compare::rate_strings_1(std::string s1, std::string s2, std::vector<SubstringTuple> substrings, double weight) {
    double rating = 0;
    
    for (std::size_t i = 0; i < substrings.size(); i++) {
        auto st = substrings[i];
        auto s1_substr = s1.substr(std::get<0>(st), std::get<2>(st));

        // bias towards same length matches
        double len_factor = 1;
        len_factor = std::min(
            (double) (s1.length()) - s1_substr.length(),
            (double) (s2.length()) - s1_substr.length()
        );
        len_factor = std::min(s1.length(), s2.length()) * std::pow(0.3, len_factor);

        rating += len_factor * std::pow(std::get<2>(st), weight);
    }

    rating = std::pow(rating, 1 / weight);
    return rating;
}


double ss_compare::rate_strings_2(std::string s1, std::string s2, std::map<std::string, std::size_t> word_frequencies) {
    double total_rating = 0;

    auto s1_words = ss_compare::words_in_string(s1);
    auto s2_words = ss_compare::words_in_string(s2);
    for (std::size_t i = 0; i < s1_words.size(); i++) {
        auto s1_word = s1_words[i];

        // try to get s1_word frequency
        std::size_t s1_word_frequency = 1;
        auto s1_word_find = word_frequencies.find(s1_word);
        if (s1_word_find != word_frequencies.end()) {
            s1_word_frequency = s1_word_find->second;
        }

        for (std::size_t j = 0; j < s2_words.size(); j++) {
            auto s2_word = s2_words[j];

            // try to get s2_word frequency
            std::size_t s2_word_frequency = 1;
            auto s2_word_find = word_frequencies.find(s2_word);
            if (s2_word_find != word_frequencies.end()) {
                s2_word_frequency = s2_word_find->second;
            }

            // rate the two words
            auto cm = ss_compare::calculate_comparisonmatrix(s1_word, s2_word);
            auto sm = ss_compare::calculate_substringmatrix(s1_word, s2_word, cm);
            auto substringtuples = ss_compare::calculate_substringtuples(s1_word, s2_word, sm);
            auto rating = ss_compare::rate_strings_1(s1_word, s2_word, substringtuples);

            // bias towards same length matches
            double len_factor = 1;
            len_factor = std::abs((double) (s1_word.length()) - (double) (s2_word.length()));
            len_factor = std::min(s1_word.length(), s2_word.length()) * std::pow(0.6, len_factor);

            // bias towards low frequency words
            double freq_factor = std::pow(1.0 / std::max(s1_word_frequency, s2_word_frequency), 2);

            double rating_increment = rating * len_factor * freq_factor;
            total_rating += rating_increment;
        }
    }

    return total_rating;
}