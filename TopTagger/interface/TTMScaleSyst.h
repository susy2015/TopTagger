#ifndef TTMOVERLAPRESOLUTION_H
#define TTMOVERLAPRESOLUTION_H

#include "TopTagger/TopTagger/include/TTModule.h"
#include "TopTagger/TopTagger/include/TopObject.h"

#include <string>
#include <memory>
#include <map>

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
    /// A histogram containing the scale factor information
    std::unique_ptr<TH1> scaleFactorHist_;
    /// Set of histograms containing the systeamtic uncertainties 
    std::map<std::string, std::unique_ptr<TH1>> systematicHists_;

public:
    void getParameters(const cfg::CfgDocument*, const std::string&);
    void run(TopTaggerResults&);
};
REGISTER_TTMODULE(TTMScaleSyst);

#endif
