
#ifndef __Simulator__RuntimeCCSImpl__
#define __Simulator__RuntimeCCSImpl__

#include "RuntimeProtocol.h"

class RuntimeCCSImpl : public RuntimeProtocol
{
public:
    static RuntimeCCSImpl *create();
    
    void onStartDebuger(const rapidjson::Document& dArgParse, rapidjson::Document& dReplyParse) override;
    void onReload(const rapidjson::Document &dArgParse, rapidjson::Document &dReplyParse) override;
    void startScript(const std::string &file) override;

private:
    void loadCSDProject(const std::string& file);
    void loadCSBProject(const std::string& file);
};

#endif /* defined(__Simulator__RuntimeCCSImpl__) */
