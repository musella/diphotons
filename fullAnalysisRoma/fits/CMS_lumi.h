#include "TPad.h"
#include "TLatex.h"
#include "TLine.h"
#include "TBox.h"
// /#include "TASImage.h"

//
// Global variables
//

TString cmsText     = "CMS";
float cmsTextFont   = 61;  // default is helvetic-bold

bool writeExtraText = true;
//TString extraText   = "Preliminary";
TString extraText   = "Simulation";   // x signal model plots
float extraTextFont = 52;  // default is helvetica-italics

bool writeExtraText2 = true;
TString extraText2   = "EBEB";   // x Zplots
//TString extraText2   = "EE";       // x T&P 
float extraTextFont2 = 42;  

// text sizes and text offsets with respect to the top frame
// in unit of the top margin size
float lumiTextSize     = 0.5;    // 0.5 x Zplots
//float lumiTextSize     = 0.3;  // 0.3 x T&P
float lumiTextOffset   = 0.2;    // 0.2 x Zplots
//float lumiTextOffset   = -0.3;   // -0.3 x T&P   
float cmsTextSize      = 0.6;    // 0.6 x Zplots
//float cmsTextSize      = 0.4;    // 0.4 x T&P
float cmsTextOffset    = 0.1;  // only used in outOfFrame version
 
float relExtraDY2 = 1.8;       // 1.8 x Zplots
//float relExtraDY2 = 2.1;     // 2.1 x T&P
float relExtraDX2 = -9.5;      // -0.5 x Zplots e T&P; -9.5 x signal model plots 

float relPosX    = 0.045;
float relPosY    = 0.035;
float relExtraDY = -1.4;   // -1.4 x Zplots
//float relExtraDY = -0.6;   // -0.6 x T&P
float relExtraDX = -2.;

// ratio of "CMS" and extra text size
float extraOverCmsTextSize   = 0.76;
 
//TString lumi_13TeV = "2.7 fb^{-1}";       // x 3.8T
TString lumi_13TeV = "";                // x signal model plots
//TString lumi_13TeV = "0.6 fb^{-1}";     // x 0T
TString lumi_8TeV  = "19.7 fb^{-1}";
TString lumi_7TeV  = "5.1 fb^{-1}";

bool drawLogo      = false;

void CMS_lumi( TPad* pad, int iPeriod=3, int iPosX=10 );

