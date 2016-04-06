#include "TopTagger/TopTagger/include/TTMHEPRequirements.h"

#include <cmath>

#include "TopTagger/TopTagger/include/TopTaggerResults.h"

void TTMHEPRequirements::run(TopTaggerResults& ttResults)
{
    std::vector<TopObject>& topCandidates = ttResults.getTopCandidates();
    std::vector<TopObject*>& tops = ttResults.getTops();

    //For now I will be a terrible person and hardcode the cuts
    for(auto& topCand : topCandidates)
    {
        const std::vector<Constituent const *>& jets = topCand.getConstituents();

        //HEP tagger requirements
        bool passHEPRequirments = true;

        const double mW = 80.4;
        const double mt = 173.4;
        const double Rmin = 0.85*mW/mt;
        const double Rmax = 1.25*mW/mt;

        double m123 = topCand.p().M();

        if(jets.size() == 3)
        {
            double m12  = (jets[0]->p() + jets[1]->p()).M();
            double m23  = (jets[1]->p() + jets[2]->p()).M();
            double m13  = (jets[0]->p() + jets[2]->p()).M();

            //Implement HEP mass ratio requirements here
            bool criterionA = 0.2 < atan(m13/m12) &&
                atan(m13/m12) < 1.3 &&
                Rmin < m23/m123 &&
                m23/m123 < Rmax;

            bool criterionB = (pow(Rmin, 2)*(1+pow(m13/m12, 2)) < (1 - pow(m23/m123, 2))) &&
                ((1 - pow(m23/m123, 2)) < pow(Rmax, 2)*(1 + pow(m13/m12, 2)));// &&
            //(m23/m123 > 0.35);

            bool criterionC = (pow(Rmin, 2)*(1+pow(m12/m13, 2)) < (1 - pow(m23/m123, 2))) &&
                ((1 - pow(m23/m123, 2)) < pow(Rmax, 2)*(1 + pow(m12/m13, 2)));// &&
            //(m23/m123 > 0.35);

            passHEPRequirments = criterionA || criterionB || criterionC;
        }
        else if(jets.size() == 2)
        {
            double m23  = jets[0]->p().M();
            double m123 = topCand.p().M();

            //Implement HEP mass ratio requirements here
            passHEPRequirments = Rmin < m23/m123 && m23/m123 < Rmax;
        }

        //mass window on the top candidate mass
        bool passMassWindow = 100 < m123 && m123 < 250;

        //Requirements on b-quarks
        //TERRIBLE HARDCODED CSV THRESHOLD HERE
        int Nb = 0;
        for(const auto& jet : jets) if(jet->getBTagDisc() > 0.81) ++Nb;
        bool bassBrequirements = (Nb <= 1);

        if(passHEPRequirments && passMassWindow && bassBrequirements) tops.push_back(&topCand);
    }
}
