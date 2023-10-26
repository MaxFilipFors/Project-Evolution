#ifndef EH_HPP
#define EH_HPP

#include <map>
#include <string>

class EvaluationHandler {
public:
    EvaluationHandler();
    void setCorrectNumbers(const std::map<std::string, int>& correct);
    bool evaluateSurvival(const std::map<std::string, int>& predicted);
    double calculateMAE (const std::map<std::string, int>& predicted, 
    const std::map<std::string, int>& correct);
private:
    std::map<std::string, int> correctNumbers;
};

#endif // EH_HPP
