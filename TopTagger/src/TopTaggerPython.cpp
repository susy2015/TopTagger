#include "TopTagger/TopTagger/interface/TopTaggerPython.h"

#include "TLorentzVector.h"

namespace ttPython
{
    /**
     *Python compatibility function.  This function creates an std::vector<Constituent> from python objects as read from nanoAOD
     */
    std::vector<Constituent> packageConstituentsAK4(PyObject* jet_pt, PyObject* jet_eta, PyObject* jet_phi, PyObject* jet_mass, PyObject* jet_btag, PyDictObject* extraVars)
    {
        //Create std::vector<TLorentzVector> for the jet 4-vectors 
        Py_buffer_wrapper<float> pt  ( jet_pt );
        Py_buffer_wrapper<float> eta ( jet_eta );
        Py_buffer_wrapper<float> phi ( jet_phi );
        Py_buffer_wrapper<float> mass( jet_mass );

        std::vector<TLorentzVector> jetLV(pt.size());
        for(unsigned int iJet = 0; iJet < pt.size(); ++iJet)
        {
            jetLV[iJet].SetPtEtaPhiM(pt[iJet], eta[iJet], phi[iJet], mass[iJet]);
            std::cout << pt[iJet] << eta[iJet] << phi[iJet] << mass[iJet] << std::endl;
        }

        //Create constituent creator
        try
        {
            Py_buffer_wrapper<float> btag(jet_btag);
            ttUtility::ConstAK4Inputs<float, Py_buffer_wrapper<float>, Py_buffer_wrapper<int> > ak4ConstInputs(jetLV, btag);
            return ttUtility::packageConstituents<ttUtility::ConstAK4Inputs<float, Py_buffer_wrapper<float>, Py_buffer_wrapper<int> >& >(ak4ConstInputs);
        }
        catch(const TTException& e)
        {
            std::cout << e << std::endl;
        }

        return std::vector<Constituent>();
    }
}
