#include "SampleAnalyzer/User/Analyzer/basic_plots.h"

using namespace MA5;
using namespace std;

// -----------------------------------------------------------------------------
// Initialize
// function called one time at the beginning of the analysis
// -----------------------------------------------------------------------------
bool basic_plots::Initialize(const MA5::Configuration& cfg,
		const std::map<std::string, std::string>& parameters) {
	cout << "BEGIN Initialization" << endl;
	mass_y = new TH1F("mass_y", "Mass of y", 50, 0, 10000);
	met_mc = new TH1F("met_mc", "MET (mc)", 50, 0, 10000);
	met_rec = new TH1F("met_rec", "MET (rec)", 50, 0, 10000);
	pt1 = new TH1F("pt1", "Pt of Higgs 1", 50, 0, 5000);
	pt2 = new TH1F("pt2", "Pt of Higgs 2", 50, 0, 5000);
	eta1 = new TH1F("eta1", "Eta of Higgs 1", 50, -4, 4);
	eta2 = new TH1F("eta2", "Eta of Higgs 2", 50, -4, 4);
	noOfJets30 = new TH1I("noOfJets30", "No. of Jets (pT > 30 GeV)", 50, 100, 200);
	noOfJets50 = new TH1I("noOfJets50", "No. of Jets (pT > 50 GeV)", 50, 100, 200);
	rec_mass = new TH1F("rec_mass", "Reconstructed Mass of y", 50, 0, 10000);
	eta_bjets = new TH1F("eta_bjets", "Eta of b-Jets", 50, -4, 4);
	eta_leading_bjet = new TH1F("eta_leading_bjet", "Eta of Leading b-Jet", 50, -4, 4);
	phi_bjets = new TH1F("phi_bjets", "Phi of b-Jets", 50, 0, 8);
	phi_leading_bjet = new TH1F("phi_leading_bjet", "Phi of Leading b-Jet", 50, 0, 8);
	cout << "END   Initialization" << endl;
	return true;
}

// -----------------------------------------------------------------------------
// Finalize
// function called one time at the end of the analysis
// -----------------------------------------------------------------------------
void basic_plots::Finalize(const SampleFormat& summary,
		const std::vector<SampleFormat>& files) {
	cout << "BEGIN Finalization" << endl;
	TCanvas c;
	c.SetLogy();
	mass_y->Draw();
	c.SaveAs("Mass_y.png");
	met_mc->Draw();
	c.SaveAs("MET_mc.png");
	met_rec->Draw();
	c.SaveAs("MET_rec.png");
	pt1->Draw();
	c.SaveAs("pt1.png");
	pt2->Draw();
	c.SaveAs("pt2.png");
	eta1->Draw();
	c.SaveAs("eta1.png");
	eta2->Draw();
	c.SaveAs("eta2.png");
	noOfJets30->Draw();
	c.SaveAs("noOfJets30.png");
	noOfJets50->Draw();
	c.SaveAs("noOfJets50.png");
	eta_bjets->Draw();
	c.SaveAs("eta_bJets.png");
	phi_bjets->Draw();
	c.SaveAs("phi_bJets.png");
	eta_leading_bjet->Draw();
	c.SaveAs("eta_leading_bJet.png");
	phi_leading_bjet->Draw();
	c.SaveAs("phi_leading_bJet.png");
	string rootf=files[0].name().substr(files[0].name().find("decayed_"),files[0].name().find(".root")-files[0].name().find("decayed_"));
	TFile f((rootf+".root").c_str(),"RECREATE");
	noOfJets30->SetName((rootf+"_noOfJets30").c_str());
	noOfJets30->Write();
	noOfJets50->SetName((rootf+"_noOfJets50").c_str());
	noOfJets50->Write();
	f.Close();
	cout << "END   Finalization" << endl;
}

// -----------------------------------------------------------------------------
// Execute
// function called each time one event is read
// -----------------------------------------------------------------------------
bool basic_plots::Execute(SampleFormat& sample, const EventFormat& event) {
	bool higgs1_done = false;
	bool higgs2_done = false;
	if (event.mc() != 0) {
		cout << "---------------NEW EVENT-------------------" << endl;

		for (unsigned int i = 0; i < event.mc()->particles().size(); i++) {
			const MCParticleFormat& part = event.mc()->particles()[i];

			if (part.pdgid() == 39)
				mass_y->Fill(part.m());
			if (part.pdgid() == 25) {
				if (!higgs1_done) {
					pt1->Fill(part.pt());
					eta1->Fill(part.eta());
					higgs1_done = true;
				} else if (!higgs2_done) {
					pt2->Fill(part.pt());
					eta2->Fill(part.eta());
					higgs2_done = true;
				}
			}
			met_mc->Fill(event.mc()->MET().pt());
			cout << endl;
		}
	}

	if (event.rec() != 0) {
		cout << "---------------NEW EVENT-------------------" << endl;
		int nJets30=0;
		int nJets50=0;
		vector<const RecJetFormat *> bJets;
		for (unsigned int i = 0; i < event.rec()->jets().size(); i++) {
			const RecJetFormat& jet = event.rec()->jets()[i];
			cout << "----------------------------------" << endl;
			cout << "Jet" << endl;
			cout << "----------------------------------" << endl;
			cout << "jet: index=" << i + 1 << " charge=" << jet.charge()
					<< endl;
			cout << "px=" << jet.px() << " py=" << jet.py() << " pz="
					<< jet.pz() << " e=" << jet.e() << " m=" << jet.m() << endl;
			cout << "pt=" << jet.pt() << " eta=" << jet.eta() << " phi="
					<< jet.phi() << endl;
			cout << "b-tag=" << jet.btag()
					<< " true b-tag (before eventual efficiency)="
					<< jet.true_btag() << endl;
			cout << "EE/HE=" << jet.EEoverHE() << " ntracks=" << jet.ntracks()
					<< endl;
			cout << endl;

			if(jet.eta()<3 && jet.pt()>30)
				nJets30++;
			if(jet.eta()<3 && jet.pt()>50)
				nJets50++;
			if(jet.btag()){
				eta_bjets->Fill(jet.eta());
				phi_bjets->Fill(jet.phi());
				bJets.push_back(&jet);
			}
		}
		noOfJets30->Fill(nJets30);
		noOfJets50->Fill(nJets50);
		met_rec->Fill(event.rec()->MET().pt());

		SORTER->sort(bJets, PTordering);
		if(bJets.size()>0){
			cout<<"No. of b-jets: "<<bJets.size()<<endl;
			eta_leading_bjet->Fill(bJets[0]->eta());
			phi_leading_bjet->Fill(bJets[0]->phi());
		}

	}
	return true;
}


