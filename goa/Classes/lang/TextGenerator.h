//
//  TextGenerator.h
//  safari
//
//  Created by Srikanth Talapadi on 05/08/16.
//
//

#include "cocos2d.h"

#ifndef TextGenerator_h
#define TextGenerator_h
#include "cocos2d.h"

class TextGenerator {
public:
    enum class P_O_S
    {
        ANY = 0,
        NOUN = 1,
        PRONOUN = 2,
        ADJECTIVE = 3,
        VERB = 4,
        ADVERB = 5,
        PREPOSITION = 6,
        CONJUNCTION = 7,
        INTERJECTION = 8,
        ARTICLE = 9
    };
    
    struct Phonic {
        std::string phonic_string;
        std::string pronunciation;
        int number_of_segments;
        int fixed_index;
    };
    
    static TextGenerator* getInstance();
    
    std::vector<std::vector<std::string>> generateMatrix(std::string const& word, int numRows, int numCols);
    std::string generateAWord(int level = 1, int length = 0);
    int getNumGraphemesInString(std::string const& word);
    std::vector<std::string> getGraphemes(std::string word);
    std::string generateASentence(int level = 1);
    std::vector<std::string> getAllChars();
    std::vector<std::string> getValidCombinations(std::string const& chars, int maxLength);
    std::map<std::string, std::string> getSynonyms(int maxNum, int level = 1);
    std::map<std::string, std::string> getAntonyms(int maxNum, int level = 1);
    std::map<std::string, std::string> getHomonyms(int maxNum, int level = 1);
    std::map<std::string, std::map<std::string, std::string>> getInitialSyllableWords(int maxNum, int maxChoices, int level = 1);
    std::vector<std::string> getWords(TextGenerator::P_O_S partOfSpeech, int maxLength, int level = 1);
    std::vector<std::string> getOrderedConcepts(int level = 1);
    std::vector<std::vector<std::pair<std::string, TextGenerator::P_O_S>>> getSentenceWithPOS(TextGenerator::P_O_S partOfSpeech, int maxLength, int level = 1);
    std::map<std::string, std::string> getSingularPlurals(int maxNum, int level = 1);
    
    std::string getLang();
	std::vector<std::string> wordsWithGivenLetter(std::string);
    
    std::string translateString(std::string input);
    std::string getPhonicForLevel(int level);
    std::map<std::string, std::string> getWordsForPhonic(int level, int maxNum);
    std::map<std::string, std::string> getWordsNotForPhonic(int level, int maxNum);
    std::string getInitialForLevel(int level);
    std::map<std::string, std::string> getWordsForInitial(int level, int maxNum);
    std::map<std::string, std::string> getWordsNotForInitial(int level, int maxNum);
    Phonic getPhonicSegmentForLevel(int level);
    std::vector<std::vector<std::string>> getSegmentsForPhonic(int level, int maxNum);
    std::vector<std::vector<std::string>> getSegmentsNotForPhonic(int level, int maxNum);
protected:
    std::map<int, int> getRandomLocations(int numLoc, int totalNum);
    std::map<std::string, std::map<std::string, std::string>> getMapOfWords(std::string const& type, int maxNum, int maxChoices, int level);
    std::map<std::string, std::string> getPairs(std::string const& type, int maxNum, int level);
    std::map<std::string, std::string> getPairsNotForLevel(std::string const& type, int maxNum, int level);
    std::string getSingle(std::string const& type, int level, int length = 0);
    std::vector<std::string> getWordList(std::string const& type, int level, int maxNum = 0);
    std::vector<std::vector<std::string>> getListsOfWords(std::string const& type, int level, int maxNum = 0);
};

#endif /* TextGenerator_h */
