#include "TopTagger/TopTagger/include/TTMScaleSyst.h"

#include "TopTagger/TopTagger/include/TopTaggerResults.h"
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

    std::string topFlavor           = cfgDoc->get("topFlavor",        localCxt, "NONE");
    std::string inputFileName       = cfgDoc->get("inputFile",        localCxt, "");
    std::string scaleFactorHistName = cfgDoc->get("scaleFactorHist",  localCxt, "");
    std::string scaleFactorHistVar  = cfgDoc->get("scaleFactorVar",   localCxt, "");

    //Get histogram names and variables for systematic evaluation
    std::vector<std::pair<std::string, std::string>> systematicNames;

    //Sort the top vector for overlap resolution
    if(topFlavor.compare("MERGED_TOP") == 0)
    {
        topType_ = TopObject::MERGED_TOP;
    }
    else if(topFlavor.compare("SEMIMERGEDWB_TOP") == 0)
    {
        topType_ = TopObject::SEMIMERGEDWB_TOP;
    }
    else if(topFlavor.compare("RESOLVED_TOP") == 0)
    {
        topType_ = TopObject::RESOLVED_TOP;
    }
    else if(topFlavor.compare("MERGED_W") == 0)
    {
        topType_ = TopObject::MERGED_W;
    }
    else if(topFlavor.compare("SEMIMERGEDQB_TOP") == 0)
    {
        topType_ = TopObject::SEMIMERGEDQB_TOP;
    }
    else
    {
        THROW_TTEXCEPTION("Invalid TopObject type: " + topFlavor);
    }

    //Get the necessary and avaliable histograms from the input root file
    std::unique_ptr<TFile> file(TFile::Open(inputFileName.c_str()));

    //Check that the file pointer is valid
    if(!file.get())
    {
        THROW_TTEXCEPTION("File \"" + inputFileName + "\" is not valid");
    }

    //Get scale factor histogram 
    scaleFactorHist_.reset(static_cast<TH1*>(file->Get(scaleFactorHistName.c_str())));

    //Check that the histogram pointer is valid
    if(!scaleFactorHist_.get())
    {
        THROW_TTEXCEPTION("Scale factor histogram  \"" + scaleFactorHistName + "\" is not valid");
    }

    //Get systematic histograms
    int iVar = 0;
    bool keepLooping;
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
        ++iVar;
    }
    while(keepLooping);

    file->Close();
}

void TTMScaleSyst::run(TopTaggerResults& ttResults)
{
    //Get final tops of the desired type    
    //Check that tops of this type actually exist
    auto& topsByType = ttResults.getTopsByType();
    if(topsByType.find(topType_) != topsByType.end())
    {
        //Nothing to do for this event
        return;
    }
    std::vector<TopObject*>& tops = ttResults.getTopsByType()[topType_];

    //Set scale factor for each top
    for(TopObject* top : tops)
    {
        
    }
}
