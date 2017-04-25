/*
Macro for diHiggs Analysis by Diptaparna Biswas

root -l examples/diHiggs.C'("/afs/cern.ch/work/d/dbiswas/public/decayed_2500_200PU.root")'
*/

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif

//------------------------------------------------------------------------------

void diHiggs(const char *inputFile)
{
  gSystem->Load("libDelphes");

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile);

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Int_t numberOfEntries = treeReader->GetEntries();

  //// Gen-level H->bb
  TClonesArray *b_part = treeReader->UseBranch("Particle");

  // Get pointers to branches used in this analysis
  TClonesArray *branchJet = treeReader->UseBranch("JetPUPPI");

  // Book histograms
  //
  TFile* fout = new TFile("fout.root", "RECREATE");
  fout->cd();
  TH2D *h_pth_drbb = new TH2D("h_pth_drbb", ";p_{T} (H) [GeV];#Delta R(b,b);", 100, 0., 2000., 40, 0., 4.) ; 
  TH1D *h_pth = new TH1D("h_pth", ";p_{T} (H) [GeV];;", 100, 0., 2000.); 
  TH1D *h_yh = new TH1D("h_yh", ";y (H);;", 40, -4., 4.); 
  TH1D *h_etah = new TH1D("h_etah", ";#eta (H);;", 40, -4., 4.); 

  TH1D *h_ptb = new TH1D("h_ptb", ";p_{T} (b/ #overline{b}) [GeV];;", 100, 0., 2000.); 
  TH1D *h_etab = new TH1D("h_etab", ";#eta (b/ #overline{b});;", 40, -4., 4.); 

  TH1F *histInvM1 = new TH1F("histInvM1", ";M (jet 1) [GeV];;", 100, 0, 500);
  TH1F *histInvM2 = new TH1F("histInvM2", ";M (jet 2) [GeV];;", 100, 0, 500);
  TH1F *etaLeadingBjet = new TH1F("etaLeadingBjet", ";#eta (leading b jet);;", 40, -4, 4);
  
  Int_t passedEvents=0;
  // Loop over all events
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  {
    // Load selected branches with data from specified event
    treeReader->ReadEntry(entry);

    std::vector<TLorentzVector> p4_qb, p4_aqb ; 
    for(int i=0; i < b_part->GetEntriesFast(); ++i) { 

      GenParticle *particle = (GenParticle*)b_part->At(i);     

      int pid(particle->PID), status(particle->Status);
      double pt(particle->PT), eta(particle->Eta), phi(particle->Phi), mass(particle->Mass);

      if ( status == 23 ) {
        if ( pid == 5 ) {
          TLorentzVector p4; 
          p4.SetPtEtaPhiM(particle->PT, particle->Eta, particle->Phi, particle->Mass) ;
          p4_qb.push_back(p4) ; 
        }
        else if ( pid == -5 ) {
          TLorentzVector p4; 
          p4.SetPtEtaPhiM(particle->PT, particle->Eta, particle->Phi, particle->Mass) ;
          p4_aqb.push_back(p4) ; 
        }
      }

    }
  
    std::vector<TLorentzVector>::iterator it_qb, it_aqb;
    for (it_qb = p4_qb.begin(); it_qb != p4_qb.end(); ++it_qb) { 
      for (it_aqb = p4_aqb.begin(); it_aqb != p4_aqb.end(); ++it_aqb) { 
        double minv(((*it_qb) + (*it_aqb)).Mag());
        double drbb((*it_qb).DeltaR(*it_aqb));
        double ptbb(((*it_qb) + (*it_aqb)).Pt());
        double ybb(((*it_qb) + (*it_aqb)).Rapidity());
        double etabb(((*it_qb) + (*it_aqb)).Eta());
        if (minv > 123. && minv < 126.) {
          h_pth_drbb->Fill(ptbb, drbb) ;  
          h_pth -> Fill(ptbb) ;
          h_yh -> Fill(ybb) ;
          h_etah -> Fill(etabb) ;
          h_ptb -> Fill(it_qb->Pt()) ;
          h_ptb -> Fill(it_aqb->Pt()) ;
          h_etab -> Fill(it_qb->Eta()) ;
          h_etab -> Fill(it_aqb->Eta()) ;
        }
      } 
    }

    // If event contains at least 2 jets
    if(branchJet->GetEntries() >= 2)
    {
      Jet *jet1 = (Jet*) branchJet->At(0);
      Jet *jet2 = (Jet*) branchJet->At(1);
      
      if(jet1->Flavor != 5 || jet2->Flavor != 5)
		continue;
/*
      if(jet1->PT <= 300 || jet2->PT <= 300 || abs(jet1->Eta) >= 3 || abs(jet2->Eta) >=3)
        continue;

      if(abs(jet1->Eta - jet2->Eta) >= 1.3)
        continue;

      if(jet1->SoftDroppedP4[0].M()<=105 || jet2->SoftDroppedP4[0].M()>=135)
        continue;

      if(jet1->SoftDroppedP4[0].M()<=105 || jet2->SoftDroppedP4[0].M()>=135)
        continue;
*/
      passedEvents++;
      histInvM1->Fill(jet1->Mass);
      histInvM2->Fill(jet2->Mass);
      etaLeadingBjet->Fill(jet1->Eta);
    }

  }

  cout<<"Total Events: "<<numberOfEntries<<endl;
  cout<<"Passed Events: "<<passedEvents<<endl;
  cout<<"Efficiency: "<<(double)passedEvents/numberOfEntries<<endl;

  // Show resulting histograms
  TCanvas c;
  c.SetLogy();

  h_pth_drbb->Draw();
  c.SaveAs(h_pth_drbb->GetName()+ TString(".png")) ;
  c.Clear();

  h_pth->Draw();
  c.SaveAs(h_pth->GetName()+ TString(".png")) ;
  c.Clear();

  h_yh->Draw();
  c.SaveAs(h_yh->GetName()+ TString(".png")) ;
  c.Clear();

  h_etah->Draw();
  c.SaveAs(h_etah->GetName()+ TString(".png")) ;
  c.Clear();

  h_ptb->Draw();
  c.SaveAs(h_ptb->GetName()+ TString(".png")) ;
  c.Clear();

  h_etab->Draw();
  c.SaveAs(h_etab->GetName()+ TString(".png")) ;
  c.Clear();

  histInvM1->Draw();
  c.SaveAs(histInvM1->GetName()+ TString(".png")) ;
  c.Clear();

  histInvM2->Draw();
  c.SaveAs(histInvM2->GetName()+ TString(".png")) ;
  c.Clear();
  
  etaLeadingBjet->Draw();
  c.SaveAs(etaLeadingBjet->GetName()+ TString(".png")) ;
  c.Clear();
  
  fout->Write();
  fout->Close();

}

