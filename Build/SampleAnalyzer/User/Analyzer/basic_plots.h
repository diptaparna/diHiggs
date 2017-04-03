#ifndef analysis_basic_plots_h
#define analysis_basic_plots_h

#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"

namespace MA5
{
class basic_plots : public AnalyzerBase
{
  INIT_ANALYSIS(basic_plots,"basic_plots")

 public:
  virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters);
  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);
  virtual bool Execute(SampleFormat& sample, const EventFormat& event);

 private:
  TH1F *mass_y;
  TH1F *met_mc;
  TH1F *met_rec;
  TH1F *pt1;
  TH1F *pt2;
  TH1F *eta1;
  TH1F *eta2;
  TH1I *noOfJets30;
  TH1I *noOfJets50;
  TH1F *rec_mass;
  TH1F *eta_bjets;
  TH1F *phi_bjets;
  TH1F *eta_leading_bjet;
  TH1F *phi_leading_bjet;
};
}

#endif
