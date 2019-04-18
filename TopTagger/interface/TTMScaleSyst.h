#ifndef TTMOVERLAPRESOLUTION_H
#define TTMOVERLAPRESOLUTION_H

#include "TopTagger/TopTagger/interface/TTModule.h"
#include "TopTagger/TopTagger/interface/TopObject.h"

#include <string>
#include <memory>
#include <map>
#include <functional>

class TopTaggerResults;
class TH1;

/**
 *This module is called to load the scale factors and systematic uncertainties.
 *
 *@param topFlavor (string) The flavor of the TopObject which will have its systematics added (Options: MERGED_TOP, SEMIMERGEDWB_TOP, RESOLVED_TOP, MERGED_W, SEMIMERGEDQB_TOP)
 *@param inputFile (string) The root file containing the scale factors and systematic uncertainties
 */
class TTMScaleSyst : public TTModule
{
private:
    /// Store the type of TopObject to operate on
    TopObject::Type topType_;
    /// Store the type of TopObject in string format
    std::string topFlavor_;
    /// Set of scale factor histograms
    std::map<std::string, std::unique_ptr<TH1>> scaleFactorHists_;
    /// Set of histograms containing the systeamtic uncertainties 
    std::map<std::string, std::unique_ptr<TH1>> systematicHists_;
    /// Map containing the variable to use for each SF and systematic histogram
    std::map<std::string, std::function<float(const TopObject&)>> variables_;

    std::function<float(const TopObject&)> parseVariable(const std::string&);

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMScaleSyst);

#endif
