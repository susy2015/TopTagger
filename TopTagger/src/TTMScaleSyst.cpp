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
        std::string sfHist =  cfgDoc->get("sfHist", iVar, localCxt, "");
        std::string sfVar  =  cfgDoc->get("sftVar",  iVar, localCxt, "");
            
        //Get syst factor histogram 
        TH1* h_temp = static_cast<TH1*>(file->Get(sfHist.c_str()));
        
        //Check that the histogram pointer is valid
        if(!h_temp)
        {
            THROW_TTEXCEPTION("Scale factor histogram  \"" + sfHist + "\" is not valid");
        }
        
        //if it is a non empty string save in vector
        if(sfHist.size() > 0 && sfVar.size() > 0)
        {
            keepLooping = true;

            scaleFactorHists_.emplace(sfHist, h_temp);
        }
        else if((sfHist.size() == 0) ^ (sfVar.size() == 0)) //^ here is to catch cases where one, but not both, variables are undefined
        {
            THROW_TTEXCEPTION("Scale factor histogram  \"" + sfHist + "\" with variable \"" + sfVar + "\" is not valid");
        }

        //Set the variable assigned to the histogram
        variables_[sfHist] = parseVariable(sfVar);

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
        std::string systVar  =  cfgDoc->get("systVar",  iVar, localCxt, "");
            
        //Get syst factor histogram 
        TH1* h_temp = static_cast<TH1*>(file->Get(systHist.c_str()));
        
        //Check that the histogram pointer is valid
        if(!h_temp)
        {
            THROW_TTEXCEPTION("Systematic histogram  \"" + systHist + "\" is not valid");
        }
        
        //if it is a non empty string save in vector
        if(systHist.size() > 0 && systVar.size() > 0)
        {
            keepLooping = true;

            systematicHists_.emplace(systHist, h_temp);
        }
        else if((systHist.size() == 0) ^ (systVar.size() == 0)) //^ here is to catch cases where one, but not both, variables are undefined
        {
            THROW_TTEXCEPTION("Systematic histogram  \"" + systHist + "\" with variable \"" + systVar + "\" is not valid");
        }

        //Set the variable assigned to the histogram
        variables_[systHist] = parseVariable(systVar);

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
                const auto& sfHist = scaleFactorHists_["SF_SIG_" + topFlavor_];
                const auto& varGetter = variables_["SF_SIG_" + topFlavor_];
                double scaleFactor = sfHist->GetBinContent(sfHist->FindBin(varGetter(topCand)));
                topCand.setMCScaleFactor(scaleFactor);

                for(auto& syst : systematicHists_)
                {
                    if(syst.first.find("SYST_SIG") != std::string::npos)
                    {
                        const auto& systHist = syst.second;
                        const auto& varGetter = variables_[syst.first];
                        double systVal = systHist->GetBinContent(systHist->FindBin(varGetter(topCand)));
                        topCand.setSystematicUncertainty(syst.first, systVal);
                    }
                }
            }
            else
            {
                auto& sfHist = scaleFactorHists_["SF_BG_" + topFlavor_];
                auto& varGetter = variables_["SF_BG_" + topFlavor_];
                double scaleFactor = sfHist->GetBinContent(sfHist->FindBin(varGetter(topCand)));
                topCand.setMCScaleFactor(scaleFactor);

                for(auto& syst : systematicHists_)
                {
                    if(syst.first.find("SYST_BG") != std::string::npos)
                    {
                        const auto& systHist = syst.second;
                        const auto& varGetter = variables_[syst.first];
                        double systVal = systHist->GetBinContent(systHist->FindBin(varGetter(topCand)));
                        topCand.setSystematicUncertainty(syst.first, systVal);
                    }
                }
            }
        }
    }
}

std::function<float(const TopObject&)> TTMScaleSyst::parseVariable(const std::string& var)
{
    if(     var.compare("pt") == 0)   return [](const TopObject& topCand) { return topCand.p().Pt(); } ;
    else if(var.compare("eta") == 0)  return [](const TopObject& topCand) { return topCand.p().Eta(); } ;
    else if(var.compare("phi") == 0)  return [](const TopObject& topCand) { return topCand.p().Phi(); } ;
    else if(var.compare("mass") == 0) return [](const TopObject& topCand) { return topCand.p().M(); } ;
    else 
    {
        THROW_TTEXCEPTION("Unknown TopObject variable: \"" + var + "\"!");
    }
}
