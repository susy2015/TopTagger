#include "TopTagger/TopTagger/interface/TTMScaleSyst.h"

#include "TopTagger/TopTagger/interface/TopTaggerResults.h"
#include "TopTagger/CfgParser/include/Context.hh"
#include "TopTagger/CfgParser/include/TTException.h"
#include "TopTagger/CfgParser/include/CfgDocument.hh"

#include "TH1.h"
#include "TFile.h"

void TTMScaleSyst::getParameters(const cfg::CfgDocument* cfgDoc, const std::string& localContextName)
{
    //Construct contexts
    //cfg::Context commonCxt("Common");
    cfg::Context localCxt(localContextName);

    topFlavor_                 = cfgDoc->get("topFlavor",        localCxt, "NONE");
    std::string inputFileName  = cfgDoc->get("inputFile",        localCxt, "");

    //Select top type
    if(     topFlavor_.compare("MERGED_TOP") == 0)       topType_ = TopObject::MERGED_TOP;
    else if(topFlavor_.compare("SEMIMERGEDWB_TOP") == 0) topType_ = TopObject::SEMIMERGEDWB_TOP;
    else if(topFlavor_.compare("RESOLVED_TOP") == 0)     topType_ = TopObject::RESOLVED_TOP;
    else if(topFlavor_.compare("MERGED_W") == 0)         topType_ = TopObject::MERGED_W;
    else if(topFlavor_.compare("SEMIMERGEDQB_TOP") == 0) topType_ = TopObject::SEMIMERGEDQB_TOP;
    else
    {
        THROW_TTEXCEPTION("Top flavor \"" + topFlavor_ + "\" is not supported!");
    }

    //Get histogram names and variables for scale factor and systematic evaluation
    std::vector<std::pair<std::string, std::string>> systematicNames;

    //Magic incantation to disassociate histograms from their file 
    TH1::AddDirectory(false);

    //Get the necessary and avaliable histograms from the input root file
    std::unique_ptr<TFile> file(TFile::Open(inputFileName.c_str()));

    //Check that the file pointer is valid
    if(!file.get())
    {
        THROW_TTEXCEPTION("File \"" + inputFileName + "\" is not valid");
    }

    //Get scale factor histograms 
    int iVar = 0;
    bool keepLooping;
    do
    {
        keepLooping = false;

        //Get variable name
        std::string sfHist =  cfgDoc->get("sfHist",  iVar, localCxt, "");
        std::string sfVar  =  cfgDoc->get("sfVar",   iVar, localCxt, "pt");
        std::string label  =  cfgDoc->get("sfLabel", iVar, localCxt, "");
        
        //if it is a non empty string save in vector
        if(sfHist.size() > 0 && sfVar.size() > 0)
        {
            keepLooping = true;

            //Get syst factor histogram 
            TH1* h_temp = static_cast<TH1*>(file->Get(sfHist.c_str()));

            //Check that the histogram pointer is valid
            if(!h_temp)
            {
                THROW_TTEXCEPTION("Scale factor histogram  \"" + sfHist + "\" is not valid");
            }

            if(label.size() == 0) label = h_temp->GetName();

            scaleFactorHists_.emplace(label, h_temp);

            //Set the variable assigned to the histogram
            variables_[label] = parseVariable(sfVar);
        }

        ++iVar;
    }
    while(keepLooping);

    //Get systematic histograms
    iVar = 0;
    do
    {
        keepLooping = false;

        //Get variable name
        std::string systHist =  cfgDoc->get("systHist", iVar, localCxt, "");
        std::string systVar  =  cfgDoc->get("systVar",  iVar, localCxt, "pt");
        std::string label  =    cfgDoc->get("systLabel", iVar, localCxt, "");

        //if it is a non empty string save in vector
        if(systHist.size() > 0 && systVar.size() > 0)
        {
            keepLooping = true;

            //Get syst factor histogram 
            TH1* h_temp = static_cast<TH1*>(file->Get(systHist.c_str()));
        
            //Check that the histogram pointer is valid
            if(!h_temp)
            {
                THROW_TTEXCEPTION("Systematic histogram  \"" + systHist + "\" is not valid");
            }

            if(label.size() == 0) label = h_temp->GetName();
        
            systematicHists_.emplace(label, h_temp);

            //Set the variable assigned to the histogram
            variables_[label] = parseVariable(systVar);

        }

        ++iVar;
    }
    while(keepLooping);

    file->Close();
}

void TTMScaleSyst::run(TopTaggerResults& ttResults)
{
    //Get final tops of the desired type    
    //Check that tops of this type actually exist
    auto& topCands = ttResults.getTopCandidates();

    //Set scale factor for each top
    for(TopObject& topCand : topCands)
    {
        if(topCand.getType() == topType_)
        {
            if(topCand.getBestGenTopMatch())
            {
                fillSfAndSyst(topCand, ttResults, "hSF_SIG", "hSYST_SIG");
            }
            else
            {
                fillSfAndSyst(topCand, ttResults, "hSF_BG", "hSYST_BG");
            }
        }
    }
}

std::function<float(const TopObject&, const TopTaggerResults&)> TTMScaleSyst::parseVariable(const std::string& var)
{
    if(     var.compare("pt") == 0)               return [](const TopObject& topCand, const TopTaggerResults& ttr) { (void)ttr;     return topCand.p().Pt(); } ;
    else if(var.compare("eta") == 0)              return [](const TopObject& topCand, const TopTaggerResults& ttr) { (void)ttr;     return topCand.p().Eta(); } ;
    else if(var.compare("phi") == 0)              return [](const TopObject& topCand, const TopTaggerResults& ttr) { (void)ttr;     return topCand.p().Phi(); } ;
    else if(var.compare("mass") == 0)             return [](const TopObject& topCand, const TopTaggerResults& ttr) { (void)ttr;     return topCand.p().M(); } ;
    else if(var.compare("njet_pt30_eta2p4") == 0) return [](const TopObject& topCand, const TopTaggerResults& ttr) { (void)topCand; return ttr.countAK4Jets(30, 2.4); } ;
    else 
    {
        THROW_TTEXCEPTION("Unknown TopObject variable: \"" + var + "\"!");
    }
}

void TTMScaleSyst::fillSfAndSyst(TopObject& topCand, const TopTaggerResults& ttResults, const std::string& sfName, const std::string& systPrefix)
{
    const auto& sfHist = scaleFactorHists_[sfName];
    const auto& varGetter = variables_[sfName];

    double scaleFactor = sfHist->GetBinContent(sfHist->FindBin(varGetter(topCand, ttResults)));
    topCand.setMCScaleFactor(scaleFactor);

    for(auto& syst : systematicHists_)
    {
        if(syst.first.find(systPrefix) != std::string::npos)
        {
            std::string label(syst.first.substr(systPrefix.size() + 1));
            const auto& systHist = syst.second;
            const auto& varGetter = variables_[syst.first];
            double systVal = systHist->GetBinContent(systHist->FindBin(varGetter(topCand, ttResults)));
            topCand.setSystematicUncertainty(label, systVal);
        }
    }
    
}
